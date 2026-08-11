#include "EMS_strategy.h"

#include <stddef.h>
#include <string.h>

/* 当前系统由24只2V/500Ah铅炭电池单体串联组成。 */
#define EMS_CELL_COUNT                    24.0f

/* Q/CY 3363.01-2025规定的25℃均充单体参考电压。 */
#define EMS_EQUALIZE_CELL_VOLTAGE         2.45f

/* Q/CY 3363.01-2025规定的25℃浮充单体参考电压。 */
#define EMS_FLOAT_CELL_VOLTAGE            2.30f

/* Q/CY 3363.01-2025规定的完全充电补充阶段单体参考电压。 */
#define EMS_FULL_CELL_VOLTAGE             2.50f

/* 前次放电量小于该值时，不计算充电系数，避免小数值放大误差。 */
#define EMS_MIN_RATIO_CAPACITY_AH          0.10f

/* 将浮点数限制在指定闭区间内。 */
static float clampf(float value, float min_value, float max_value)
{
/* 输入值低于下限时返回下限。 */
    if (value < min_value) return min_value;

/* 输入值高于上限时返回上限。 */
    if (value > max_value) return max_value;

/* 输入值已经有效时原值返回。 */
    return value;
}

/* 不链接libm，直接返回浮点数绝对值。 */
static float absf(float value)
{
/* 负数取反，非负数保持不变。 */
    return (value < 0.0f) ? -value : value;
}

/* 将任务周期换算为状态计数使用的整数秒。 */
static uint32_t period_to_seconds(float period_seconds)
{
/* 将正数周期四舍五入到最接近的整数秒。 */
    uint32_t seconds = (uint32_t)(period_seconds + 0.5f);

/* 调用周期不足1秒时仍至少累加1秒，防止确认计数停滞。 */
    return (seconds == 0U) ? 1U : seconds;
}

/* 按运行工艺标准中的温度-电压关系进行补偿。 */
static float compensated_cell_voltage(float base_voltage,
                                      float temperature_c,
                                      float coefficient)
{
/* 工艺表仅定义10～40℃范围内的温度补偿。 */
    float temperature = clampf(temperature_c, 10.0f, 40.0f);

/* 低于25℃时提高充电电压，高于25℃时降低充电电压。 */
    return base_voltage + coefficient * (25.0f - temperature);
}

/* 不改变当前EMS状态，仅强制生成安全的DC/DC停止目标。 */
static void stop_output(ems_context_t *context)
{
/* 禁止DC/DC运行。 */
    context->output.enable = 0U;

/* 请求停止功率传输。 */
    context->output.mode = DCDC_MODE_STOP;

/* 清零电池侧电流指令。 */
    context->output.battery_current_limit_a = 0.0f;

/* 清零电池侧电压指令。 */
    context->output.battery_voltage_limit_v = 0.0f;

/* 清零母线电压指令。 */
    context->output.bus_voltage_target_v = 0.0f;

/* 0表示策略层不下发独立功率限制。 */
    context->output.power_limit_kw = 0.0f;
}

/* 仅在状态真正变化时切换EMS状态并重新开始状态计时。 */
static void change_state(ems_context_t *context, ems_state_t state)
{
/* 状态未变化时不重复清零计时器。 */
    if (context->state != state) {
/* 保存新状态。 */
        context->state = state;

/* 新状态从0开始计时。 */
        context->state_seconds = 0U;
    }
}

/* 校验BMS测量值并汇总保护级电池故障。 */
static uint32_t battery_faults(const ems_config_t *config,
                               const ems_bms_input_t *bms)
{
/* 初始认为不存在电池侧故障。 */
    uint32_t faults = EMS_FAULT_NONE;

/* BMS离线时无法安全允许充电或放电。 */
    if (!bms->online) faults |= EMS_FAULT_BMS_OFFLINE;

/* 电池组或单体关键数据缺失时，不得参与控制判断。 */
    if ((bms->pack_voltage_v < 10.0f) ||
        (bms->max_cell_voltage_v <= 0.0f) ||
        (bms->min_cell_voltage_v <= 0.0f) ||
        (bms->soc_percent < 0.0f) ||
        (bms->soc_percent > 100.0f)) {
        faults |= EMS_FAULT_DATA_INVALID;
    }

/* 单体过压属于硬保护，与当前运行请求无关。 */
    if (bms->max_cell_voltage_v >= config->cell_charge_protect_v) {
        faults |= EMS_FAULT_CELL_OVERVOLT;
    }

/* 电池运行工艺规定温度保护阈值为45℃。 */
    if (bms->max_temperature_c >= config->temperature_protect_c) {
        faults |= EMS_FAULT_OVERTEMP;
    }

/* 返回本周期同时检测到的全部电池故障。 */
    return faults;
}

