#ifndef EMS_STRATEGY_H
#define EMS_STRATEGY_H

#include <stdint.h>

/* EMS接收外部运行请求，但本地安全策略拥有更高优先级，可覆盖外部请求。 */
typedef enum {
    EMS_REQUEST_STOP = 0,             /* 正常待机请求；电网掉电时可被备电策略覆盖。 */
    EMS_REQUEST_AUTO,                 /* 有市电时充电，市电掉电时自动为负载备电。 */
    EMS_REQUEST_REGULAR_CHARGE,       /* 执行铅炭电池常规充电工艺。 */
    EMS_REQUEST_FULL_CHARGE,          /* 执行周期性完全充电工艺。 */
    EMS_REQUEST_PEAK_DISCHARGE,       /* 向直流母线放电，用于削峰。 */
    EMS_REQUEST_BACKUP_DISCHARGE,     /* 强制执行电池备电放电。 */
    EMS_REQUEST_EMERGENCY_STOP        /* 绝对停机；电网掉电也不能覆盖该请求。 */
} ems_request_t;

/* 状态枚举描述EMS通过双向DC/DC控制的功率流向。 */
typedef enum {
    EMS_STATE_INIT = 0,               /* 上下文已初始化，但尚未执行策略判断。 */
    EMS_STATE_STANDBY,                /* DC/DC关闭，由AC/DC向负载供电。 */
    EMS_STATE_GRID_CHARGE_CC,         /* 使用市电对电池进行恒流充电。 */
    EMS_STATE_GRID_CHARGE_CV,         /* 使用市电对电池进行恒压充电。 */
    EMS_STATE_FLOAT_CHARGE,           /* 常规浮充阶段。 */
    EMS_STATE_FULL_TRICKLE,           /* 完全充电的0.015C限时补充充电阶段。 */
    EMS_STATE_PEAK_DISCHARGE,         /* 市电存在时由电池向母线放电削峰。 */
    EMS_STATE_BACKUP_DISCHARGE,       /* 电网或AC/DC掉电后由电池维持母线。 */
    EMS_STATE_STOPPING,               /* 预留给后续DC/DC受控降功率停机。 */
    EMS_STATE_FAULT_LOCK              /* 故障锁定状态，需调用EMS_ResetFault()复位。 */
} ems_state_t;

/* EMS锁定保护原因，可同时置位多个故障位。 */
typedef enum {
    EMS_FAULT_NONE           = 0U,          /* 当前没有EMS锁定故障。 */
    EMS_FAULT_BMS_OFFLINE    = (1U << 0),   /* BCMU/BMS数据源离线。 */
    EMS_FAULT_DATA_INVALID   = (1U << 1),   /* 电池侧或功率侧测量数据无效。 */
    EMS_FAULT_CELL_OVERVOLT  = (1U << 2),   /* 至少一个单体达到充电保护上限。 */
    EMS_FAULT_CELL_UNDERVOLT = (1U << 3),   /* 至少一个单体达到当前放电保护下限。 */
    EMS_FAULT_OVERTEMP       = (1U << 4),   /* 电池温度达到保护上限。 */
    EMS_FAULT_SOC_HIGH       = (1U << 5),   /* 预留：SOC过高通常作为正常停止条件。 */
    EMS_FAULT_SOC_LOW        = (1U << 6),   /* 电池达到当前放电SOC下限。 */
    EMS_FAULT_DCDC_OFFLINE   = (1U << 7),   /* 双向DC/DC通信离线。 */
    EMS_FAULT_DCDC_DEVICE    = (1U << 8),   /* DC/DC上报设备故障。 */
    EMS_FAULT_BUS_OVERVOLT   = (1U << 9),   /* 直流母线电压超过配置上限。 */
    EMS_FAULT_BUS_UNDERVOLT  = (1U << 10),  /* 直流母线电压低于配置下限。 */
    EMS_FAULT_DIRECTION      = (1U << 11)   /* DC/DC实测电流方向与控制命令相反。 */
} ems_fault_t;

/* 进入策略前，需将Client_Sd中的BMS数据转换为以下标准单位快照。 */
typedef struct {
    uint8_t online;                    /* 为1表示BCMU/BMS快照有效且数据未超时。 */
    float pack_voltage_v;              /* 电池组总电压，单位V。 */
    float pack_current_a;              /* 电池组电流，充电为正，放电为负，单位A。 */
    float soc_percent;                 /* 电池组SOC，范围0～100，单位%。 */
    float max_cell_voltage_v;          /* 有效单体中的最高电压，单位V。 */
    float min_cell_voltage_v;          /* 有效单体中的最低电压，单位V。 */
    float max_temperature_c;           /* 有效电池温度中的最高值，单位℃。 */
} ems_bms_input_t;

/* 由功率设备驱动提供的AC/DC、DC/DC及直流母线数据快照。 */
typedef struct {
    uint8_t data_valid;                /* 为1表示功率侧测量值已通过有效性检查。 */
    uint8_t grid_online;               /* 为1表示物理市电输入存在。 */
    uint8_t acdc_online;               /* 为1表示AC/DC通信及运行状态正常。 */
    uint8_t dcdc_online;               /* 为1表示双向DC/DC通信正常。 */
    uint8_t dcdc_running;              /* 为1表示DC/DC处于功率变换运行状态。 */
    float bus_voltage_v;               /* 公共直流母线电压，单位V。 */
    float load_current_a;              /* 负载从直流母线获取的电流，单位A。 */
    float dcdc_current_a;              /* DC/DC电池侧电流，充电为正，放电为负，单位A。 */
    float dcdc_power_kw;               /* DC/DC功率，正负方向按驱动层约定，单位kW。 */
    uint32_t acdc_fault_bits;           /* AC/DC原始故障字，用于遥测及后续策略扩展。 */
    uint32_t dcdc_fault_bits;           /* DC/DC原始故障字，当前非零即进入故障锁定。 */
} ems_power_input_t;

