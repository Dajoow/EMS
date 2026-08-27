#include "FreeRTOS.h"       /* 引入 FreeRTOS 基础类型和配置。 */
#include "cmsis_os.h"       /* 引入 CMSIS-RTOS 线程和延时接口。 */
#include "task.h"           /* 引入任务通知 ulTaskNotifyTake 等接口。 */
#include "queue.h"          /* 引入 FreeRTOS 队列接口，用于把写命令安全交给唯一的 Modbus 任务。 */
#include "RS485.h"          /* 引入 USART2 的 RS485 缓冲区和复位接口。 */
#include "agile_modbus.h"   /* 引入 Agile Modbus RTU 主站组帧和解帧接口。 */
#include "modbus.h"         /* 引入 ACDC 主站配置和数据结构。 */
#include <string.h>          /* 引入 memset 和 memcpy。 */

osThreadId modbus_taskhandle = NULL; /* 保存 Modbus 任务句柄，USART2 中断用它发送任务通知。 */

static uint8_t modbus_send_buf[BUFFERSIZE]; /* Agile Modbus 生成请求帧和保存最近请求的缓冲区。 */
static uint8_t modbus_read_buf[BUFFERSIZE]; /* Agile Modbus 校验和解析响应帧的缓冲区。 */

volatile acdc_modbus_data_t g_acdc_modbus_data; /* 保存 ACDC 最新数据和通信诊断信息。 */
volatile dcdc_modbus_data_t g_dcdc_modbus_data; /* 保存 DCDC 最新数据和独立通信诊断信息。 */
volatile dcdc_modbus_write_status_t g_dcdc_modbus_write_status; /* 保存最近一条 DCDC 写命令及读回结果。 */
#if MODBUS_RX_DIAG_ENABLE
typedef struct                                      /* 保存当前尚未确认成功或失败的 USART2 接收现场。 */
{
    uint32_t uart_isr_snapshot;                     /* 进入 USART2 中断时的硬件 ISR 寄存器。 */
    uint32_t uart_error_code;                       /* HAL 中断处理后的 UART 错误码。 */
    uint16_t dma_remaining;                         /* IDLE 时 DMA 尚未接收的缓冲字节数。 */
    uint16_t isr_received_length;                   /* IDLE 中断根据 DMA 计数得到的帧长度。 */
    uint16_t dma_received_crc;                      /* DMA 原始缓冲区末两字节。 */
    uint16_t isr_receive_buffer_crc;                /* ISR 复制后接收缓冲区末两字节。 */
    uint16_t task_receive_buffer_crc;               /* 任务复制前接收缓冲区末两字节。 */
    uint16_t parser_buffer_crc;                     /* 任务复制后解析缓冲区末两字节。 */
    uint16_t copy_mismatch_index;                   /* 首个复制差异下标，0xFFFF 表示一致。 */
    uint8_t copy_source_byte;                       /* 首个差异位置的源字节。 */
    uint8_t copy_destination_byte;                  /* 首个差异位置的目标字节。 */
} modbus_rx_diag_pending_t;                        /* 单笔事务的临时诊断快照类型。 */

volatile modbus_rx_diag_t g_modbus_rx_diag; /* 保存最近一次 DCDC 读失败的长度、阶段和原始响应。 */
static volatile modbus_rx_diag_pending_t s_modbus_rx_diag_pending; /* 中断和任务之间传递当前帧诊断。 */
#endif
#if DCDC_MODBUS_PC_TEST_ENABLE
volatile dcdc_modbus_pc_test_t g_dcdc_modbus_pc_test; /* 保存 Keil Watch 触发的 PC 从站模拟写命令。 */
#endif

typedef struct                                      /* 定义写命令队列内部使用的完整命令副本。 */
{
    uint16_t address;                               /* 需要写入的保持寄存器起始地址。 */
    uint16_t count;                                 /* 需要连续写入的寄存器数量。 */
    uint16_t values[DCDC_MODBUS_MAX_WRITE_REGISTERS]; /* 在提交时复制数据，避免调用者缓冲区失效。 */
    uint32_t sequence;                              /* 命令序号，用于匹配公开的执行状态。 */
    uint32_t submitted_tick;                        /* 命令成功占用槽位时的HAL Tick，用于限制总执行时间。 */
} dcdc_modbus_write_command_t;                     /* DCDC 内部写命令类型名称。 */

static QueueHandle_t dcdc_write_queue = NULL;      /* 保存长度为一的 DCDC 写命令队列句柄。 */
static volatile uint8_t dcdc_write_busy = 0U;      /* 1 表示已有命令等待或执行，阻止控制命令堆积。 */
static uint32_t dcdc_write_sequence = 0U;          /* 为每条成功提交的写命令生成递增序号。 */
static void modbus_task(void const *args);      /* 声明唯一的 Modbus 主站轮询任务，并限制为本文件可见。 */
static int modbus_master_read_holding_registers(
    agile_modbus_t *ctx,                        /* Agile Modbus RTU 上下文。 */
    uint8_t slave,                              /* 本次请求的从站地址。 */
    uint16_t address,                           /* 本次请求的保持寄存器起始地址。 */
    uint16_t count,                             /* 本次请求的保持寄存器数量。 */
    uint16_t *registers);                       /* 保存解码后寄存器值的目标数组。 */
static int modbus_master_write_registers(
    agile_modbus_t *ctx,                        /* Agile Modbus RTU 上下文。 */
    uint8_t slave,                              /* 本次写请求的从站地址。 */
    uint16_t address,                           /* 本次写请求的保持寄存器起始地址。 */
    uint16_t count,                             /* 本次连续写入的寄存器数量。 */
    const uint16_t *values);                    /* 保存待写入原始寄存器值的数组。 */

#if MODBUS_RX_DIAG_ENABLE
static void modbus_rx_diag_begin_transaction(void); /* 清除上一笔事务的临时现场，避免超时引用旧帧。 */
#if !MODBUS_BLOCKING_RX_DIAG_ENABLE
static void modbus_rx_diag_capture_copy(            /* 比较 ISR 接收缓冲与 Agile Modbus 解析缓冲。 */
    const uint8_t *source,                          /* ISR 已完成的接收缓冲。 */
    const uint8_t *destination,                     /* Modbus 任务复制后的解析缓冲。 */
    int length);                                    /* 本次需要比较的有效字节数。 */
#endif
static uint16_t modbus_rx_diag_crc_wire_order(      /* 按 Modbus 线上先低后高顺序组合 CRC。 */
    const uint8_t *data,                            /* 参与 CRC 计算的数据起始地址。 */
    uint16_t length);                               /* 不含响应末尾 CRC 的数据长度。 */
static void modbus_rx_diag_record(
    uint8_t stage,                            /* 本次失败发生的阶段。 */
    uint16_t address,                         /* 本次请求的起始寄存器地址。 */
    uint16_t count,                           /* 本次请求的寄存器数量。 */
    int read_len,                             /* USART2 DMA 得到的响应长度，无响应时为零。 */
    int parser_result,                        /* 主站内部错误码或 Agile Modbus 解析结果。 */
    const uint8_t *raw);                      /* 需要保存的异常响应，不能读取时传 NULL。 */

#define MODBUS_RX_DIAG_RECORD(stage, address, count, read_len, parser_result, raw) \
    modbus_rx_diag_record((stage), (address), (count), (read_len), (parser_result), (raw))
#define MODBUS_RX_DIAG_BEGIN_TRANSACTION() \
    modbus_rx_diag_begin_transaction()
#define MODBUS_RX_DIAG_CAPTURE_COPY(source, destination, length) \
    modbus_rx_diag_capture_copy((source), (destination), (length))
#else
#define MODBUS_RX_DIAG_RECORD(stage, address, count, read_len, parser_result, raw) \
    do { (void)0; } while (0)
#define MODBUS_RX_DIAG_BEGIN_TRANSACTION() \
    do { (void)0; } while (0)
#define MODBUS_RX_DIAG_CAPTURE_COPY(source, destination, length) \
    do { (void)0; } while (0)
#endif

static int acdc_poll(agile_modbus_t *ctx);      /* 声明一次完整的 ACDC 数据轮询。 */
static int dcdc_poll(agile_modbus_t *ctx);      /* 声明一次完整的 DCDC 数据轮询。 */
static void acdc_update_communication_state(int rc); /* 声明 ACDC 独立通信状态更新函数。 */
static void dcdc_update_communication_state(int rc); /* 声明 DCDC 独立通信状态更新函数。 */
static uint8_t dcdc_write_address_allowed(uint16_t address, uint16_t count); /* 声明写地址白名单检查函数。 */
static uint8_t dcdc_write_value_allowed(uint16_t address, uint16_t value); /* 声明特殊控制寄存器数值检查函数。 */
static uint8_t dcdc_write_is_safe_stop(uint16_t address, const uint16_t *values, uint16_t count); /* 声明离线时仍允许提交的停机命令判断。 */
static uint8_t dcdc_write_is_start(uint16_t address, const uint16_t *values, uint16_t count); /* 声明需要执行故障门禁的启动命令判断。 */
static void dcdc_process_write_command(agile_modbus_t *ctx); /* 声明队列中单条写命令的执行和读回校验函数。 */
#if DCDC_MODBUS_PC_TEST_ENABLE
static void dcdc_process_pc_test_trigger(void); /* 声明 Keil Watch 魔术值触发检查函数。 */
#endif

void
modbus_init (void)
{
  memset((void *)&g_acdc_modbus_data, 0, sizeof(g_acdc_modbus_data)); /* 清零数据、在线状态和统计计数。 */
  memset((void *)&g_dcdc_modbus_data, 0, sizeof(g_dcdc_modbus_data)); /* 清零 DCDC 数据、在线状态和统计计数。 */
  memset((void *)&g_dcdc_modbus_write_status, 0, sizeof(g_dcdc_modbus_write_status)); /* 清零写命令状态和读回缓存。 */
#if MODBUS_RX_DIAG_ENABLE
  memset((void *)&g_modbus_rx_diag, 0, sizeof(g_modbus_rx_diag)); /* 每次复位从零开始详细收帧诊断。 */
  memset((void *)&s_modbus_rx_diag_pending, 0, sizeof(s_modbus_rx_diag_pending)); /* 清零中断临时快照。 */
#endif
#if DCDC_MODBUS_PC_TEST_ENABLE
  memset((void *)&g_dcdc_modbus_pc_test, 0, sizeof(g_dcdc_modbus_pc_test)); /* 清零 PC 模拟测试触发器和结果。 */
#endif
  dcdc_write_busy = 0U;                                                /* 初始化时确认没有待执行的 DCDC 写命令。 */
  dcdc_write_sequence = 0U;                                            /* 从零开始生成本次上电周期的命令序号。 */
  dcdc_write_queue = xQueueCreate(                                     /* 创建只容纳一条命令的 FreeRTOS 队列。 */
      DCDC_MODBUS_WRITE_QUEUE_LENGTH,                                  /* 队列长度为一，避免启停等控制命令堆积。 */
      sizeof(dcdc_modbus_write_command_t));                            /* 队列按值复制完整命令和全部寄存器数据。 */
#if MODBUS_BLOCKING_RX_DIAG_ENABLE
  (void)HAL_UART_DMAStop(&huart2);                                     /* 停止 main 中已经启动的 USART2 DMA，避免与阻塞接收同时访问 RDR。 */
  __HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE);                        /* 阻塞诊断不依赖 IDLE 中断，防止旧 ISR 路径提前停止接收。 */
  __HAL_UART_FLUSH_DRREGISTER(&huart2);                                /* 丢弃切换模式前可能残留在 RDR 中的旧字节并清除溢出状态。 */
  RS485_BUFF.recv_end_flag = 0U;                                      /* 阻塞模式不使用 ISR 完成标志，初始化时明确保持为零。 */
  RS485_BUFF.recv_len = 0U;                                           /* 阻塞模式的实际长度由 HAL 的 RxXferCount 计算。 */