/* 校验拓扑中AC/DC、DC/DC及公共直流母线侧状态。 */
static uint32_t power_faults(const ems_config_t *config,
                             const ems_power_input_t *power,
                             ems_state_t state)
{
/* 初始认为不存在功率侧故障。 */
    uint32_t faults = EMS_FAULT_NONE;

/* 功率设备驱动将快照标记为无效时进入保护。 */
    if (!power->data_valid) faults |= EMS_FAULT_DATA_INVALID;

/* 任何电池功率传输模式都必须保证DC/DC通信正常。 */
    if (!power->dcdc_online) faults |= EMS_FAULT_DCDC_OFFLINE;

/* 在故障位映射细化前，DC/DC原始故障字非零即按锁定故障处理。 */
    if (power->dcdc_fault_bits != 0U) faults |= EMS_FAULT_DCDC_DEVICE;

/* 仅在配置了有效阈值后启用母线过压保护。 */
    if ((config->bus_high_protect_v > 0.0f) &&
        (power->bus_voltage_v >= config->bus_high_protect_v)) {
        faults |= EMS_FAULT_BUS_OVERVOLT;
    }

/* 市电供电正常或DC/DC正在备电支撑时检查母线欠压。 */
    if ((config->bus_low_protect_v > 0.0f) &&
        ((power->grid_online && power->acdc_online) ||
         ((state == EMS_STATE_BACKUP_DISCHARGE) && power->dcdc_running)) &&
        (power->bus_voltage_v <= config->bus_low_protect_v)) {
        faults |= EMS_FAULT_BUS_UNDERVOLT;
    }

/* AC/DC掉电用于触发电池备电，不能直接作为锁定故障。 */
    return faults;
}

/* 新充放电周期开始时重置相应Ah计数。 */
static void update_cycle_accounting(ems_context_t *context,
                                    ems_request_t effective_request)
{
/* 仅在有效请求发生变化时处理周期切换。 */
    if (effective_request == context->last_effective_request) return;

/* 新放电周期开始时，充入及放出容量均从0重新统计。 */
    if ((effective_request == EMS_REQUEST_PEAK_DISCHARGE) ||
        (effective_request == EMS_REQUEST_BACKUP_DISCHARGE)) {
        context->charged_ah = 0.0f;
        context->discharged_ah = 0.0f;
        context->charge_ratio = 0.0f;
    }

/* 新充电周期保留前次放电量，但充入容量从0重新统计。 */
    if ((effective_request == EMS_REQUEST_REGULAR_CHARGE) ||
        (effective_request == EMS_REQUEST_FULL_CHARGE)) {
        context->charged_ah = 0.0f;
        context->charge_ratio = 0.0f;
    }

/* 保存本次切换使用的有效请求。 */
    context->last_effective_request = effective_request;
}

/* 根据电池实测电流累计充放电Ah。 */
static void accumulate_capacity(ems_context_t *context,
                                const ems_bms_input_t *bms,
                                float period_seconds)
{
/* BMS离线时不累计可能已经失效的电流数据。 */
    if (!bms->online) return;

/* 正电流表示容量充入电池。 */
    if (bms->pack_current_a > 0.0f) {
        context->charged_ah += bms->pack_current_a * period_seconds / 3600.0f;
    }

/* 负电流表示容量从电池放出。 */
    if (bms->pack_current_a < 0.0f) {
        context->discharged_ah += -bms->pack_current_a * period_seconds / 3600.0f;
    }

/* 只有前次放电量有效时才计算充电系数。 */
    if (context->discharged_ah > EMS_MIN_RATIO_CAPACITY_AH) {
        context->charge_ratio = context->charged_ah / context->discharged_ah;
    } else {
        context->charge_ratio = 0.0f;
    }
}

