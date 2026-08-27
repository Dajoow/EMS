#ifndef __MODBUS_H_                         /* 防止头文件被重复包含。 */
#define __MODBUS_H_                         /* 定义 Modbus 模块头文件保护宏。 */

#include <stdint.h>                         /* 引入 uint8_t、uint16_t 和 uint32_t 等定长类型。 */

/*
 * ACDC 和 DCDC 共用 USART2 对应的同一条二线 RS485 总线。
 *
 * 总线上只能有一个主站，本工程中的 STM32H750 EMS 是唯一主站；
 * ACDC 和 DCDC 都是从站，因此两台设备必须使用不同的从站地址。
 *
 * 注意：
 * 1. ACDC 地址 0x21 来自当前 SMU02C 协议示例，仍需与实机设置核对。
 * 2. PC Modbus Slave 模拟阶段曾使用地址 0x01；真实 DCDC 实机联调已确认使用地址 0xFF。
 * 3. DCDC 文档中 0x0400 同时出现“模块号”和“保留”描述，设备更换或恢复出厂后仍需重新核对地址。
 */
#define ACDC_MODBUS_SLAVE_ADDR           0x21U  /* ACDC/SMU02C 从站地址，十进制为 33。 */
#define DCDC_MODBUS_SLAVE_ADDR           0xFFU  /* 当前真实 DCDC 已确认的从站地址，十六进制为 0xFF。 */

/*
 * 这是整条 Modbus RTU 总线共用的响应超时，不应继续使用 ACDC 专用名称。
 *
 * 主站发完请求后，USART2 DMA 开始等待从站响应；
 * 如果 500 ms 内没有收到完整响应，则本次事务判定为超时。
 */
#define MODBUS_RESPONSE_TIMEOUT_MS       500U

/*
 * 一轮 ACDC 和 DCDC 快速数据轮询结束后的延迟。
 *
 * 实际轮询周期还要加上每次 Modbus 请求和响应所消耗的时间；
 * 如果某个从站离线，还要加上该从站的一次 500 ms 超时。
 */
#define MODBUS_POLL_PERIOD_MS            500U

/*
 * Modbus RTU要求相邻报文之间至少保持3.5个字符时间的总线静默。
 * 当前串口参数为9600bps、8N1，一个字符约占1.04ms，
 * 3.5个字符约为3.65ms，因此取5ms并留出调度和晶振误差余量。
 */
#define MODBUS_RTU_FRAME_GAP_MS          5U

/*
 * 某个从站连续通信失败达到 3 次后，将该从站标记为离线。
 *
 * ACDC 和 DCDC 必须分别统计失败次数，不能因为 ACDC 离线就把 DCDC
 * 也标记为离线，反之亦然。
 */
#define MODBUS_OFFLINE_LIMIT             3U
#define DCDC_MODBUS_WRITE_QUEUE_LENGTH   1U     /* 只允许一条 DCDC 写命令等待执行，防止控制命令堆积。 */
#define DCDC_MODBUS_MAX_WRITE_REGISTERS  14U    /* 0x0427～0x0434 参数区最多连续十四只寄存器。 */
#define DCDC_MODBUS_DATA_FRESHNESS_MS    2000U  /* 除安全停机外，写命令要求最近完整轮询数据不超过 2 秒。 */
#define DCDC_MODBUS_WRITE_MAX_ATTEMPTS   3U     /* 单条写命令最多尝试三次，避免故障总线上无限重试。 */
#define DCDC_MODBUS_WRITE_RETRY_DELAY_MS 20U    /* 两次写入尝试之间保留 20 ms 总线恢复时间。 */
#define DCDC_MODBUS_WRITE_TOTAL_TIMEOUT_MS 4000U /* 从提交起最多允许执行 4 秒，超时命令不得继续写设备。 */
#define DCDC_MODBUS_PC_TEST_ENABLE       0U     /* 正式界面接入后关闭 Keil Watch 写触发器，避免与 TouchGFX 控制入口并存。 */
#define DCDC_MODBUS_PC_TEST_MAGIC        0x5AA5U /* 只有 trigger 写入该魔术值时才提交 PC 模拟写命令。 */

/* DCDC 0x0402 工作模式选择寄存器的有效值。 */
#define DCDC_WORK_MODE_STANDBY           0U     /* 写 0 请求设备进入待机状态。 */
#define DCDC_WORK_MODE_CV_CURRENT_LIMIT  1U     /* 写 1 选择恒压限流模式。 */