#else
  ResetRS85UsartREV();                                                /* 清空 USART2 接收状态并启动 DMA 接收。 */
#endif
  osThreadDef (modbus_task, modbus_task, osPriorityNormal, 0, 512);   /* 定义普通优先级、512 字栈的主站任务。 */
  modbus_taskhandle = osThreadCreate (osThread (modbus_task), NULL);  /* 创建任务并保存句柄供中断唤醒。 */
}

const volatile acdc_modbus_data_t *acdc_modbus_get_data(void)
{
    return &g_acdc_modbus_data; /* 返回只读数据指针，避免业务层直接修改通信数据。 */
}

const volatile dcdc_modbus_data_t *dcdc_modbus_get_data(void)
{
    return &g_dcdc_modbus_data; /* 返回 DCDC 只读数据指针，避免业务层修改主站任务维护的数据。 */
}

const volatile dcdc_modbus_write_status_t *dcdc_modbus_get_write_status(void)
{
    return &g_dcdc_modbus_write_status; /* 返回最近写命令状态，供界面、策略和 Keil Watch 只读观察。 */
}

void modbus_copy_gui_snapshot(
    acdc_modbus_data_t *acdc_data,
    dcdc_modbus_data_t *dcdc_data,
    dcdc_modbus_write_status_t *write_status)
{
    /*
     * 三份状态只由 Modbus 任务写入。禁止任务切换后一次性复制，可保证
     * TouchGFX 不会在写任务更新结构体中途读到混合字段；临界区内不做计算。
     */
    taskENTER_CRITICAL();
    if (acdc_data != NULL) {
        memcpy(acdc_data, (const void *)&g_acdc_modbus_data, sizeof(*acdc_data));
    }
    if (dcdc_data != NULL) {
        memcpy(dcdc_data, (const void *)&g_dcdc_modbus_data, sizeof(*dcdc_data));
    }
    if (write_status != NULL) {
        memcpy(write_status, (const void *)&g_dcdc_modbus_write_status, sizeof(*write_status));
    }
    taskEXIT_CRITICAL();
}

uint32_t dcdc_modbus_get_data_age_ms(void)
{
    uint32_t last_update_tick = g_dcdc_modbus_data.last_update_tick; /* 原子读取最近完整轮询成功时刻。 */

    if (last_update_tick == 0U) {                                  /* 上电后尚未完成过一轮有效读取。 */
        return UINT32_MAX;                                         /* 返回最大值明确表示数据不可用于控制。 */
    }

    return HAL_GetTick() - last_update_tick;                        /* 无符号减法可正确处理HAL Tick回绕。 */
}

uint8_t dcdc_modbus_data_is_fresh(void)
{
    if (g_dcdc_modbus_data.online == 0U) {                          /* 离线状态下禁止依赖旧数据启动设备。 */
        return 0U;
    }

    return (uint8_t)(dcdc_modbus_get_data_age_ms() <=               /* 同时要求最近完整数据没有超过阈值。 */
                     DCDC_MODBUS_DATA_FRESHNESS_MS);
}

uint8_t dcdc_modbus_is_running(void)
{
    return (uint8_t)((dcdc_modbus_data_is_fresh() != 0U) &&          /* 只使用在线且未过期的一致数据。 */
                     (g_dcdc_modbus_data.fault_raw == 0U) &&        /* 0x0405 无有效故障位。 */
                     (g_dcdc_modbus_data.work_state ==              /* 0x0404 明确处于恒压限流运行状态。 */
                      DCDC_WORK_STATE_CV_CURRENT_LIMIT));
}

uint8_t dcdc_modbus_is_stopped(void)
{
    return (uint8_t)((dcdc_modbus_data_is_fresh() != 0U) &&          /* 离线或旧数据不能证明设备已经停机。 */
                     (g_dcdc_modbus_data.fault_raw == 0U) &&        /* 状态 4 且无故障才解释为停机/待机。 */
                     (g_dcdc_modbus_data.work_state ==
                      DCDC_WORK_STATE_STOP_OR_FAULT));
}

uint8_t dcdc_modbus_is_faulted(void)
{
    return (uint8_t)((dcdc_modbus_data_is_fresh() != 0U) &&          /* 只根据当前新鲜数据判断实时故障。 */
                     (g_dcdc_modbus_data.fault_raw != 0U));          /* 任一已定义故障位置位都属于故障。 */
}

static uint8_t dcdc_write_address_allowed(uint16_t address, uint16_t count)
{
    uint32_t end_address; /* 使用 32 位计算末地址，避免 16 位地址加法发生溢出。 */

    if (count == 0U) {    /* Modbus 写命令至少应包含一只保持寄存器。 */
        return 0U;        /* 数量为零时拒绝命令。 */
    }

    end_address = (uint32_t)address + (uint32_t)count - 1U; /* 计算本次连续写入覆盖的最后地址。 */

    if (count == 1U) {                                    /* 单写命令允许访问少量独立控制寄存器。 */
        if ((address == 0x0402U) ||                        /* 允许写工作模式选择寄存器。 */
            (address == 0x0403U) ||                        /* 允许写 DCDC 启停控制寄存器。 */
            (address == 0x0424U)) {                        /* 允许写特殊功能/参数保存寄存器。 */
            return 1U;                                     /* 独立控制寄存器命中白名单。 */
        }
    }

    if ((address >= 0x0427U) &&                            /* 参数写入必须从公开参数区开始。 */
        (end_address <= 0x0434U) &&                        /* 参数写入不能越过公开参数区末地址。 */
        (count <= DCDC_MODBUS_MAX_WRITE_REGISTERS)) {      /* 写入数量不能超过内部命令数组容量。 */
        return 1U;                                         /* 整个连续地址范围均位于安全白名单。 */
    }

    return 0U; /* 校准、内部调试、模块号、通信参数和其他未授权地址全部拒绝。 */
}

static uint8_t dcdc_write_value_allowed(uint16_t address, uint16_t value)
{
    if (address == 0x0402U) {                 /* 单独检查工作模式选择寄存器。 */
        return (uint8_t)((value == DCDC_WORK_MODE_STANDBY) ||          /* 新协议允许写 0：待机。 */
                         (value == DCDC_WORK_MODE_CV_CURRENT_LIMIT));  /* 新协议允许写 1：恒压限流。 */
    }

    if (address == 0x0403U) {                 /* 单独检查启停控制寄存器。 */
        return (uint8_t)((value == DCDC_RUN_COMMAND_START) || /* 允许写 4：DCDC 手动开机。 */
                         (value == DCDC_RUN_COMMAND_STOP));   /* 允许写 5：DCDC 手动关机。 */
    }

    return 1U; /* 参数区和特殊功能寄存器的工程范围将在接真实设备前另行增加限制。 */
}

static uint8_t dcdc_write_is_safe_stop(uint16_t address, const uint16_t *values, uint16_t count)
{
    if ((values == NULL) || (count != 1U)) {                        /* 安全停机只接受明确的单寄存器命令。 */
        return 0U;
    }

    if ((address == 0x0403U) && (values[0] == DCDC_RUN_COMMAND_STOP)) { /* 0x0403写5是协议定义的手动关机。 */
        return 1U;
    }

    if ((address == 0x0402U) && (values[0] == DCDC_WORK_MODE_STANDBY)) { /* 0x0402写0请求待机，不产生功率传输。 */
        return 1U;
    }

    return 0U;                                                       /* 启动、运行模式和参数命令必须通过新鲜度门禁。 */
}

static uint8_t dcdc_write_is_start(uint16_t address, const uint16_t *values, uint16_t count)
{
    return (uint8_t)((values != NULL) &&                             /* 必须提供有效的命令值指针。 */
                     (count == 1U) &&                               /* 启动控制只允许单寄存器写入。 */
                     (address == 0x0403U) &&                        /* 0x0403 是启停控制寄存器。 */
                     (values[0] == DCDC_RUN_COMMAND_START));        /* 只有写 4 的启动命令需要故障门禁。 */
}