/* 结合电网掉电及恢复确认时间，确定本地最终有效请求。 */
static ems_request_t arbitrate_request(ems_context_t *context,
                                       const ems_config_t *config,
                                       const ems_power_input_t *power,
                                       uint32_t step_seconds)
{
/* 只有物理市电存在且AC/DC运行正常时，才认为电网可用。 */
    uint8_t grid_available = (uint8_t)(power->grid_online && power->acdc_online);

/* 电网不可用时累计连续掉电时间，并清零恢复计时。 */
    if (!grid_available) {
        context->grid_loss_seconds += step_seconds;
        context->grid_restore_seconds = 0U;
    } else {
/* 电网可用时清零掉电计时，并累计稳定恢复时间。 */
        context->grid_loss_seconds = 0U;
        context->grid_restore_seconds += step_seconds;
    }

/* 紧急停机具有绝对优先级，备电策略不能覆盖。 */
    if (context->request == EMS_REQUEST_EMERGENCY_STOP) {
        return EMS_REQUEST_EMERGENCY_STOP;
    }

/* 电网或AC/DC掉电确认后自动切换至本地电池备电。 */
    if (context->grid_loss_seconds >= config->grid_loss_confirm_seconds) {
        return EMS_REQUEST_BACKUP_DISCHARGE;
    }

/* 电网连续稳定达到配置时间前，保持备电运行。 */
    if ((context->state == EMS_STATE_BACKUP_DISCHARGE) &&
        (context->grid_restore_seconds < config->grid_restore_confirm_seconds)) {
        return EMS_REQUEST_BACKUP_DISCHARGE;
    }

/* AUTO模式在电网稳定恢复后默认执行常规充电。 */
    if (context->request == EMS_REQUEST_AUTO) {
        return EMS_REQUEST_REGULAR_CHARGE;
    }

/* 没有安全策略覆盖时，保留本地或云端下发的明确请求。 */
    return context->request;
}

/* 锁定故障并强制生成DC/DC停止目标。 */
static void enter_fault_lock(ems_context_t *context, uint32_t faults)
{
/* 手动复位前保留所有已经检测到的故障位。 */
    context->fault_bits |= faults;

/* 进入故障锁定保护状态。 */
    change_state(context, EMS_STATE_FAULT_LOCK);

/* 撤销全部DC/DC功率传输指令。 */
    stop_output(context);
}

/* 装载当前24串500Ah电池系统的工艺默认参数。 */
void EMS_DefaultConfig(ems_config_t *config)
{
/* 目标指针无效时直接返回。 */
    if (config == NULL) return;

/* 设置C10电池容量。 */
    config->capacity_ah = 500.0f;

/* 第一阶段最大充电电流按0.35C设置。 */
    config->max_charge_current_a = 175.0f;

/* DC/DC能力未明确前，暂时沿用项目原有放电电流限制。 */
    config->max_discharge_current_a = 165.0f;

/* 无充电系数历史时，使用SOC作为充电停止后备条件。 */
    config->charge_stop_soc = 100.0f;

/* 常规70%放电深度工况在约30% SOC时停止。 */
    config->regular_discharge_stop_soc = 30.0f;

/* 备电允许使用额定全部容量，但仍受单体电压保护约束。 */
    config->backup_discharge_stop_soc = 0.0f;

/* 运行工艺规定单体充电保护电压为2.65V。 */
    config->cell_charge_protect_v = 2.65f;

/* 工艺表规定常规放电单体保护电压为1.90V。 */
    config->cell_regular_discharge_protect_v = 1.90f;

/* 工艺表规定备电放电单体保护电压为1.70V。 */
    config->cell_backup_discharge_protect_v = 1.70f;

/* 运行工艺规定温度保护值为45℃。 */
    config->temperature_protect_c = 45.0f;

/* AC/DC及负载规格未确定前设为0，暂不下发母线目标。 */
    config->bus_target_voltage_v = 0.0f;

/* 经评审的阈值未确定前设为0，关闭母线欠压保护。 */
    config->bus_low_protect_v = 0.0f;

/* 经评审的阈值未确定前设为0，关闭母线过压保护。 */
    config->bus_high_protect_v = 0.0f;

/* 校验充放电方向时忽略较小的电流零点偏差。 */
    config->direction_check_current_a = 2.0f;

/* 电网连续掉电2秒后才进入备电。 */
    config->grid_loss_confirm_seconds = 2U;

/* 电网恢复后连续稳定5秒才退出备电。 */
    config->grid_restore_confirm_seconds = 5U;

/* 反向电流连续3秒后才锁定方向故障。 */
    config->direction_fault_confirm_seconds = 3U;
}