/* DCDC 0x0403 启停控制寄存器的命令值。 */
#define DCDC_RUN_COMMAND_START           4U     /* 写 4 执行手动开机。 */
#define DCDC_RUN_COMMAND_STOP            5U     /* 写 5 执行手动关机。 */

/* DCDC 0x0404 原始状态值；状态 4 必须结合 0x0405 才能区分停机和故障。 */
#define DCDC_WORK_STATE_CV_CURRENT_LIMIT 1U     /* 恒压限流运行状态。 */
#define DCDC_WORK_STATE_STOP_OR_FAULT    4U     /* 停机/待机或故障共用的厂家状态值。 */

/* DCDC 0x0405 故障位定义；允许按位组合。 */
#define DCDC_FAULT_P_OVERVOLTAGE         0x0001U /* P 侧过压。 */
#define DCDC_FAULT_P_UNDERVOLTAGE        0x0002U /* P 侧欠压。 */
#define DCDC_FAULT_B_OVERVOLTAGE         0x0004U /* B 侧过压。 */
#define DCDC_FAULT_B_UNDERVOLTAGE        0x0008U /* B 侧欠压。 */
#define DCDC_FAULT_OVERCURRENT           0x0020U /* 充放电过流。 */
#define DCDC_FAULT_CHARGE_OVERTEMP       0x0100U /* 充电高温保护。 */
#define DCDC_FAULT_OVERTEMP              0x0200U /* 高温保护。 */

#define MODBUS_BLOCKING_RX_DIAG_ENABLE   0U     /* 0 恢复 USART2 DMA+IDLE 接收；改为 1 时才启用阻塞定长 A/B 诊断。 */
#define MODBUS_RX_DIAG_ENABLE            1U     /* 1 编译详细收帧诊断；正式版本改为 0 可完全移除诊断代码和变量。 */
#define MODBUS_RX_DIAG_RAW_MAX           32U    /* 最多保存异常响应前 32 字节，兼顾问题定位和 RAM 占用。 */

#define MODBUS_RX_DIAG_STAGE_NONE         0U    /* 尚未记录任何异常。 */
#define MODBUS_RX_DIAG_STAGE_PARAMETER    1U    /* 主站读函数收到非法参数。 */
#define MODBUS_RX_DIAG_STAGE_SERIALIZE    2U    /* Agile Modbus 生成 0x03 请求帧失败。 */
#define MODBUS_RX_DIAG_STAGE_TRANSMIT     3U    /* USART2 发送请求帧失败。 */
#define MODBUS_RX_DIAG_STAGE_TIMEOUT      4U    /* 规定时间内没有收到 USART2 IDLE 任务通知。 */
#define MODBUS_RX_DIAG_STAGE_RECEIVE_FLAG 5U    /* 收到任务通知但接收完成标志没有置位。 */
#define MODBUS_RX_DIAG_STAGE_LENGTH       6U    /* USART2 DMA 计算出的响应长度非法。 */
#define MODBUS_RX_DIAG_STAGE_DESERIALIZE  7U    /* CRC、地址、功能码、字节数或寄存器数量校验失败。 */

typedef enum                                  /* 定义异步 DCDC 写命令的执行状态。 */
{
    DCDC_WRITE_STATE_IDLE = 0,                /* 当前没有已提交或正在执行的写命令。 */
    DCDC_WRITE_STATE_PENDING = 1,             /* 写命令已进入队列，等待 Modbus 任务取出。 */
    DCDC_WRITE_STATE_RUNNING = 2,             /* Modbus 任务正在发送写请求或执行读回校验。 */
    DCDC_WRITE_STATE_SUCCESS = 3,             /* 写响应和 0x03 读回数据均验证成功。 */
    DCDC_WRITE_STATE_FAILED = 4               /* 写响应失败、读回失败或读回数据不一致。 */
} dcdc_modbus_write_state_t;                  /* DCDC 异步写状态类型名称。 */