int dcdc_modbus_write_multiple_async(uint16_t address, const uint16_t *values, uint16_t count)
{
    dcdc_modbus_write_command_t command; /* 在调用者栈上建立待入队命令，队列会完整复制它。 */
    uint16_t index;                      /* 用于复制并检查每一只待写寄存器。 */
    BaseType_t queue_result;             /* 保存 FreeRTOS 非阻塞入队结果。 */

    if ((values == NULL) ||                                      /* 调用者必须提供有效的数值数组。 */
        (count == 0U) ||                                         /* 至少写入一只寄存器。 */
        (count > DCDC_MODBUS_MAX_WRITE_REGISTERS) ||             /* 防止内部固定数组越界。 */
        (dcdc_write_address_allowed(address, count) == 0U)) {    /* 检查整个地址范围是否在白名单内。 */
        return DCDC_WRITE_SUBMIT_INVALID;                        /* 参数或地址无效，不产生总线请求。 */
    }

    for (index = 0U; index < count; index++) {                   /* 检查本次命令中的全部寄存器值。 */
        if (dcdc_write_value_allowed((uint16_t)(address + index), values[index]) == 0U) { /* 检查特殊值限制。 */
            return DCDC_WRITE_SUBMIT_INVALID;                    /* 任意值不合法就拒绝整条命令。 */
        }
    }

    if (dcdc_write_queue == NULL) {                              /* 检查 modbus_init 是否已创建队列。 */
        return DCDC_WRITE_SUBMIT_NOT_READY;                      /* 初始化前禁止提交写命令。 */
    }

    taskENTER_CRITICAL();                                        /* 原子检查新鲜度并占用唯一写命令槽位。 */
    if (dcdc_write_busy != 0U) {                                 /* 检查上一条命令是否尚未完成。 */
        taskEXIT_CRITICAL();                                     /* 离开临界区后再向调用者返回忙。 */
        return DCDC_WRITE_SUBMIT_BUSY;                           /* 拒绝堆积可能过时的控制命令。 */
    }

    if ((dcdc_modbus_data_is_fresh() == 0U) &&                   /* 启动、模式和参数命令要求完整数据新鲜。 */
        (dcdc_write_is_safe_stop(address, values, count) == 0U)) { /* 明确的关机/待机命令不受该门禁阻断。 */
        g_dcdc_modbus_write_status.stale_reject_count++;         /* 累加数据过期拒绝次数，便于Watch观察。 */
        g_dcdc_modbus_write_status.last_error = DCDC_WRITE_EXEC_STALE_DATA; /* 保存最近拒绝原因。 */
        taskEXIT_CRITICAL();                                     /* 未占用命令槽位，直接退出临界区。 */
        return DCDC_WRITE_SUBMIT_STALE_DATA;                     /* 告知业务层刷新数据后再提交。 */
    }

    if ((dcdc_write_is_start(address, values, count) != 0U) &&   /* 启动命令还必须通过当前故障门禁。 */
        (dcdc_modbus_is_faulted() != 0U)) {                      /* 0x0405 任一故障位置位时禁止开机。 */
        g_dcdc_modbus_write_status.device_fault_reject_count++; /* 累加故障拒绝次数，便于界面和Watch诊断。 */
        g_dcdc_modbus_write_status.last_error = DCDC_WRITE_EXEC_DEVICE_FAULT; /* 保存明确的拒绝原因。 */
        taskEXIT_CRITICAL();                                     /* 未占用命令槽位，直接退出临界区。 */
        return DCDC_WRITE_SUBMIT_DEVICE_FAULT;                   /* 告知业务层应先处理并清除设备故障。 */
    }

    dcdc_write_busy = 1U;                                        /* 占用写命令槽位，阻止其他任务并发提交。 */
    dcdc_write_sequence++;                                       /* 为新命令生成本次上电周期内的唯一序号。 */
    command.sequence = dcdc_write_sequence;                      /* 把新序号写入即将入队的命令副本。 */
    command.submitted_tick = HAL_GetTick();                      /* 固化提交时刻，后续重试不能无限延长。 */
    taskEXIT_CRITICAL();                                         /* 完成共享变量更新后退出临界区。 */

    command.address = address;                                   /* 保存本次命令的起始寄存器地址。 */
    command.count = count;                                       /* 保存本次命令的连续寄存器数量。 */
    memset(command.values, 0, sizeof(command.values));           /* 清零未使用的尾部元素，便于调试观察。 */
    memcpy(command.values, values, count * sizeof(uint16_t));    /* 复制调用者数据，避免异步执行时指针失效。 */

    g_dcdc_modbus_write_status.state = DCDC_WRITE_STATE_PENDING; /* 公开状态改为等待 Modbus 任务执行。 */
    g_dcdc_modbus_write_status.function_code =                   /* 根据数量记录预期使用的写功能码。 */
        (count == 1U) ? 0x06U : 0x10U;                            /* 单写使用 0x06，多写使用 0x10。 */
    g_dcdc_modbus_write_status.address = address;                /* 公开记录待执行命令起始地址。 */
    g_dcdc_modbus_write_status.count = count;                    /* 公开记录待执行命令寄存器数量。 */
    g_dcdc_modbus_write_status.attempt_count = 0U;               /* 尚未发送任何写请求。 */
    g_dcdc_modbus_write_status.readback_confirmed = 0U;          /* 尚未通过0x03读回确认。 */
    g_dcdc_modbus_write_status.last_error = 0;                   /* 新命令开始前清除上一条命令错误码。 */
    g_dcdc_modbus_write_status.sequence = command.sequence;      /* 公开新命令序号。 */
    g_dcdc_modbus_write_status.submitted_tick = command.submitted_tick; /* 公开命令提交时刻。 */
    g_dcdc_modbus_write_status.started_tick = 0U;                /* Modbus任务尚未取出该命令。 */
    g_dcdc_modbus_write_status.completed_tick = 0U;              /* 命令尚未完成，所以完成时间清零。 */
    memset((void *)g_dcdc_modbus_write_status.requested, 0,      /* 清零公开的目标值数组。 */
           sizeof(g_dcdc_modbus_write_status.requested));       /* 确保旧命令尾部值不会造成误判。 */
    memset((void *)g_dcdc_modbus_write_status.readback, 0,       /* 清零公开的读回校验数组。 */
           sizeof(g_dcdc_modbus_write_status.readback));        /* 命令完成前不保留上次读回数据。 */
    memcpy((void *)g_dcdc_modbus_write_status.requested,         /* 把本次目标值复制到公开状态。 */
           values, count * sizeof(uint16_t));                    /* 仅复制本次命令实际使用的元素。 */

    queue_result = xQueueSend(dcdc_write_queue, &command, 0U);   /* 非阻塞地把完整命令复制进长度一队列。 */

    if (queue_result != pdPASS) {                                /* 理论上仅在队列状态异常时发生。 */
        g_dcdc_modbus_write_status.state = DCDC_WRITE_STATE_FAILED; /* 标记命令未能进入执行队列。 */
        g_dcdc_modbus_write_status.last_error = DCDC_WRITE_SUBMIT_BUSY; /* 保存入队失败原因。 */
        dcdc_write_busy = 0U;                                    /* 释放写命令槽位，允许重新提交。 */
        return DCDC_WRITE_SUBMIT_BUSY;                           /* 向调用者返回队列忙。 */
    }

    return DCDC_WRITE_SUBMIT_OK;                                 /* 命令已成功排队，稍后由 Modbus 任务执行。 */
}

int dcdc_modbus_write_single_async(uint16_t address, uint16_t value)
{
    return dcdc_modbus_write_multiple_async(address, &value, 1U); /* 复用统一提交流程并指定一只寄存器。 */
}

int dcdc_modbus_set_work_mode_async(uint16_t mode)
{
    return dcdc_modbus_write_single_async(0x0402U, mode); /* 复用白名单和值域检查提交工作模式命令。 */
}

int dcdc_modbus_set_run_async(uint8_t enable)
{
    uint16_t command_value = (enable != 0U) ? DCDC_RUN_COMMAND_START : DCDC_RUN_COMMAND_STOP; /* 4开机、5关机。 */

    return dcdc_modbus_write_single_async(0x0403U, command_value); /* 通过正式安全队列异步下发。 */
}

#if DCDC_MODBUS_PC_TEST_ENABLE
static void dcdc_process_pc_test_trigger(void)
{
    uint16_t address;                                      /* 保存触发瞬间的测试起始地址。 */
    uint16_t count;                                        /* 保存触发瞬间的测试寄存器数量。 */
    uint16_t values[DCDC_MODBUS_MAX_WRITE_REGISTERS];      /* 复制 Watch 中的值，避免执行期间被再次修改。 */

    if (g_dcdc_modbus_pc_test.trigger != DCDC_MODBUS_PC_TEST_MAGIC) { /* 只有魔术值才能触发总线写入。 */
        return;                                            /* 未触发时不执行任何写操作。 */
    }

    g_dcdc_modbus_pc_test.trigger = 0U;                    /* 先清除触发值，确保每次只执行一条命令。 */
    address = g_dcdc_modbus_pc_test.address;               /* 复制 Keil Watch 中设置的起始地址。 */
    count = g_dcdc_modbus_pc_test.count;                   /* 复制 Keil Watch 中设置的寄存器数量。 */
    memcpy(values,                                         /* 把全部测试数值复制到当前函数局部数组。 */
           (const void *)g_dcdc_modbus_pc_test.values,     /* 源数据来自调试器可以修改的 volatile 对象。 */
           sizeof(values));                                /* 固定复制十四个元素，后续 API 按 count 使用。 */
    g_dcdc_modbus_pc_test.submit_result = (int16_t)        /* 保存异步提交接口的立即返回值。 */
        dcdc_modbus_write_multiple_async(address, values, count); /* 通过正式白名单和队列框架提交命令。 */
}
#endif

enum
{
    MODBUS_MASTER_OK = 0,          /* 一次主站事务成功完成。 */
    MODBUS_MASTER_TX_ERROR = -1,   /* USART2 请求帧发送失败。 */
    MODBUS_MASTER_TIMEOUT = -2,    /* 指定时间内没有收到从站响应。 */
    MODBUS_MASTER_FRAME_ERROR = -3, /* 响应帧的 CRC、地址、功能码或长度不正确。 */
    MODBUS_MASTER_VERIFY_ERROR = -4 /* 写响应成功，但使用 0x03 读回的数据与目标值不一致。 */
};

#if MODBUS_RX_DIAG_ENABLE
static uint16_t modbus_rx_diag_crc_wire_order(const uint8_t *data, uint16_t length)
{
    uint16_t crc;       /* 保存标准 Modbus CRC16 的内部计算值。 */
    uint16_t index;     /* 遍历参与校验的数据字节。 */
    uint8_t bit;        /* 对每个数据字节依次处理八个位。 */

    if (data == NULL) { /* 防止诊断代码意外读取空指针。 */
        return 0U;      /* 无有效数据时返回零，不影响业务解析。 */
    }

    crc = 0xFFFFU;      /* Modbus RTU CRC16 的初始值为全一。 */

    for (index = 0U; index < length; index++) { /* 依次送入本帧除 CRC 外的全部字节。 */
        crc ^= data[index];                     /* 当前字节与 CRC 低八位异或。 */

        for (bit = 0U; bit < 8U; bit++) {       /* 按 0xA001 多项式处理八个位。 */
            if ((crc & 0x0001U) != 0U) {        /* 最低位为一时右移后异或多项式。 */
                crc = (uint16_t)((crc >> 1U) ^ 0xA001U); /* 完成当前位的反馈计算。 */
            } else {
                crc = (uint16_t)(crc >> 1U);    /* 最低位为零时只进行右移。 */
            }
        }
    }

    return (uint16_t)(((crc & 0x00FFU) << 8U) | /* 把线上先发送的 CRC 低字节放到高八位。 */
                      ((crc >> 8U) & 0x00FFU)); /* 把线上后发送的 CRC 高字节放到低八位。 */
}

static void modbus_rx_diag_begin_transaction(void)
{
    memset((void *)&s_modbus_rx_diag_pending, 0, sizeof(s_modbus_rx_diag_pending)); /* 清除上一帧现场。 */
    s_modbus_rx_diag_pending.copy_mismatch_index = 0xFFFFU; /* 默认 ISR 缓冲和解析缓冲完全一致。 */
}