/* 现场可配置参数；增加版本号和CRC字段后才能直接持久化该结构体。 */
typedef struct {
    float capacity_ah;                         /* C10电池容量，当前电池组为500Ah。 */
    float max_charge_current_a;                /* 允许下发的最大充电电流。 */
    float max_discharge_current_a;             /* 允许下发的最大放电电流。 */
    float charge_stop_soc;                     /* 无充电系数历史数据时使用的SOC充电停止值。 */
    float regular_discharge_stop_soc;          /* 常规及削峰放电的SOC下限。 */
    float backup_discharge_stop_soc;           /* 应急备电放电的SOC下限。 */
    float cell_charge_protect_v;               /* 单体过压锁定阈值。 */
    float cell_regular_discharge_protect_v;    /* 70%放电深度工况的单体电压下限。 */
    float cell_backup_discharge_protect_v;     /* 100%放电深度备电工况的单体电压下限。 */
    float temperature_protect_c;               /* 电池最高允许温度。 */
    float bus_target_voltage_v;                /* DC/DC母线目标电压，需依据功率设备规格填写。 */
    float bus_low_protect_v;                   /* 母线欠压保护值，设为0时关闭该检查。 */
    float bus_high_protect_v;                  /* 母线过压保护值，设为0时关闭该检查。 */
    float direction_check_current_a;           /* 电流绝对值低于该值时不检查功率方向。 */
    uint16_t grid_loss_confirm_seconds;        /* 进入备电前的电网掉电确认时间。 */
    uint16_t grid_restore_confirm_seconds;     /* 退出备电前的电网稳定恢复时间。 */
    uint16_t direction_fault_confirm_seconds;  /* 反向电流持续达到该时间后锁定故障。 */
} ems_config_t;

/* EMS请求的DC/DC工作方向。 */
typedef enum {
    DCDC_MODE_STOP = 0,                /* 停止功率传输。 */
    DCDC_MODE_CHARGE,                  /* 能量由母线传输至电池。 */
    DCDC_MODE_DISCHARGE                /* 能量由电池传输至母线。 */
} dcdc_mode_t;

/* 纯策略输出，需由独立的DC/DC驱动转换为实际通信协议帧。 */
typedef struct {
    uint8_t enable;                    /* 为1时允许DC/DC驱动启动设备。 */
    dcdc_mode_t mode;                  /* 停止、充电或放电方向。 */
    float battery_current_limit_a;     /* 电池侧电流绝对值限制，单位A。 */
    float battery_voltage_limit_v;     /* 充电电压上限或放电电压下限，单位V。 */
    float bus_voltage_target_v;        /* DC/DC支撑负载时使用的母线目标电压。 */
    float power_limit_kw;              /* 可选功率限制，0表示策略层不指定。 */
} ems_dcdc_target_t;

/* 由单个EMS策略任务持有的持续运行上下文。 */
typedef struct {
    ems_state_t state;                         /* 当前EMS状态。 */
    ems_request_t request;                     /* 本地、云端或自动控制下发的原始请求。 */
    ems_request_t effective_request;           /* 经本地电网安全仲裁后的有效请求。 */
    ems_request_t last_effective_request;      /* 上一次有效请求，用于识别充放电周期切换。 */
    uint32_t fault_bits;                       /* 已锁定的ems_fault_t故障位。 */
    float charged_ah;                          /* 当前周期已经充入的容量。 */
    float discharged_ah;                       /* 当前周期已经放出的容量。 */
    float charge_ratio;                        /* 存在放电记录时，等于charged_ah/discharged_ah。 */
    uint32_t state_seconds;                    /* 当前状态持续时间。 */
    uint32_t grid_loss_seconds;                /* 电网或AC/DC连续掉电时间。 */
    uint32_t grid_restore_seconds;             /* 电网及AC/DC连续正常时间。 */
    uint32_t direction_error_seconds;          /* 实测电流方向连续错误时间。 */
    ems_dcdc_target_t output;                  /* 最新的DC/DC控制目标。 */
} ems_context_t;

/* 装载Q/CY 3363.01-2025电池默认参数及暂时关闭的母线参数。 */
void EMS_DefaultConfig(ems_config_t *config);

/* 清空运行上下文并进入EMS_STATE_INIT状态。 */
void EMS_Init(ems_context_t *context);

/* 保存外部请求；EMS_Step()仍会执行本地安全优先级仲裁。 */
void EMS_SetRequest(ems_context_t *context, ems_request_t request);

/* 清除锁定故障并返回DC/DC停止的待机状态。 */
void EMS_ResetFault(ems_context_t *context);

/* 执行一次纯策略周期，不直接访问硬件或RTOS服务。 */
void EMS_Step(ems_context_t *context,
              const ems_config_t *config,
              const ems_bms_input_t *bms,
              const ems_power_input_t *power,
              float period_seconds);

#endif