typedef enum                                  /* 定义提交 DCDC 写命令时立即返回的结果。 */
{
    DCDC_WRITE_SUBMIT_OK = 0,                 /* 命令参数有效，并已成功放入写命令队列。 */
    DCDC_WRITE_SUBMIT_INVALID = -1,           /* 地址、数量、数值或指针不符合安全规则。 */
    DCDC_WRITE_SUBMIT_BUSY = -2,              /* 上一条写命令尚未完成，本次命令未被接收。 */
    DCDC_WRITE_SUBMIT_NOT_READY = -3,         /* 写命令队列尚未创建，Modbus 模块未初始化。 */
    DCDC_WRITE_SUBMIT_STALE_DATA = -4,        /* DCDC 数据不新鲜；除安全停机外拒绝提交控制命令。 */
    DCDC_WRITE_SUBMIT_DEVICE_FAULT = -7       /* 0x0405 存在有效故障位，拒绝提交启动命令。 */
} dcdc_modbus_write_submit_result_t;          /* DCDC 写命令提交结果类型名称。 */

#define DCDC_WRITE_EXEC_STALE_DATA       (-5) /* 命令执行前数据已过期，未向 DCDC 发出写请求。 */
#define DCDC_WRITE_EXEC_TOTAL_TIMEOUT     (-6) /* 命令从提交起已超过总时限，停止后续尝试。 */
#define DCDC_WRITE_EXEC_DEVICE_FAULT      (-8) /* 命令执行前发现 0x0405 故障，未向 DCDC 发出启动请求。 */

typedef struct                                /* 定义 EMS 内部保存的 ACDC 实时数据镜像。 */
{
    /* 0x1000～0x100C */
    float dc_bus_voltage_v;                   /* 系统直流母排电压，单位 V。 */
    float dc_load_current_a;                  /* 系统总直流负载电流，单位 A。 */
    float ac_voltage_v;                       /* 系统汇总交流电压，单位 V。 */
    float ac_current_a;                       /* 系统汇总交流电流，单位 A。 */
    float ac_l1_voltage_v;                    /* L1 相或 L1-L2 线电压，单位 V。 */
    float ac_l2_voltage_v;                    /* L2 相或 L2-L3 线电压，单位 V。 */
    float ac_l3_voltage_v;                    /* L3 相或 L3-L1 线电压，单位 V。 */
    float ac_l1_current_a;                    /* L1 相电流，单位 A。 */
    float ac_l2_current_a;                    /* L2 相电流，单位 A。 */
    float ac_l3_current_a;                    /* L3 相电流，单位 A。 */
    uint16_t ac_frequency_hz;                 /* 交流频率，单位 Hz。 */

    /* 0x1022～0x1023 */
    float dc_load_power_kw;                   /* 系统总直流负载功率，单位 kW。 */

    /* 0x1100～0x1103 */
    float rectifier_total_current_a;          /* 全部整流模块的总输出电流，单位 A。 */
    uint16_t rectifier_count;                 /* 当前系统识别到的整流模块数量。 */
    uint32_t rectifier_total_power_w;         /* 全部整流模块的总输出功率，单位 W。 */

    /* 0x5002～0x500D */
    uint16_t ac_alarm[12];                    /* 交流停电、过欠压、缺相等十二项原始告警值。 */

    uint8_t online;                           /* 通信在线标志：1 在线，0 离线。 */
    uint8_t consecutive_failures;             /* 当前连续通信失败次数。 */
    int16_t last_error;                       /* 最近一次主站事务的错误码。 */
    uint32_t last_update_tick;                /* 最近一次完整轮询成功时的 HAL Tick。 */
    uint32_t success_count;                   /* 完整轮询成功的累计次数。 */
    uint32_t timeout_count;                   /* 等待响应超时的累计次数。 */
    uint32_t protocol_error_count;            /* CRC、地址、功能码或长度错误的累计次数。 */
} acdc_modbus_data_t;                         /* ACDC Modbus 数据类型名称。 */