void modbus_rx_diag_capture_isr(
    uint32_t uart_isr_snapshot,
    uint32_t uart_error_code,
    uint16_t dma_remaining,
    uint16_t received_length,
    const uint8_t *dma_buffer,
    const uint8_t *receive_buffer)
{
    s_modbus_rx_diag_pending.uart_isr_snapshot = uart_isr_snapshot; /* 保存 HAL 清标志前的硬件现场。 */
    s_modbus_rx_diag_pending.uart_error_code = uart_error_code;     /* 保存 HAL 处理后的软件错误码。 */
    s_modbus_rx_diag_pending.dma_remaining = dma_remaining;         /* 保存 DMA 剩余计数。 */
    s_modbus_rx_diag_pending.isr_received_length = received_length; /* 保存 IDLE 中断计算长度。 */

    s_modbus_rx_diag_pending.dma_received_crc = 0U;          /* 长度不足时保持 CRC 快照为零。 */
    s_modbus_rx_diag_pending.isr_receive_buffer_crc = 0U;   /* 长度不足时保持复制后 CRC 为零。 */

    if ((received_length >= 2U) && (received_length <= BUFFERSIZE)) { /* 末两字节位于有效缓冲范围内。 */
        if (dma_buffer != NULL) {                            /* 检查 DMA 原始缓冲指针。 */
            s_modbus_rx_diag_pending.dma_received_crc =     /* 按线上先后顺序组合 DMA 帧尾。 */
                (uint16_t)(((uint16_t)dma_buffer[received_length - 2U] << 8U) |
                           (uint16_t)dma_buffer[received_length - 1U]);
        }

        if (receive_buffer != NULL) {                        /* 检查 ISR 复制目标缓冲指针。 */
            s_modbus_rx_diag_pending.isr_receive_buffer_crc = /* 按线上顺序组合复制后帧尾。 */
                (uint16_t)(((uint16_t)receive_buffer[received_length - 2U] << 8U) |
                           (uint16_t)receive_buffer[received_length - 1U]);
        }
    }

    if ((uart_isr_snapshot & UART_FLAG_PE) != 0U) { /* 中断入口硬件快照含奇偶校验错误。 */
        g_modbus_rx_diag.uart_parity_error_count++;  /* 累计 USART2 奇偶校验错误。 */
    }
    if ((uart_isr_snapshot & UART_FLAG_NE) != 0U) { /* 中断入口硬件快照含噪声错误。 */
        g_modbus_rx_diag.uart_noise_error_count++;   /* 累计 USART2 噪声错误。 */
    }
    if ((uart_isr_snapshot & UART_FLAG_FE) != 0U) { /* 中断入口硬件快照含帧格式错误。 */
        g_modbus_rx_diag.uart_frame_error_count++;   /* 累计 USART2 帧格式错误。 */
    }
    if ((uart_isr_snapshot & UART_FLAG_ORE) != 0U) { /* 中断入口硬件快照含接收溢出。 */
        g_modbus_rx_diag.uart_overrun_error_count++; /* 累计 USART2 接收溢出。 */
    }
}

#if !MODBUS_BLOCKING_RX_DIAG_ENABLE
static void modbus_rx_diag_capture_copy(
    const uint8_t *source,
    const uint8_t *destination,
    int length)
{
    uint16_t compare_length; /* 保存经过边界检查后的比较长度。 */
    uint16_t index;          /* 查找两个缓冲区首个不同字节。 */

    if ((source == NULL) || (destination == NULL) || /* 两侧缓冲必须有效。 */
        (length <= 0) || (length > BUFFERSIZE)) {    /* 比较范围必须位于固定缓冲区内。 */
        return;                                      /* 参数无效时只放弃诊断，不改变通信结果。 */
    }

    compare_length = (uint16_t)length;               /* 边界确认后转换为固定宽度长度。 */
    s_modbus_rx_diag_pending.task_receive_buffer_crc = /* 保存任务复制前接收缓冲帧尾。 */
        (compare_length >= 2U) ?
        (uint16_t)(((uint16_t)source[compare_length - 2U] << 8U) |
                   (uint16_t)source[compare_length - 1U]) : 0U;
    s_modbus_rx_diag_pending.parser_buffer_crc =     /* 保存任务复制后解析缓冲帧尾。 */
        (compare_length >= 2U) ?
        (uint16_t)(((uint16_t)destination[compare_length - 2U] << 8U) |
                   (uint16_t)destination[compare_length - 1U]) : 0U;

    for (index = 0U; index < compare_length; index++) { /* 逐字节确认任务复制是否改变数据。 */
        if (source[index] != destination[index]) {       /* 找到首个不一致字节。 */
            s_modbus_rx_diag_pending.copy_mismatch_index = index; /* 保存差异位置。 */
            s_modbus_rx_diag_pending.copy_source_byte = source[index]; /* 保存源字节。 */
            s_modbus_rx_diag_pending.copy_destination_byte = destination[index]; /* 保存目标字节。 */
            break;                                      /* 只记录首个差异，缩短诊断执行时间。 */
        }
    }
}
#endif

static void modbus_rx_diag_record(
    uint8_t stage,
    uint16_t address,
    uint16_t count,
    int read_len,
    int parser_result,
    const uint8_t *raw)
{
    uint16_t index;       /* 遍历原始异常响应时使用的数组下标。 */
    uint16_t copy_length; /* 本次实际复制到 raw[] 的字节数。 */

    g_modbus_rx_diag.request_address = address; /* 保存最近失败的起始寄存器地址。 */
    g_modbus_rx_diag.request_count = count;     /* 保存最近失败的寄存器数量。 */
    g_modbus_rx_diag.expected_length =           /* 保存正常功能码 0x03 响应应有的完整长度。 */
        (uint16_t)(5U + (2U * count));           /* 地址、功能码、字节数、数据和 CRC 共 5+2N 字节。 */

    if (read_len > 0) {                         /* 只有实际收到数据时才保存响应长度。 */
        g_modbus_rx_diag.received_length = (uint16_t)read_len; /* 保存 USART2 DMA 统计长度。 */
    } else {
        g_modbus_rx_diag.received_length = 0U;  /* 超时和发送失败没有有效响应长度。 */
    }

    g_modbus_rx_diag.parser_result =            /* 保存最近一次底层错误码或解析结果。 */
        (int16_t)parser_result;                  /* Keil Watch 可用有符号十进制或十六进制查看。 */
    g_modbus_rx_diag.failure_stage = stage;      /* 保存本次失败发生的具体阶段。 */
    g_modbus_rx_diag.uart_isr_snapshot = s_modbus_rx_diag_pending.uart_isr_snapshot; /* 固化失败帧硬件现场。 */
    g_modbus_rx_diag.uart_error_code = s_modbus_rx_diag_pending.uart_error_code; /* 固化失败帧 HAL 错误码。 */
    g_modbus_rx_diag.dma_remaining = s_modbus_rx_diag_pending.dma_remaining; /* 固化失败帧 DMA 余量。 */
    g_modbus_rx_diag.isr_received_length = s_modbus_rx_diag_pending.isr_received_length; /* 固化 ISR 长度。 */
    g_modbus_rx_diag.dma_received_crc = s_modbus_rx_diag_pending.dma_received_crc; /* 固化 DMA 帧尾。 */
    g_modbus_rx_diag.isr_receive_buffer_crc = s_modbus_rx_diag_pending.isr_receive_buffer_crc; /* 固化 ISR 复制帧尾。 */
    g_modbus_rx_diag.task_receive_buffer_crc = s_modbus_rx_diag_pending.task_receive_buffer_crc; /* 固化任务源帧尾。 */
    g_modbus_rx_diag.parser_buffer_crc = s_modbus_rx_diag_pending.parser_buffer_crc; /* 固化解析缓冲帧尾。 */
    g_modbus_rx_diag.copy_mismatch_index = s_modbus_rx_diag_pending.copy_mismatch_index; /* 固化首个复制差异。 */
    g_modbus_rx_diag.copy_source_byte = s_modbus_rx_diag_pending.copy_source_byte; /* 固化复制源字节。 */
    g_modbus_rx_diag.copy_destination_byte = s_modbus_rx_diag_pending.copy_destination_byte; /* 固化复制目标字节。 */
    g_modbus_rx_diag.calculated_crc = 0U;       /* 没有有效响应时 CRC 诊断保持为零。 */
    g_modbus_rx_diag.received_crc = 0U;         /* 没有有效响应时接收 CRC 保持为零。 */

    if ((raw != NULL) && (read_len >= 2) &&     /* 至少包含两个 CRC 字节时才进行比较。 */
        (read_len <= BUFFERSIZE)) {             /* 防止长度异常诊断再次越过固定接收缓冲。 */
        g_modbus_rx_diag.calculated_crc =       /* 使用与 Modbus RTU 等价的 0xA001 算法计算 CRC。 */
            modbus_rx_diag_crc_wire_order(raw, (uint16_t)(read_len - 2));
        g_modbus_rx_diag.received_crc =         /* 按线上顺序组合帧末实际收到的 CRC。 */
            (uint16_t)(((uint16_t)raw[read_len - 2] << 8U) |
                       (uint16_t)raw[read_len - 1]);

        if (stage == MODBUS_RX_DIAG_STAGE_DESERIALIZE) { /* 只细分实际进入解帧器的失败。 */
            if (g_modbus_rx_diag.calculated_crc != g_modbus_rx_diag.received_crc) { /* CRC 不一致。 */
                g_modbus_rx_diag.crc_mismatch_count++;   /* 累计真实 CRC 失败。 */
            } else {
                g_modbus_rx_diag.confirmation_failure_count++; /* CRC 正确，失败来自地址/功能码/数量确认。 */
            }
        }
    }

    if (s_modbus_rx_diag_pending.copy_mismatch_index != 0xFFFFU) { /* 任务复制改变了至少一字节。 */
        g_modbus_rx_diag.buffer_copy_mismatch_count++; /* 累计接收缓冲到解析缓冲的复制异常。 */
    }

    for (index = 0U; index < MODBUS_RX_DIAG_RAW_MAX; index++) { /* 遍历全部原始帧快照空间。 */
        g_modbus_rx_diag.raw[index] = 0U;        /* 清除上一次异常帧，防止旧尾部数据混入。 */
    }

    copy_length = 0U;                           /* 默认本次没有可保存的响应字节。 */

    if ((raw != NULL) && (read_len > 0)) {      /* 只有源指针和长度均有效时才复制。 */
        copy_length = (uint16_t)read_len;        /* 从实际响应长度开始计算复制数量。 */

        if (copy_length > MODBUS_RX_DIAG_RAW_MAX) { /* 检查是否超过诊断数组容量。 */
            copy_length = MODBUS_RX_DIAG_RAW_MAX;   /* 最多保存前 32 字节，防止数组越界。 */
        }

        for (index = 0U; index < copy_length; index++) { /* 逐字节保存本次异常响应。 */
            g_modbus_rx_diag.raw[index] = raw[index];    /* 避免对 volatile 目标使用不清晰的强制转换。 */
        }
    }

    g_modbus_rx_diag.raw_length =               /* 保存 raw[] 中本次真正有效的字节数。 */
        (uint8_t)copy_length;                    /* 上限为 32，可以安全转换为八位数。 */

    if (address == 0x0404U) {                   /* 判断是否为状态、故障和温度数据块。 */
        g_modbus_rx_diag.status_block_failure_count++; /* 累加第一组读取失败次数。 */
    } else if (address == 0x041BU) {            /* 判断是否为实时功率、电压和电流数据块。 */
        g_modbus_rx_diag.realtime_block_failure_count++; /* 累加第二组读取失败次数。 */
    } else {
        g_modbus_rx_diag.other_address_failure_count++; /* 当前 DCDC 只读测试正常应保持为零。 */
    }

    if (stage == MODBUS_RX_DIAG_STAGE_TIMEOUT) { /* 判断是否完全没有收到从站响应。 */
        g_modbus_rx_diag.timeout_failure_count++; /* 单独累计超时异常。 */
    } else {
        g_modbus_rx_diag.protocol_failure_count++; /* 其余异常计入非超时失败。 */
    }

    switch (stage) {                            /* 按失败阶段累计更细的根因计数。 */
        case MODBUS_RX_DIAG_STAGE_PARAMETER:
            g_modbus_rx_diag.parameter_failure_count++; /* 累加内部参数异常。 */
            break;

        case MODBUS_RX_DIAG_STAGE_SERIALIZE:
            g_modbus_rx_diag.serialize_failure_count++; /* 累加请求组帧异常。 */
            break;

        case MODBUS_RX_DIAG_STAGE_TRANSMIT:
            g_modbus_rx_diag.transmit_failure_count++; /* 累加 USART2 请求发送异常。 */
            break;

        case MODBUS_RX_DIAG_STAGE_RECEIVE_FLAG:
            g_modbus_rx_diag.receive_flag_failure_count++; /* 累加通知与标志不一致异常。 */
            break;

        case MODBUS_RX_DIAG_STAGE_LENGTH:
            g_modbus_rx_diag.receive_length_failure_count++; /* 累加 DMA 长度异常。 */
            break;

        case MODBUS_RX_DIAG_STAGE_DESERIALIZE:
            g_modbus_rx_diag.deserialize_failure_count++; /* 累加协议解析异常。 */
            break;

        default:
            break;                               /* 超时已单独统计，未知阶段不增加细分类。 */
    }

    g_modbus_rx_diag.sequence++;                /* 最后递增，表示这一整组字段已经更新完成。 */
}
#endif