/* 初始化一个EMS策略运行上下文。 */
void EMS_Init(ems_context_t *context)
{
/* 上下文指针无效时直接返回。 */
    if (context == NULL) return;

/* 清空计数器、请求、故障及输出字段。 */
    memset(context, 0, sizeof(*context));

/* 标记策略已初始化但尚未执行判断。 */
    context->state = EMS_STATE_INIT;

/* 默认请求正常待机，但确认掉电后仍可自动进入备电。 */
    context->request = EMS_REQUEST_STOP;

/* 按默认请求同步初始化仲裁历史。 */
    context->effective_request = EMS_REQUEST_STOP;
    context->last_effective_request = EMS_REQUEST_STOP;

/* 明确生成安全的初始停止目标。 */
    stop_output(context);
}

/* 保存外部运行请求。 */
void EMS_SetRequest(ems_context_t *context, ems_request_t request)
{
    /* 上下文指针无效时直接返回。 */
    if (context == NULL) return;

/* 仅接受ems_request_t中定义的合法枚举值。 */
    if (request > EMS_REQUEST_EMERGENCY_STOP) return;

/* 保存请求；本地安全仲裁由EMS_Step()执行。 */
    context->request = request;
}

/* 调用者确认故障原因消失后，清除锁定故障。 */
void EMS_ResetFault(ems_context_t *context)
{
    /* 上下文指针无效时直接返回。 */
    if (context == NULL) return;

/* 只有处于故障锁定状态时才执行复位。 */
    if (context->state != EMS_STATE_FAULT_LOCK) return;

/* 清除全部历史故障位。 */
    context->fault_bits = EMS_FAULT_NONE;

/* 故障复位后要求重新下发运行请求。 */
    context->request = EMS_REQUEST_STOP;
    context->effective_request = EMS_REQUEST_STOP;
    context->last_effective_request = EMS_REQUEST_STOP;

/* 清除掉电、恢复及电流方向确认历史。 */
    context->grid_loss_seconds = 0U;
    context->grid_restore_seconds = 0U;
    context->direction_error_seconds = 0U;

/* 返回DC/DC关闭的安全待机状态。 */
    change_state(context, EMS_STATE_STANDBY);
    stop_output(context);
}