typedef struct                                /* 定义 EMS 内部保存的 DCDC 实时数据镜像。 */
{
    uint16_t work_state;                      /* 0x0404 原始状态：1 恒压限流；4 表示停机/待机或故障，须结合 fault_raw。 */
    uint16_t fault_raw;                       /* 0x0405 原始故障位掩码；0 表示当前没有有效故障，非零可按 DCDC_FAULT_* 拆分。 */
    float temperature_c[8];                   /* 0x0406～0x040D，八路有符号温度，原始值除以 10 得到 ℃。 */
    float max_temperature_c;                  /* 0x040E，机器内部最高温度，原始有符号值除以 10。 */
    uint16_t b_power_w;                       /* 0x041B，B 侧实时功率，协议示例表明原始值单位为 W。 */
    uint16_t p_power_w;                       /* 0x041C，P 侧实时功率，协议示例表明原始值单位为 W。 */
    float b_voltage_v;                        /* 0x041D，B 侧有符号实时电压，原始值除以 10 得到 V。 */
    float p_voltage_v;                        /* 0x041E，P 侧无符号实时电压，原始值除以 10 得到 V。 */
    float b_current_a;                        /* 0x041F，B 侧有符号实时电流，原始值除以 100 得到 A。 */
    float p_current_a;                        /* 0x0420，P 侧有符号实时电流，原始值除以 100 得到 A。 */
    uint8_t online;                           /* DCDC 通信在线标志：1 在线，0 离线。 */
    uint8_t consecutive_failures;             /* DCDC 当前连续通信失败次数，成功一次后清零。 */
    int16_t last_error;                       /* DCDC 最近一次主站事务错误码，0 表示成功。 */
    uint32_t last_update_tick;                /* DCDC 最近一次完整轮询成功时的 HAL Tick。 */
    uint32_t success_count;                   /* DCDC 完整轮询成功的累计次数。 */
    uint32_t timeout_count;                   /* DCDC 等待响应超时的累计次数。 */
    uint32_t protocol_error_count;            /* DCDC CRC、地址、功能码、长度或发送错误累计次数。 */
} dcdc_modbus_data_t;                         /* DCDC Modbus 数据类型名称。 */

typedef struct                                /* 保存最近一条 DCDC 写命令的执行和校验结果。 */
{
    uint8_t state;                            /* 当前执行状态，取值见 dcdc_modbus_write_state_t。 */
    uint8_t function_code;                    /* 实际使用的功能码：单写为 0x06，多写为 0x10。 */
    uint8_t attempt_count;                    /* 本条命令已经发起的写尝试次数，最大为三次。 */
    uint8_t readback_confirmed;               /* 0x03 读回值与请求值完全一致时置 1。 */
    uint16_t address;                         /* 最近一条写命令的保持寄存器起始地址。 */
    uint16_t count;                           /* 最近一条写命令连续写入的寄存器数量。 */
    uint16_t requested[DCDC_MODBUS_MAX_WRITE_REGISTERS]; /* 最近一次要求写入的原始寄存器值。 */
    uint16_t readback[DCDC_MODBUS_MAX_WRITE_REGISTERS];  /* 写成功后使用 0x03 读取的校验值。 */
    int16_t last_error;                       /* 最近写事务错误码，0 成功，-4 表示读回不一致。 */
    uint32_t sequence;                        /* 每接受一条新命令就递增，用于区分不同测试。 */
    uint32_t submitted_tick;                  /* 命令成功进入队列时的 HAL Tick。 */
    uint32_t started_tick;                    /* Modbus 任务开始执行该命令时的 HAL Tick。 */
    uint32_t completed_tick;                  /* 最近一条命令完成时的 HAL Tick。 */
    uint32_t stale_reject_count;              /* 因 DCDC 数据不新鲜而拒绝的累计次数。 */
    uint32_t device_fault_reject_count;       /* 因 0x0405 存在故障而拒绝启动的累计次数。 */
    uint32_t total_timeout_count;              /* 因命令总执行时间超限而终止的累计次数。 */
} dcdc_modbus_write_status_t;                 /* DCDC 写命令状态类型名称。 */

typedef struct                                /* 定义仅用于 PC Modbus Slave 模拟的 Keil Watch 触发器。 */
{
    uint16_t trigger;                         /* 先填写其他字段，最后写 0x5AA5 才会触发一次命令。 */
    uint16_t address;                         /* 模拟写命令的保持寄存器起始地址。 */
    uint16_t count;                           /* 写入数量：1 使用 0x06，大于 1 使用 0x10。 */
    uint16_t values[DCDC_MODBUS_MAX_WRITE_REGISTERS]; /* 需要写入 Modbus Slave 的原始寄存器值。 */
    int16_t submit_result;                    /* 最近一次触发的提交结果，0 表示已经成功排队。 */
} dcdc_modbus_pc_test_t;                      /* PC 模拟写触发器类型名称。 */