static int modbus_master_read_holding_registers(
    agile_modbus_t *ctx,
    uint8_t slave,
    uint16_t address,
    uint16_t count,
    uint16_t *registers)
{
    int send_len; /* 保存 Agile Modbus 生成的 RTU 请求帧长度。 */
    int read_len; /* 保存 USART2 实际收到的响应帧长度。 */
    int rc;       /* 保存 Agile Modbus 解帧结果。 */
#if MODBUS_BLOCKING_RX_DIAG_ENABLE
    uint16_t expected_len;          /* 保存功能码 0x03 正常响应应有的固定长度。 */
    HAL_StatusTypeDef rx_hal_status; /* 保存阻塞接收返回的 HAL 状态。 */
#endif

    MODBUS_RX_DIAG_BEGIN_TRANSACTION(); /* 每笔读事务先清除上一帧临时现场，防止超时沿用旧诊断。 */

    if ((ctx == NULL) || (registers == NULL) || (count == 0U)) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_PARAMETER,
            address,
            count,
            0,
            MODBUS_MASTER_FRAME_ERROR,
            NULL); /* 记录内部参数错误，不访问 USART2。 */
        return MODBUS_MASTER_FRAME_ERROR; /* 参数无效时不访问串口。 */
    }

#if MODBUS_BLOCKING_RX_DIAG_ENABLE
    /*
     * 本诊断分支完全绕过 USART2 DMA、IDLE 中断、任务通知和 ISR memcpy。
     * 每次发送请求前刷新 RDR，随后由当前 Modbus 任务直接阻塞读取固定长度。
     */
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_IDLE); /* 防止其他初始化路径重新打开 IDLE 中断。 */
    __HAL_UART_FLUSH_DRREGISTER(&huart2);         /* 丢弃上一笔事务可能迟到并残留的字节。 */
    RS485_BUFF.recv_end_flag = 0U;               /* 明确阻塞诊断不依赖 ISR 接收完成标志。 */
    RS485_BUFF.recv_len = 0U;                    /* 清除仅供原 DMA 路径使用的旧长度。 */
#else
    /*
     * 清除上一次可能残留的任务通知。
     */
    (void)ulTaskNotifyTake(pdTRUE, 0U); /* 非阻塞清除上一次事务可能残留的通知计数。 */

    /*
     * 停止可能仍在运行的DMA，再清空接收状态并重新启动DMA。
     */
    (void)HAL_UART_DMAStop(&huart2); /* 停止旧 DMA，避免上一帧残留数据进入新事务。 */
    ResetRS85UsartREV();             /* 清零软件缓冲并重新启动 USART2 DMA 接收。 */
#endif

    agile_modbus_set_slave(ctx, slave); /* 设置本次请求和响应校验使用的从站地址。 */

    send_len = agile_modbus_serialize_read_registers(
        ctx,
        address,
        count); /* 使用功能码 0x03 生成“读保持寄存器”RTU 请求帧。 */

    if (send_len <= 0) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_SERIALIZE,
            address,
            count,
            0,
            send_len,
            NULL); /* 保存 Agile Modbus 组帧失败结果。 */
        return MODBUS_MASTER_FRAME_ERROR; /* 组帧失败时结束本次事务。 */
    }

    if (HAL_UART_Transmit(
            &huart2,
            ctx->send_buf,
            (uint16_t)send_len,
            100U) != HAL_OK) { /* 通过隔离二线 RS485 发送请求，发送超时为 100 ms。 */
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_TRANSMIT,
            address,
            count,
            0,
            MODBUS_MASTER_TX_ERROR,
            NULL); /* 保存 USART2 请求发送失败。 */
        return MODBUS_MASTER_TX_ERROR; /* HAL 发送失败时记录发送错误。 */
    }

#if MODBUS_BLOCKING_RX_DIAG_ENABLE
    /*
     * 0x03正常响应长度固定为：地址1 + 功能码1 + 字节数1 + 数据2*N + CRC2。
     * HAL_UART_Receive不使用DMA和IDLE中断，因此可独立验证原接收框架是否引入末字节异常。
     */
    expected_len = (uint16_t)(5U + (2U * count));

    if ((expected_len > (uint16_t)ctx->read_bufsz) ||
        (expected_len > BUFFERSIZE)) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_PARAMETER,
            address,
            count,
            0,
            MODBUS_MASTER_FRAME_ERROR,
            NULL); /* 固定响应长度超过解析缓冲时拒绝访问串口。 */
        return MODBUS_MASTER_FRAME_ERROR;
    }

    memset(ctx->read_buf, 0, expected_len); /* 清除上一次响应，确保部分接收不会混入旧字节。 */
    rx_hal_status = HAL_UART_Receive(
        &huart2,
        ctx->read_buf,
        expected_len,
        MODBUS_RESPONSE_TIMEOUT_MS); /* 直接等待正常响应长度；不经过DMA、IDLE中断或任务通知。 */

    read_len = (int)(expected_len - huart2.RxXferCount); /* 超时时也保留已经实际收到的部分帧长度。 */

    if ((rx_hal_status != HAL_OK) && (read_len == 0)) {
        if (rx_hal_status == HAL_TIMEOUT) {
            MODBUS_RX_DIAG_RECORD(
                MODBUS_RX_DIAG_STAGE_TIMEOUT,
                address,
                count,
                0,
                MODBUS_MASTER_TIMEOUT,
                NULL); /* 500 ms 内一个字节都没有收到，仍按真实无响应统计。 */
            return MODBUS_MASTER_TIMEOUT;
        }

        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_RECEIVE_FLAG,
            address,
            count,
            0,
            MODBUS_MASTER_FRAME_ERROR,
            NULL); /* HAL非超时错误且没有数据时记为接收框架错误。 */
        return MODBUS_MASTER_FRAME_ERROR;
    }

    if ((read_len <= 0) ||
        (read_len > (int)expected_len) ||
        (read_len > (int)ctx->read_bufsz)) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_LENGTH,
            address,
            count,
            read_len,
            MODBUS_MASTER_FRAME_ERROR,
            ctx->read_buf); /* 保存阻塞接收得到的非法长度和原始字节。 */
        return MODBUS_MASTER_FRAME_ERROR;
    }
#else
    /*
     * USART2 IDLE中断在收到完整响应后通知本任务。
     */
    if (ulTaskNotifyTake(
            pdTRUE,
            pdMS_TO_TICKS(MODBUS_RESPONSE_TIMEOUT_MS)) == 0U) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_TIMEOUT,
            address,
            count,
            0,
            MODBUS_MASTER_TIMEOUT,
            NULL); /* 保存完全没有收到响应的超时异常。 */
        (void)HAL_UART_DMAStop(&huart2); /* 超时后停止仍在等待数据的 DMA。 */
        ResetRS85UsartREV();             /* 重新启动 DMA，为下一轮请求恢复接收能力。 */
        return MODBUS_MASTER_TIMEOUT;    /* 将超时结果交给在线状态统计逻辑。 */
    }

    if (RS485_BUFF.recv_end_flag == 0U) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_RECEIVE_FLAG,
            address,
            count,
            0,
            MODBUS_MASTER_FRAME_ERROR,
            NULL); /* 保存任务通知与接收完成标志不一致异常。 */
        (void)HAL_UART_DMAStop(&huart2); /* 通知与接收完成标志不一致时停止 DMA。 */
        ResetRS85UsartREV();             /* 清除异常状态并重新启动 DMA。 */
        return MODBUS_MASTER_FRAME_ERROR; /* 返回响应帧错误。 */
    }

    read_len = RS485_BUFF.recv_len; /* 读取 IDLE 中断计算出的有效响应字节数。 */

    if ((read_len <= 0) ||
        (read_len > (int)ctx->read_bufsz) ||
        (read_len > BUFFERSIZE)) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_LENGTH,
            address,
            count,
            read_len,
            MODBUS_MASTER_FRAME_ERROR,
            RS485_BUFF.recv_buf); /* 在清空接收缓冲前保存非法长度和前 32 字节。 */
        ResetRS85UsartREV();              /* 长度非法时丢弃当前帧并重启 DMA。 */
        return MODBUS_MASTER_FRAME_ERROR; /* 防止缓冲区越界并返回帧错误。 */
    }

    memcpy(
        ctx->read_buf,
        RS485_BUFF.recv_buf,
        (size_t)read_len); /* 将中断接收缓冲复制到 Agile Modbus 上下文。 */

    MODBUS_RX_DIAG_CAPTURE_COPY(
        RS485_BUFF.recv_buf,
        ctx->read_buf,
        read_len); /* 在清空接收缓冲前确认任务复制是否改变任何字节和帧尾 CRC。 */

    /*
     * 复制完成后立即重新开启DMA，为下一次事务做准备。
     */
    ResetRS85UsartREV(); /* 当前响应已复制完成，重新启动 DMA 接收下一帧。 */