/* 针对“AC/DC+直流母线+双向DC/DC”拓扑执行一次纯EMS策略计算。 */
void EMS_Step(ems_context_t *context,
              const ems_config_t *config,
              const ems_bms_input_t *bms,
              const ems_power_input_t *power,
              float period_seconds)
{
/* 暂存本周期检测到的故障，随后统一锁定。 */
    uint32_t faults;

/* 保存常规或备电工况选用的放电阈值。 */
    float discharge_cell_limit;
    float discharge_soc_limit;

/* 保存经过温度补偿的电池组目标电压。 */
    float equalize_voltage;
    float float_voltage;
    float full_voltage;

/* 保存状态及确认计数器使用的整数秒增量。 */
    uint32_t step_seconds;

/* 标记前次放电量是否足以支持充电系数控制。 */
    uint8_t ratio_available;

/* 指针无效时直接返回，并保持上一次安全或运行输出不变。 */
    if ((context == NULL) || (config == NULL) ||
        (bms == NULL) || (power == NULL)) return;

/* 拒绝非正任务周期，防止Ah积分异常。 */
    if (period_seconds <= 0.0f) return;

/* 将任务周期换算为状态计数器使用的整数秒。 */
    step_seconds = period_to_seconds(period_seconds);

/* 累加当前状态持续时间。 */
    context->state_seconds += step_seconds;

/* 在选择控制限值前先处理掉电优先级和恢复确认。 */
    context->effective_request = arbitrate_request(context, config, power, step_seconds);

/* 仅在充放电周期意图改变时重新开始容量统计。 */
    update_cycle_accounting(context, context->effective_request);

/* 周期切换计数处理完成后，再积分BMS实测电流。 */
    accumulate_capacity(context, bms, period_seconds);

/* 有效充电系数要求前次放电容量达到最小统计值。 */
    ratio_available = (uint8_t)(context->discharged_ah > EMS_MIN_RATIO_CAPACITY_AH);

/* 汇总电池及功率设备保护故障。 */
    faults = battery_faults(config, bms);
    faults |= power_faults(config, power, context->state);

/* 仅在实际备电运行时选用100%放电深度保护下限。 */
    if (context->effective_request == EMS_REQUEST_BACKUP_DISCHARGE) {
        discharge_cell_limit = config->cell_backup_discharge_protect_v;
        discharge_soc_limit = config->backup_discharge_stop_soc;
    } else {
/* 其他放电模式统一使用常规70%放电深度限值。 */
        discharge_cell_limit = config->cell_regular_discharge_protect_v;
        discharge_soc_limit = config->regular_discharge_stop_soc;
    }

/* 仅在请求放电时应用单体电压及SOC下限。 */
    if ((context->effective_request == EMS_REQUEST_PEAK_DISCHARGE) ||
        (context->effective_request == EMS_REQUEST_BACKUP_DISCHARGE)) {
/* 单体欠压属于保护级锁定故障。 */
        if (bms->min_cell_voltage_v <= discharge_cell_limit) {
            faults |= EMS_FAULT_CELL_UNDERVOLT;
        }

/* SOC下限单独保留故障位，便于诊断停止原因。 */
        if (bms->soc_percent <= discharge_soc_limit) {
            faults |= EMS_FAULT_SOC_LOW;
        }
    }

/* 生成新DC/DC指令前先锁定所有硬故障。 */
    if (faults != EMS_FAULT_NONE) {
        enter_fault_lock(context, faults);
        return;
    }

/* 已锁定故障的上下文保持停止，直到调用EMS_ResetFault()。 */
    if (context->state == EMS_STATE_FAULT_LOCK) {
        stop_output(context);
        return;
    }

/* 计算温度补偿后的24串均充目标电压。 */
    equalize_voltage = compensated_cell_voltage(EMS_EQUALIZE_CELL_VOLTAGE,
                                                 bms->max_temperature_c,
                                                 0.004f) * EMS_CELL_COUNT;

/* 计算温度补偿后的24串浮充目标电压。 */
    float_voltage = compensated_cell_voltage(EMS_FLOAT_CELL_VOLTAGE,
                                              bms->max_temperature_c,
                                              0.003f) * EMS_CELL_COUNT;

/* 计算温度补偿后的完全充电目标电压。 */
    full_voltage = compensated_cell_voltage(EMS_FULL_CELL_VOLTAGE,
                                             bms->max_temperature_c,
                                             0.004f) * EMS_CELL_COUNT;

/* 根据本地最终有效请求生成DC/DC控制目标。 */
    switch (context->effective_request) {
    case EMS_REQUEST_STOP:
/* 正常停止时由AC/DC继续向负载供电。 */
        change_state(context, EMS_STATE_STANDBY);
        stop_output(context);
        break;

    case EMS_REQUEST_EMERGENCY_STOP:
/* 即使市电掉电，紧急停机仍具有绝对优先级。 */
        change_state(context, EMS_STATE_STOPPING);
        stop_output(context);
        break;

    case EMS_REQUEST_REGULAR_CHARGE:
    case EMS_REQUEST_FULL_CHARGE:
/* 允许能量从AC/DC母线传输至电池。 */
        context->output.enable = 1U;
        context->output.mode = DCDC_MODE_CHARGE;
        context->output.bus_voltage_target_v = config->bus_target_voltage_v;
        context->output.power_limit_kw = 0.0f;

/* 无充电系数历史时达到SOC上限属于正常充电完成，不作为故障。 */
        if ((!ratio_available) && (bms->soc_percent >= config->charge_stop_soc)) {
            context->request = EMS_REQUEST_STOP;
            context->effective_request = EMS_REQUEST_STOP;
            change_state(context, EMS_STATE_STANDBY);
            stop_output(context);
            break;
        }

/* 最高单体低于2.45V时，执行第一阶段0.35C恒流充电。 */
        if ((bms->max_cell_voltage_v < EMS_EQUALIZE_CELL_VOLTAGE) &&
            ((!ratio_available) || (context->charge_ratio < 1.02f))) {
            change_state(context, EMS_STATE_GRID_CHARGE_CC);
            context->output.battery_current_limit_a = config->max_charge_current_a;
            context->output.battery_voltage_limit_v = equalize_voltage;
            break;
        }

/* 均充恒压阶段持续至充电系数达到1.03，或实测电流下降至0.03C。 */
        if ((ratio_available && (context->charge_ratio < 1.03f)) ||
            ((!ratio_available) && (absf(bms->pack_current_a) > 15.0f))) {
            change_state(context, EMS_STATE_GRID_CHARGE_CV);
            context->output.battery_current_limit_a = 50.0f;
            context->output.battery_voltage_limit_v = equalize_voltage;
            break;
        }

/* 完全充电请求使用0.015C、最长1.5小时的末级补充充电。 */
        if (context->effective_request == EMS_REQUEST_FULL_CHARGE) {
/* 充电系数达到1.07或持续时间达到1.5小时，任一满足即停止。 */
            if ((ratio_available && (context->charge_ratio >= 1.07f)) ||
                ((context->state == EMS_STATE_FULL_TRICKLE) &&
                 (context->state_seconds >= 5400U))) {
                context->request = EMS_REQUEST_STOP;
                context->effective_request = EMS_REQUEST_STOP;
                change_state(context, EMS_STATE_STANDBY);
                stop_output(context);
            } else {
/* 下发完全充电末级小电流控制目标。 */
                change_state(context, EMS_STATE_FULL_TRICKLE);
                context->output.battery_current_limit_a = 7.5f;
                context->output.battery_voltage_limit_v = full_voltage;
            }
            break;
        }

/* 常规充电在充电系数达到1.05后完成。 */
        if (ratio_available && (context->charge_ratio >= 1.05f)) {
            context->request = EMS_REQUEST_STOP;
            context->effective_request = EMS_REQUEST_STOP;
            change_state(context, EMS_STATE_STANDBY);
            stop_output(context);
        } else {
/* 未达到完成条件时维持0.03C浮充阶段。 */
            change_state(context, EMS_STATE_FLOAT_CHARGE);
            context->output.battery_current_limit_a = 15.0f;
            context->output.battery_voltage_limit_v = float_voltage;
        }
        break;

    case EMS_REQUEST_PEAK_DISCHARGE:
/* 按常规放电限值向母线供能，实现削峰。 */
        change_state(context, EMS_STATE_PEAK_DISCHARGE);
        context->output.enable = 1U;
        context->output.mode = DCDC_MODE_DISCHARGE;
        context->output.battery_current_limit_a = config->max_discharge_current_a;
        context->output.battery_voltage_limit_v =
            config->cell_regular_discharge_protect_v * EMS_CELL_COUNT;
        context->output.bus_voltage_target_v = config->bus_target_voltage_v;
        context->output.power_limit_kw = 0.0f;
        break;

    case EMS_REQUEST_BACKUP_DISCHARGE:
/* 电网或AC/DC掉电确认后，由电池维持直流母线。 */
        change_state(context, EMS_STATE_BACKUP_DISCHARGE);
        context->output.enable = 1U;
        context->output.mode = DCDC_MODE_DISCHARGE;
        context->output.battery_current_limit_a = config->max_discharge_current_a;
        context->output.battery_voltage_limit_v =
            config->cell_backup_discharge_protect_v * EMS_CELL_COUNT;
        context->output.bus_voltage_target_v = config->bus_target_voltage_v;
        context->output.power_limit_kw = 0.0f;
        break;

    case EMS_REQUEST_AUTO:
    default:
/* AUTO应已由仲裁转换为具体请求，运行至此说明请求路径异常。 */
        context->request = EMS_REQUEST_STOP;
        context->effective_request = EMS_REQUEST_STOP;
        change_state(context, EMS_STATE_STANDBY);
        stop_output(context);
        break;
    }

/* 仅在DC/DC上报运行后开始校验电流方向。 */
    if (power->dcdc_running) {
/* 充电指令下不应出现超过阈值的负向电池电流。 */
        if ((context->output.mode == DCDC_MODE_CHARGE) &&
            (power->dcdc_current_a < -config->direction_check_current_a)) {
            context->direction_error_seconds += step_seconds;
/* 放电指令下不应出现超过阈值的正向电池电流。 */
        } else if ((context->output.mode == DCDC_MODE_DISCHARGE) &&
                   (power->dcdc_current_a > config->direction_check_current_a)) {
            context->direction_error_seconds += step_seconds;
        } else {
/* 电流方向正确或接近零时，清除瞬时方向错误计时。 */
            context->direction_error_seconds = 0U;
        }
    } else {
/* DC/DC未运行时无法判断功率方向，不累计方向错误。 */
        context->direction_error_seconds = 0U;
    }

/* 电流方向错误持续达到配置确认时间后才锁定故障。 */
    if (context->direction_error_seconds >= config->direction_fault_confirm_seconds) {
        enter_fault_lock(context, EMS_FAULT_DIRECTION);
    }
}