#if MODBUS_RX_DIAG_ENABLE
typedef struct                                /* 保存 DCDC 实机读通信最近一次异常及分类累计值。 */
{
    uint32_t sequence;                        /* 每记录一次失败最后递增，表示全部诊断字段已更新。 */
    uint32_t status_block_failure_count;      /* 读取 0x0404～0x040E 状态数据块的失败次数。 */
    uint32_t realtime_block_failure_count;    /* 读取 0x041B～0x0420 实时数据块的失败次数。 */
    uint32_t other_address_failure_count;     /* 其他起始地址的失败次数，当前 DCDC 只读测试应为零。 */
    uint32_t timeout_failure_count;           /* 等待从站响应超时次数。 */
    uint32_t protocol_failure_count;          /* 除超时外的参数、组帧、发送和响应协议失败次数。 */
    uint32_t parameter_failure_count;         /* 主站读函数参数错误次数。 */
    uint32_t serialize_failure_count;         /* 0x03 请求组帧失败次数。 */
    uint32_t transmit_failure_count;          /* USART2 请求发送失败次数。 */
    uint32_t receive_flag_failure_count;      /* 任务通知和接收完成标志不一致次数。 */
    uint32_t receive_length_failure_count;    /* DMA 响应长度非法次数。 */
    uint32_t deserialize_failure_count;       /* 地址、功能码、长度、CRC 或数量解析失败次数。 */
    uint32_t crc_mismatch_count;              /* 解帧失败中计算 CRC 与接收 CRC 不一致的次数。 */
    uint32_t confirmation_failure_count;      /* CRC 正确但地址、功能码、长度或数量确认失败的次数。 */
    uint32_t buffer_copy_mismatch_count;      /* ISR 接收缓冲复制到解析缓冲后内容不一致的次数。 */
    uint32_t uart_parity_error_count;         /* USART2 入口 ISR 快照中出现奇偶校验错误的次数。 */
    uint32_t uart_noise_error_count;          /* USART2 入口 ISR 快照中出现噪声错误的次数。 */
    uint32_t uart_frame_error_count;          /* USART2 入口 ISR 快照中出现帧格式错误的次数。 */
    uint32_t uart_overrun_error_count;        /* USART2 入口 ISR 快照中出现接收溢出的次数。 */
    uint16_t request_address;                 /* 最近一次失败请求的起始寄存器地址。 */
    uint16_t request_count;                   /* 最近一次失败请求的寄存器数量。 */
    uint16_t expected_length;                 /* 根据寄存器数量计算的正常 0x03 RTU 响应长度。 */
    uint16_t received_length;                 /* 最近一次失败时 USART2 DMA 实际得到的响应长度。 */
    uint16_t dma_remaining;                   /* 最近失败帧触发 IDLE 时 DMA 尚未接收的缓冲字节数。 */
    uint16_t isr_received_length;             /* USART2 IDLE 中断现场计算出的原始接收长度。 */
    uint16_t dma_received_crc;                /* DMA 原始缓冲区最后两字节，按线上顺序组合。 */
    uint16_t isr_receive_buffer_crc;          /* ISR memcpy 后接收缓冲区最后两字节。 */
    uint16_t task_receive_buffer_crc;         /* Modbus 任务复制前接收缓冲区最后两字节。 */
    uint16_t parser_buffer_crc;               /* 复制完成后解析缓冲区最后两字节。 */
    uint16_t calculated_crc;                  /* 对解析缓冲区除末两字节外计算出的 Modbus CRC。 */
    uint16_t received_crc;                    /* 解析缓冲区实际携带的末两字节 CRC。 */
    uint16_t copy_mismatch_index;             /* 两缓冲区首个不同字节下标；0xFFFF 表示完全一致。 */
    int16_t parser_result;                    /* 最近一次主站内部错误码或 Agile Modbus 解析结果。 */
    uint32_t uart_isr_snapshot;               /* 进入 USART2 中断时读取的硬件 ISR 寄存器。 */
    uint32_t uart_error_code;                 /* HAL_UART_IRQHandler 执行后的 huart2.ErrorCode。 */
    uint8_t failure_stage;                    /* 最近一次失败阶段，取值见 MODBUS_RX_DIAG_STAGE_*。 */
    uint8_t raw_length;                       /* raw[] 中本次实际保存的字节数。 */
    uint8_t copy_source_byte;                 /* 首个复制不一致位置在接收缓冲区中的字节。 */
    uint8_t copy_destination_byte;            /* 首个复制不一致位置在解析缓冲区中的字节。 */
    uint8_t raw[MODBUS_RX_DIAG_RAW_MAX];      /* 最近一次异常响应的前 32 字节。 */
} modbus_rx_diag_t;                           /* Modbus RTU 收帧详细诊断类型名称。 */
#endif