#endif

    rc = agile_modbus_deserialize_read_registers(
        ctx,
        read_len,
        registers); /* 校验从站地址、功能码、数据长度和 CRC，并提取寄存器值。 */

    if (rc != count) {
        MODBUS_RX_DIAG_RECORD(
            MODBUS_RX_DIAG_STAGE_DESERIALIZE,
            address,
            count,
            read_len,
            rc,
            ctx->read_buf); /* 保存解析失败响应的完整长度和前 32 字节。 */
        return MODBUS_MASTER_FRAME_ERROR; /* 实际返回数量与请求数量不同则判定失败。 */
    }

    return MODBUS_MASTER_OK; /* 返回主站事务成功。 */
}

static int modbus_master_write_registers(
    agile_modbus_t *ctx,
    uint8_t slave,
    uint16_t address,
    uint16_t count,
    const uint16_t *values)
{
    int send_len; /* 保存 Agile Modbus 生成的功能码 0x06 或 0x10 请求帧长度。 */
    int read_len; /* 保存 USART2 实际收到的写响应帧长度。 */
    int rc;       /* 保存 Agile Modbus 对写响应的校验结果。 */

    if ((ctx == NULL) ||                                  /* 必须提供有效的 Agile Modbus 上下文。 */
        (values == NULL) ||                               /* 必须提供至少一个待写寄存器值。 */
        (count == 0U) ||                                  /* Modbus 不允许写入零只寄存器。 */
        (count > DCDC_MODBUS_MAX_WRITE_REGISTERS)) {      /* 防止超过当前 DCDC 命令框架容量。 */
        return MODBUS_MASTER_FRAME_ERROR;                 /* 参数非法时不访问 RS485 总线。 */
    }

    (void)ulTaskNotifyTake(pdTRUE, 0U);                   /* 清除上一笔事务可能残留的任务通知。 */
    (void)HAL_UART_DMAStop(&huart2);                      /* 停止上一笔事务可能仍在运行的接收 DMA。 */
    ResetRS85UsartREV();                                  /* 清空接收状态并重新启动 USART2 DMA。 */
    agile_modbus_set_slave(ctx, slave);                   /* 设置本次写请求和响应校验的从站地址。 */

    if (count == 1U) {                                    /* 单寄存器写入应使用 DCDC 支持的功能码 0x06。 */
        send_len = agile_modbus_serialize_write_register( /* 生成单寄存器写入 RTU 请求帧。 */
            ctx,                                          /* 使用当前任务独占的 Modbus RTU 上下文。 */
            address,                                      /* 写入指定的保持寄存器地址。 */
            values[0]);                                   /* 写入数组中的第一只原始寄存器值。 */
    } else {                                              /* 两只及以上连续寄存器使用功能码 0x10。 */
        send_len = agile_modbus_serialize_write_registers( /* 生成多寄存器写入 RTU 请求帧。 */
            ctx,                                          /* 使用相同的 RTU 上下文和静态发送缓冲。 */
            address,                                      /* 指定连续写参数区的起始地址。 */
            count,                                        /* 指定本次连续写入的寄存器数量。 */
            values);                                      /* Agile Modbus 按高字节在前组装每个值。 */
    }

    if (send_len <= 0) {                                  /* 检查 Agile Modbus 是否成功生成请求帧。 */
        return MODBUS_MASTER_FRAME_ERROR;                 /* 组帧失败时结束本次写事务。 */
    }

    if (HAL_UART_Transmit(                                /* 使用阻塞方式发送完整的 Modbus RTU 请求。 */
            &huart2,                                      /* DCDC 与 ACDC 共用 USART2 二线 RS485。 */
            ctx->send_buf,                                /* 发送 Agile Modbus 已生成并附加 CRC 的帧。 */
            (uint16_t)send_len,                           /* 发送本次请求帧的全部有效字节。 */
            100U) != HAL_OK) {                            /* 单次 UART 发送最长等待 100 ms。 */
        return MODBUS_MASTER_TX_ERROR;                    /* UART 发送失败时返回发送错误。 */
    }

    if (ulTaskNotifyTake(                                 /* 阻塞等待 USART2 IDLE 中断通知完整响应到达。 */
            pdTRUE,                                       /* 取走通知后自动清零通知计数。 */
            pdMS_TO_TICKS(MODBUS_RESPONSE_TIMEOUT_MS)) == 0U) { /* 超过统一响应时间则判定超时。 */
        (void)HAL_UART_DMAStop(&huart2);                  /* 超时后停止仍在等待数据的 DMA。 */
        ResetRS85UsartREV();                              /* 清空异常接收状态并恢复下一帧 DMA。 */
        return MODBUS_MASTER_TIMEOUT;                     /* 把超时结果交给写命令状态记录。 */
    }

    if (RS485_BUFF.recv_end_flag == 0U) {                 /* 检查中断是否真的标记了完整接收帧。 */
        (void)HAL_UART_DMAStop(&huart2);                  /* 通知与完成标志不一致时停止 DMA。 */
        ResetRS85UsartREV();                              /* 清除异常并恢复下一笔事务接收。 */
        return MODBUS_MASTER_FRAME_ERROR;                 /* 将异常状态判定为响应帧错误。 */
    }

    read_len = RS485_BUFF.recv_len;                       /* 读取 USART2 IDLE 中断统计的响应长度。 */

    if ((read_len <= 0) ||                                /* 响应长度必须大于零。 */
        (read_len > (int)ctx->read_bufsz) ||              /* 响应不能超过 Agile Modbus 接收缓冲。 */
        (read_len > BUFFERSIZE)) {                        /* 响应也不能超过 RS485 软件缓冲上限。 */
        ResetRS85UsartREV();                              /* 长度非法时丢弃帧并恢复 DMA。 */
        return MODBUS_MASTER_FRAME_ERROR;                 /* 防止缓冲区越界并报告帧错误。 */
    }

    memcpy(                                               /* 把中断缓冲复制到 Agile Modbus 解析缓冲。 */
        ctx->read_buf,                                    /* 目标是当前上下文绑定的接收缓冲。 */
        RS485_BUFF.recv_buf,                              /* 源数据是 USART2 IDLE 中断完成的帧。 */
        (size_t)read_len);                                /* 仅复制本次响应实际收到的字节。 */

    ResetRS85UsartREV();                                  /* 复制完成后立即恢复下一笔 DMA 接收。 */

    if (count == 1U) {                                    /* 根据请求功能码选择对应的响应解析函数。 */
        rc = agile_modbus_deserialize_write_register(     /* 校验 0x06 响应是否原样回显地址和值。 */
            ctx,                                          /* 使用保存了原始请求和响应的上下文。 */
            read_len);                                    /* 传入完整 RTU 响应帧长度。 */
    } else {                                              /* 多寄存器写入解析 0x10 响应。 */
        rc = agile_modbus_deserialize_write_registers(    /* 校验响应地址和已写寄存器数量。 */
            ctx,                                          /* 使用同一个请求/响应上下文。 */
            read_len);                                    /* 传入本次 0x10 响应帧长度。 */
    }

    if (rc != count) {                                    /* Agile Modbus 成功时返回确认的寄存器数量。 */
        return MODBUS_MASTER_FRAME_ERROR;                 /* 数量、CRC、地址或功能码不符均判定失败。 */
    }

    return MODBUS_MASTER_OK;                              /* 写响应已经完整通过协议校验。 */
}

static uint32_t modbus_u32(const uint16_t *reg)
{
    return ((uint32_t)reg[0] << 16U) | reg[1]; /* 按高寄存器在前的顺序组合 UINT32。 */
}

static int acdc_poll(agile_modbus_t *ctx)
{
    uint16_t reg[16]; /* 保存本轮单个数据块的原始寄存器值。 */
    int rc;           /* 保存每一次 Modbus 读事务的执行结果。 */

    /*
     * 快速状态块：0x1000～0x100C，共13个寄存器。
     */
    rc = modbus_master_read_holding_registers(
        ctx,
        ACDC_MODBUS_SLAVE_ADDR,
        0x1000U,
        13U,
        reg); /* 读取系统母排、交流电压电流和交流频率。 */

    if (rc != MODBUS_MASTER_OK) {
        return rc; /* 任一数据块失败就终止本轮，防止把半轮数据标记为完整更新。 */
    }

    g_acdc_modbus_data.dc_bus_voltage_v = reg[0] / 10.0f;  /* 0x1000，精度 1，换算为 V。 */
    g_acdc_modbus_data.dc_load_current_a = reg[1] / 10.0f; /* 0x1001，精度 1，换算为 A。 */

    g_acdc_modbus_data.ac_voltage_v = reg[4] / 10.0f; /* 0x1004，系统汇总交流电压。 */
    g_acdc_modbus_data.ac_current_a = reg[5] / 10.0f; /* 0x1005，系统汇总交流电流。 */

    g_acdc_modbus_data.ac_l1_voltage_v = reg[6] / 10.0f; /* 0x1006，L1 相/线电压。 */
    g_acdc_modbus_data.ac_l2_voltage_v = reg[7] / 10.0f; /* 0x1007，L2 相/线电压。 */
    g_acdc_modbus_data.ac_l3_voltage_v = reg[8] / 10.0f; /* 0x1008，L3 相/线电压。 */

    g_acdc_modbus_data.ac_l1_current_a = reg[9] / 10.0f;  /* 0x1009，L1 相电流。 */
    g_acdc_modbus_data.ac_l2_current_a = reg[10] / 10.0f; /* 0x100A，L2 相电流。 */
    g_acdc_modbus_data.ac_l3_current_a = reg[11] / 10.0f; /* 0x100B，L3 相电流。 */

    g_acdc_modbus_data.ac_frequency_hz = reg[12]; /* 0x100C，协议精度为 0，直接保存 Hz。 */
    /*
    * 上一帧0x1000～0x100C的响应已经接收并解析完成。
    * 这里等待5ms，确保RS485总线满足Modbus RTU规定的
    * 至少3.5个字符静默时间，然后再发送下一帧请求。
    */
    osDelay(MODBUS_RTU_FRAME_GAP_MS);

    /*
     * 总直流负载功率：UINT32，精度2。
     */
    rc = modbus_master_read_holding_registers(
        ctx,
        ACDC_MODBUS_SLAVE_ADDR,
        0x1022U,
        2U,
        reg); /* 读取两个寄存器组成的总直流负载功率。 */

    if (rc != MODBUS_MASTER_OK) {
        return rc; /* 功率读取失败时终止本轮。 */
    }

    g_acdc_modbus_data.dc_load_power_kw =
        modbus_u32(reg) / 100.0f; /* UINT32 精度 2，除以 100 后得到 kW。 */
    /*
    * 第二个ACDC响应帧处理完成后保留RTU帧间隔，
    * 防止PC模拟从站或真实SMU02C把相邻请求识别成连续数据。
    */
    osDelay(MODBUS_RTU_FRAME_GAP_MS);

    /*
     * 整流模块汇总：0x1100～0x1103。
     */
    rc = modbus_master_read_holding_registers(
        ctx,
        ACDC_MODBUS_SLAVE_ADDR,
        0x1100U,
        4U,
        reg); /* 读取整流模块总电流、数量和总功率。 */

    if (rc != MODBUS_MASTER_OK) {
        return rc; /* 整流模块汇总读取失败时终止本轮。 */
    }

    g_acdc_modbus_data.rectifier_total_current_a =
        reg[0] / 10.0f; /* 0x1100，精度 1，换算为 A。 */

    g_acdc_modbus_data.rectifier_count = reg[1]; /* 0x1101，直接保存整流模块数量。 */

    g_acdc_modbus_data.rectifier_total_power_w =
        modbus_u32(&reg[2]); /* 0x1102～0x1103，高字在前组合为总输出功率 W。 */

    /*
    * 第三个ACDC响应帧处理完成后等待5ms，
    * 保证下一条告警读取请求从新的RTU帧边界开始。
    */
    osDelay(MODBUS_RTU_FRAME_GAP_MS);

    /*
     * 基本交流告警：0x5002～0x500D。
     */
    rc = modbus_master_read_holding_registers(
        ctx,
        ACDC_MODBUS_SLAVE_ADDR,
        0x5002U,
        12U,
        reg); /* 读取交流停电、过压、欠压、缺相等基础告警。 */

    if (rc != MODBUS_MASTER_OK) {
        return rc; /* 告警读取失败时终止本轮。 */
    }

    memcpy(
        (void *)g_acdc_modbus_data.ac_alarm,
        reg,
        sizeof(g_acdc_modbus_data.ac_alarm)); /* 原样保存十二个告警寄存器，交由业务层解释。 */

    return MODBUS_MASTER_OK; /* 四个数据块全部成功，本轮轮询完成。 */
}

static int dcdc_poll(agile_modbus_t *ctx)
{
    uint16_t status_reg[11];   /* 保存 0x0404～0x040E 的状态、故障和温度原始寄存器。 */
    uint16_t realtime_reg[6];  /* 保存 0x041B～0x0420 的功率、电压和电流原始寄存器。 */
    uint8_t index;             /* 遍历八路温度寄存器时使用的数组下标。 */
    int rc;                    /* 保存每一次 DCDC Modbus 读事务的执行结果。 */

    rc = modbus_master_read_holding_registers( /* 发起第一帧 DCDC 功能码 0x03 读请求。 */
        ctx,                                   /* 传入当前任务独占的 Agile Modbus RTU 上下文。 */
        DCDC_MODBUS_SLAVE_ADDR,                /* 指定本次请求访问实机地址为 0xFF 的 DCDC。 */
        0x0404U,                               /* 从 DCDC 工作状态寄存器 0x0404 开始读取。 */
        11U,                                   /* 连续读取到 0x040E，共十一只保持寄存器。 */
        status_reg);                           /* 将解析完成的原始寄存器保存到局部数组。 */

    if (rc != MODBUS_MASTER_OK) {             /* 检查状态、故障和温度数据块是否读取成功。 */
        return rc;                             /* 第一帧失败时不更新全局数据并结束本轮。 */
    }

    /*
    * DCDC第一个数据块0x0404～0x040E已经成功接收。
    * 在读取第二个数据块0x041B～0x0420之前等待5ms，
    * 避免两个RTU事务之间没有足够静默时间。
    */
    osDelay(MODBUS_RTU_FRAME_GAP_MS);

    rc = modbus_master_read_holding_registers( /* 发起第二帧 DCDC 功能码 0x03 读请求。 */
        ctx,                                   /* 继续使用同一个串行执行的 RTU 上下文。 */
        DCDC_MODBUS_SLAVE_ADDR,                /* 第二帧仍然访问同一台 DCDC 从站。 */
        0x041BU,                               /* 从 B 侧实时功率寄存器 0x041B 开始读取。 */
        6U,                                    /* 连续读取到 0x0420，共六只保持寄存器。 */
        realtime_reg);                         /* 将功率、电压和电流原始值保存到局部数组。 */

    if (rc != MODBUS_MASTER_OK) {             /* 检查实时电气量数据块是否读取成功。 */
        return rc;                             /* 第二帧失败时也不提交第一帧的局部数据。 */
    }

    g_dcdc_modbus_data.work_state = status_reg[0]; /* 0x0404 保存原始状态；状态4需结合0x0405区分停机与故障。 */
    g_dcdc_modbus_data.fault_raw = status_reg[1];  /* 0x0405 保存完整故障位掩码，0表示当前没有有效故障。 */

    for (index = 0U; index < 8U; index++) {        /* 依次解析 0x0406～0x040D 八路温度。 */
        g_dcdc_modbus_data.temperature_c[index] =  /* 把当前温度写入对应的工程量数组元素。 */
            (float)(int16_t)status_reg[2U + index] / 10.0f; /* 先按有符号补码解释，再换算为 ℃。 */
    }

    g_dcdc_modbus_data.max_temperature_c =         /* 保存协议中的机器内部最高热点温度。 */
        (float)(int16_t)status_reg[10] / 10.0f;    /* 0x040E 按有符号补码除以 10 得到 ℃。 */

    g_dcdc_modbus_data.b_power_w = realtime_reg[0]; /* 0x041B，B 侧功率原始值直接以 W 保存。 */
    g_dcdc_modbus_data.p_power_w = realtime_reg[1]; /* 0x041C，P 侧功率原始值直接以 W 保存。 */

    g_dcdc_modbus_data.b_voltage_v =               /* 保存 B 侧实时工作电压。 */
        (float)(int16_t)realtime_reg[2] / 10.0f;   /* 0x041D 按有符号补码除以 10 得到 V。 */

    g_dcdc_modbus_data.p_voltage_v =               /* 保存 P 侧实时工作电压。 */
        (float)realtime_reg[3] / 10.0f;            /* 0x041E 按无符号数除以 10 得到 V。 */

    g_dcdc_modbus_data.b_current_a =               /* 保存 B 侧实时工作电流及其方向符号。 */
        (float)(int16_t)realtime_reg[4] / 100.0f;  /* 0x041F 按有符号补码除以 100 得到 A。 */

    g_dcdc_modbus_data.p_current_a =               /* 保存 P 侧实时工作电流及其方向符号。 */
        (float)(int16_t)realtime_reg[5] / 100.0f;  /* 0x0420 按有符号补码除以 100 得到 A。 */

    return MODBUS_MASTER_OK;                       /* 两个数据块均成功，本轮 DCDC 轮询完成。 */
}

static void acdc_update_communication_state(int rc)
{
    if (rc == MODBUS_MASTER_OK) {                  /* 判断本轮 ACDC 四个数据块是否全部成功。 */
        g_acdc_modbus_data.online = 1U;            /* 完整成功后将 ACDC 标记为在线。 */
        g_acdc_modbus_data.consecutive_failures = 0U; /* 完整成功后清除 ACDC 连续失败次数。 */
        g_acdc_modbus_data.last_error = 0;         /* 完整成功后清除 ACDC 最近错误码。 */
        g_acdc_modbus_data.last_update_tick = HAL_GetTick(); /* 记录 ACDC 完整数据更新时间。 */
        g_acdc_modbus_data.success_count++;        /* 累加 ACDC 完整轮询成功次数。 */
        return;                                    /* 成功状态已经处理完毕，直接返回。 */
    }

    g_acdc_modbus_data.last_error = (int16_t)rc;   /* 保存本轮 ACDC 失败原因。 */

    if (g_acdc_modbus_data.consecutive_failures < 0xFFU) { /* 防止八位失败计数发生回绕。 */
        g_acdc_modbus_data.consecutive_failures++; /* 在未达到 255 时累加 ACDC 连续失败次数。 */
    }

    if (rc == MODBUS_MASTER_TIMEOUT) {             /* 判断 ACDC 是否因为完全无响应而失败。 */
        g_acdc_modbus_data.timeout_count++;        /* 累加 ACDC 响应超时次数。 */
    } else {                                       /* 处理发送、CRC、地址、功能码或长度错误。 */
        g_acdc_modbus_data.protocol_error_count++; /* 累加 ACDC 非超时通信错误次数。 */
    }

    if (g_acdc_modbus_data.consecutive_failures >= MODBUS_OFFLINE_LIMIT) { /* 检查离线阈值。 */
        g_acdc_modbus_data.online = 0U;            /* 仅将 ACDC 标记为离线，不影响 DCDC。 */
    }
}

static void dcdc_update_communication_state(int rc)
{
    if (rc == MODBUS_MASTER_OK) {                  /* 判断本轮 DCDC 两个数据块是否全部成功。 */
        g_dcdc_modbus_data.online = 1U;            /* 完整成功后将 DCDC 标记为在线。 */
        g_dcdc_modbus_data.consecutive_failures = 0U; /* 完整成功后清除 DCDC 连续失败次数。 */
        g_dcdc_modbus_data.last_error = 0;         /* 完整成功后清除 DCDC 最近错误码。 */
        g_dcdc_modbus_data.last_update_tick = HAL_GetTick(); /* 记录 DCDC 完整数据更新时间。 */
        g_dcdc_modbus_data.success_count++;        /* 累加 DCDC 完整轮询成功次数。 */
        return;                                    /* 成功状态已经处理完毕，直接返回。 */
    }

    g_dcdc_modbus_data.last_error = (int16_t)rc;   /* 保存本轮 DCDC 失败原因。 */

    if (g_dcdc_modbus_data.consecutive_failures < 0xFFU) { /* 防止八位失败计数发生回绕。 */
        g_dcdc_modbus_data.consecutive_failures++; /* 在未达到 255 时累加 DCDC 连续失败次数。 */
    }

    if (rc == MODBUS_MASTER_TIMEOUT) {             /* 判断 DCDC 是否因为完全无响应而失败。 */
        g_dcdc_modbus_data.timeout_count++;        /* 累加 DCDC 响应超时次数。 */
    } else {                                       /* 处理发送、CRC、地址、功能码或长度错误。 */
        g_dcdc_modbus_data.protocol_error_count++; /* 累加 DCDC 非超时通信错误次数。 */
    }

    if (g_dcdc_modbus_data.consecutive_failures >= MODBUS_OFFLINE_LIMIT) { /* 检查离线阈值。 */
        g_dcdc_modbus_data.online = 0U;            /* 仅将 DCDC 标记为离线，不影响 ACDC。 */
    }
}