extern volatile acdc_modbus_data_t g_acdc_modbus_data; /* 导出数据镜像，供 EMS 策略和界面只读访问。 */
extern volatile dcdc_modbus_data_t g_dcdc_modbus_data;  /* 导出 DCDC 数据镜像，供 EMS 策略和界面只读访问。 */
extern volatile dcdc_modbus_write_status_t g_dcdc_modbus_write_status; /* 导出最近一次 DCDC 写命令状态。 */
#if MODBUS_RX_DIAG_ENABLE
extern volatile modbus_rx_diag_t g_modbus_rx_diag; /* 导出详细收帧诊断，供 Keil Watch 只读观察。 */
void modbus_rx_diag_capture_isr(              /* 仅由 USART2 IDLE 中断保存轻量现场快照。 */
    uint32_t uart_isr_snapshot,               /* 进入中断且尚未由 HAL 清除时的 USART2 ISR。 */
    uint32_t uart_error_code,                 /* HAL 中断处理完成后的 UART 错误码。 */
    uint16_t dma_remaining,                   /* DMA 剩余未接收字节数。 */
    uint16_t received_length,                 /* 本次 IDLE 中断计算的响应长度。 */
    const uint8_t *dma_buffer,                /* USART2 DMA 直接写入的原始缓冲区。 */
    const uint8_t *receive_buffer);           /* ISR memcpy 后供任务使用的接收缓冲区。 */
#endif
#if DCDC_MODBUS_PC_TEST_ENABLE
extern volatile dcdc_modbus_pc_test_t g_dcdc_modbus_pc_test; /* 导出 PC 模拟触发器，供 Keil Watch 修改。 */
#endif

void modbus_init(void);                       /* 初始化 Modbus RTU 主站任务。 */
const volatile acdc_modbus_data_t *acdc_modbus_get_data(void); /* 获取 ACDC 数据镜像的只读指针。 */
const volatile dcdc_modbus_data_t *dcdc_modbus_get_data(void); /* 获取 DCDC 数据镜像的只读指针。 */
const volatile dcdc_modbus_write_status_t *dcdc_modbus_get_write_status(void); /* 获取最近写命令状态。 */
void modbus_copy_gui_snapshot(            /* 在短临界区内复制界面需要的三份状态，避免任务切换造成撕裂读取。 */
    acdc_modbus_data_t *acdc_data,
    dcdc_modbus_data_t *dcdc_data,
    dcdc_modbus_write_status_t *write_status);
uint32_t dcdc_modbus_get_data_age_ms(void); /* 获取最近一轮完整 DCDC 数据距当前的毫秒数。 */
uint8_t dcdc_modbus_data_is_fresh(void); /* 判断 DCDC 在线且最近完整数据未超过新鲜度阈值。 */
uint8_t dcdc_modbus_is_running(void); /* 数据新鲜、无故障且 0x0404 为 1 时返回 1。 */
uint8_t dcdc_modbus_is_stopped(void); /* 数据新鲜、无故障且 0x0404 为 4 时返回 1。 */
uint8_t dcdc_modbus_is_faulted(void); /* 数据新鲜且 0x0405 任一故障位置位时返回 1。 */
int dcdc_modbus_write_single_async(uint16_t address, uint16_t value); /* 异步提交一条功能码 0x06 单写命令。 */
int dcdc_modbus_write_multiple_async(uint16_t address, const uint16_t *values, uint16_t count); /* 异步提交功能码 0x10 多写命令。 */
int dcdc_modbus_set_work_mode_async(uint16_t mode); /* 异步设置 DCDC 工作模式，内部写 0x0402。 */
int dcdc_modbus_set_run_async(uint8_t enable); /* 异步启停 DCDC，1 启动、0 安全停机。 */

#endif                                       /* __MODBUS_H_ */