static void dcdc_process_write_command(agile_modbus_t *ctx)
{
    dcdc_modbus_write_command_t command;                    /* 保存从队列取出的完整写命令副本。 */
    uint16_t verify_values[DCDC_MODBUS_MAX_WRITE_REGISTERS]; /* 保存功能码 0x03 的写后读回数据。 */
    uint16_t index;                                         /* 用于逐项比较目标值和读回值。 */
    uint8_t attempt;                                        /* 保存当前是第几次有限写尝试。 */
    uint8_t values_match;                                   /* 1表示本次0x03读回值与目标完全一致。 */
    int write_rc;                                           /* 保存功能码0x06或0x10写响应结果。 */
    int readback_rc;                                        /* 保存功能码0x03读回事务结果。 */
    int rc = MODBUS_MASTER_VERIFY_ERROR;                    /* 保存本条命令最终对外报告的结果。 */

    if (dcdc_write_queue == NULL) {                         /* 队列创建失败时不能处理写命令。 */
        return;                                             /* 保持常规轮询运行并直接返回。 */
    }

    if (xQueueReceive(dcdc_write_queue, &command, 0U) != pdPASS) { /* 非阻塞检查是否存在待写命令。 */
        return;                                             /* 队列为空时不占用总线。 */
    }

    g_dcdc_modbus_write_status.state = DCDC_WRITE_STATE_RUNNING; /* 通知业务层命令已开始执行。 */
    g_dcdc_modbus_write_status.started_tick = HAL_GetTick(); /* 记录任务真正开始处理该命令的时刻。 */
    g_dcdc_modbus_write_status.attempt_count = 0U;          /* 执行前尚未发起任何写事务。 */
    g_dcdc_modbus_write_status.readback_confirmed = 0U;     /* 只有后续严格读回一致才会置一。 */
    g_dcdc_modbus_write_status.last_error = 0;              /* 开始执行前清除上一阶段错误码。 */
    memset(verify_values, 0, sizeof(verify_values));        /* 清零局部读回数组，避免残留栈数据。 */

    if ((dcdc_modbus_data_is_fresh() == 0U) &&              /* 执行前再次检查，防止排队期间数据变旧。 */
        (dcdc_write_is_safe_stop(command.address, command.values, command.count) == 0U)) { /* 关机/待机仍允许尝试。 */
        rc = DCDC_WRITE_EXEC_STALE_DATA;                    /* 未发总线写请求，直接报告数据过期。 */
        g_dcdc_modbus_write_status.stale_reject_count++;    /* 累加执行阶段的新鲜度拒绝次数。 */
    } else if ((dcdc_write_is_start(command.address, command.values, command.count) != 0U) && /* 排队期间可能出现新故障。 */
               (dcdc_modbus_is_faulted() != 0U)) {          /* 执行前再次读取0x0405镜像并禁止带故障启动。 */
        rc = DCDC_WRITE_EXEC_DEVICE_FAULT;                  /* 不向总线发出启动请求。 */
        g_dcdc_modbus_write_status.device_fault_reject_count++; /* 累加执行阶段故障拒绝次数。 */
    } else if ((HAL_GetTick() - command.submitted_tick) >=  /* 检查命令是否已经在队列中等待过久。 */
               DCDC_MODBUS_WRITE_TOTAL_TIMEOUT_MS) {
        rc = DCDC_WRITE_EXEC_TOTAL_TIMEOUT;                 /* 过期命令不得再改变DCDC状态。 */
        g_dcdc_modbus_write_status.total_timeout_count++;   /* 累加命令总超时次数。 */
    } else {
        for (attempt = 1U; attempt <= DCDC_MODBUS_WRITE_MAX_ATTEMPTS; attempt++) { /* 执行有限次数尝试。 */
            if ((HAL_GetTick() - command.submitted_tick) >= /* 每次真正写入前重新检查总时限。 */
                DCDC_MODBUS_WRITE_TOTAL_TIMEOUT_MS) {
                rc = DCDC_WRITE_EXEC_TOTAL_TIMEOUT;         /* 禁止开始可能已经过时的新尝试。 */
                g_dcdc_modbus_write_status.total_timeout_count++; /* 记录总时限保护动作。 */
                break;
            }

            g_dcdc_modbus_write_status.attempt_count = attempt; /* 公开当前尝试次数供Watch观察。 */
            memset(verify_values, 0, sizeof(verify_values)); /* 每次读回前清除上一次尝试的值。 */

            write_rc = modbus_master_write_registers(      /* 发送功能码0x06或0x10写请求。 */
                ctx,                                       /* 使用唯一Modbus任务的RTU上下文。 */
                DCDC_MODBUS_SLAVE_ADDR,                    /* 写框架只访问地址0xFF的真实DCDC。 */
                command.address,                           /* 使用提交时保存的目标起始地址。 */
                command.count,                             /* 使用提交时保存的连续写入数量。 */
                command.values);                           /* 使用队列内部稳定副本，避免调用者改值。 */

            /*
             * 即使写响应超时或CRC错误，也不能立即盲目重发。
             * 从站可能已经执行写入但响应在链路中损坏，因此先读回目标地址确认实际状态。
             */
            osDelay(MODBUS_RTU_FRAME_GAP_MS);               /* 写事务结束后保留完整RTU静默间隔。 */
            readback_rc = modbus_master_read_holding_registers( /* 使用0x03确认从站实际保存值。 */
                ctx,                                       /* 继续使用当前唯一RTU上下文。 */
                DCDC_MODBUS_SLAVE_ADDR,                    /* 从同一台DCDC读取确认值。 */
                command.address,                           /* 读回起始地址与写入地址严格一致。 */
                command.count,                             /* 读回数量与写入数量严格一致。 */
                verify_values);                            /* 保存本次协议解析后的实际值。 */

            values_match = 0U;                             /* 默认认为尚未确认写入成功。 */
            if (readback_rc == MODBUS_MASTER_OK) {         /* 只有合法0x03响应才能比较寄存器值。 */
                values_match = 1U;                         /* 先假设全部读回值一致。 */
                for (index = 0U; index < command.count; index++) { /* 逐项严格比较目标和实际值。 */
                    if (verify_values[index] != command.values[index]) {
                        values_match = 0U;                 /* 任一寄存器不同即未确认成功。 */
                        break;
                    }
                }
            }

            if (values_match != 0U) {                      /* 读回一致是写成功的唯一判据。 */
                rc = MODBUS_MASTER_OK;                     /* 即使写响应丢失，也可确认设备已经执行。 */
                g_dcdc_modbus_write_status.readback_confirmed = 1U; /* 向业务层公开确认结果。 */
                break;                                    /* 已成功，不再重复发送控制命令。 */
            }

            if (write_rc != MODBUS_MASTER_OK) {            /* 写响应本身失败时优先保留该错误。 */
                rc = write_rc;
            } else if (readback_rc != MODBUS_MASTER_OK) {  /* 写响应正常但读回事务失败。 */
                rc = readback_rc;
            } else {
                rc = MODBUS_MASTER_VERIFY_ERROR;           /* 两笔事务均正常但从站值与目标不一致。 */
            }

            if (attempt < DCDC_MODBUS_WRITE_MAX_ATTEMPTS) { /* 尚有剩余次数时才进入重试等待。 */
                osDelay(DCDC_MODBUS_WRITE_RETRY_DELAY_MS); /* 避免连续请求过快占用总线。 */
            }
        }
    }

    memset((void *)g_dcdc_modbus_write_status.readback, 0,  /* 清零上一条命令公开的读回数组。 */
           sizeof(g_dcdc_modbus_write_status.readback));    /* 确保未使用的尾部元素保持为零。 */
    memcpy((void *)g_dcdc_modbus_write_status.readback,     /* 复制本次实际读取到的校验数据。 */
           verify_values, command.count * sizeof(uint16_t)); /* 仅复制本次命令覆盖的寄存器数量。 */
    g_dcdc_modbus_write_status.last_error = (int16_t)rc;    /* 保存最终写入或读回校验结果。 */
    g_dcdc_modbus_write_status.completed_tick = HAL_GetTick(); /* 记录本条命令完成时刻。 */

    if ((rc == MODBUS_MASTER_OK) &&                         /* 成功必须同时满足事务结果为零， */
        (g_dcdc_modbus_write_status.readback_confirmed != 0U)) { /* 且已经取得严格一致的0x03读回。 */
        g_dcdc_modbus_write_status.state = DCDC_WRITE_STATE_SUCCESS; /* 向业务层报告写命令成功。 */
    } else {                                               /* 处理超时、帧错误或读回不一致。 */
        g_dcdc_modbus_write_status.state = DCDC_WRITE_STATE_FAILED; /* 向业务层报告写命令失败。 */
    }

    taskENTER_CRITICAL();                                  /* 原子释放唯一写命令槽位。 */
    dcdc_write_busy = 0U;                                  /* 允许业务层提交下一条 DCDC 写命令。 */
    taskEXIT_CRITICAL();                                   /* 共享忙标志更新完成后退出临界区。 */
}

static void modbus_task(void const *args)
{
    agile_modbus_rtu_t mb_rtu;             /* 在任务栈上创建 RTU 模式上下文对象。 */
    agile_modbus_t *ctx = &mb_rtu._ctx;    /* 获取通用 Agile Modbus 上下文指针。 */
    int acdc_rc;                            /* 保存整轮 ACDC 只读轮询结果。 */
    int dcdc_rc;                           /* 保存整轮 DCDC 轮询结果。 */

    (void)args; /* 当前任务不使用创建时传入的参数。 */

    agile_modbus_rtu_init(
        &mb_rtu,
        modbus_send_buf,
        sizeof(modbus_send_buf),
        modbus_read_buf,
        sizeof(modbus_read_buf)); /* 绑定静态收发缓冲并初始化 Modbus RTU 后端。 */

    while (1) {
        acdc_rc = acdc_poll(ctx);                      /* 先按固定数据块顺序完成一次 ACDC 只读轮询。 */
        acdc_update_communication_state(acdc_rc);      /* 独立更新 ACDC 在线状态，不提供任何写入口。 */

        /*
        * ACDC最后一帧结束后不能立即切换到DCDC发送下一帧。
        * 这里保留5ms静默时间，使从站地址33与地址1的RTU事务
        * 在物理总线上具有清晰的帧边界。
        *
        * 即使本轮ACDC失败，也等待相同时间再访问DCDC，
        * 避免ACDC迟到响应被误认为DCDC响应。
        */
        osDelay(MODBUS_RTU_FRAME_GAP_MS);

        dcdc_rc = dcdc_poll(ctx);                    /* 无论 ACDC 是否成功，都继续执行 DCDC 轮询。 */
        dcdc_update_communication_state(dcdc_rc);    /* 根据本轮结果更新 DCDC 自己的在线状态。 */

        /*
        * 本轮DCDC周期读取结束后保留RTU帧间隔。
        * 如果队列中存在写命令，可以确保写请求不会紧跟在读响应之后发送。
        */
        osDelay(MODBUS_RTU_FRAME_GAP_MS);
#if DCDC_MODBUS_PC_TEST_ENABLE
        dcdc_process_pc_test_trigger();              /* 检查 Keil Watch 是否写入 0x5AA5 PC 模拟触发值。 */
#endif
        dcdc_process_write_command(ctx);               /* 每轮最多执行一条写命令，并在保护范围内重试和读回确认。 */
        osDelay(MODBUS_POLL_PERIOD_MS);              /* 一轮双从站轮询结束后延时，避免持续占用总线。 */
    }
}
