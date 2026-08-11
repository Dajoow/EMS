# Hdu_EMS_H750 项目上下文

> 最近核对：2026-08-11（Asia/Shanghai）  
> 工程根目录：本文所在目录  
> 事实标记：**[已验证]** 表示可由当前源码、Git、已有测试记录或本对话截图确认；**[代码推断]** 表示由当前代码路径推导、尚未做现场验证；**[待确认]** 表示当前材料不足。  
> 维护规则：接手者必须先核对实际工程和 `git diff`。本文是恢复上下文的入口，不是源码与硬件实测的替代品。

## 1. 项目概述

### 1.1 项目目标

- **[已验证]** 工程名为 `BSMU_H750IB`，目标 MCU 为 STM32H750IBTx（Cortex-M7）。
- **[已验证]** 系统承担电池簇数据采集/聚合、TouchGFX 本地显示、本地 TCP 与 HTTPS 上报、参数持久化等 EMS/BSMU 功能。
- **[已验证]** 当前拓扑已经收敛为 `1 BSMU -> 1 BCMU -> 2 BMU`，对应 `cluster_num=1`、`GRP_num=2`、每组 12 节电池。
- **[已验证]** 当前重点是把板卡现有的一路二线 RS485 用作 Modbus RTU 主站总线，同时访问 ACDC 和双向 DCDC 两个从站。

### 1.2 当前开发阶段

- **[已验证]** ACDC+DCDC 双从站主站轮询框架已经写入工作区，但尚未提交。
- **[已验证]** PC 端用 Witte Software Modbus Slave + USB-RS485 完成了正常读通信、DCDC `0x06`/`0x10` 写通信及写后 `0x03` 读回模拟测试。
- **[已验证]** 双从站正常稳定性测试通过：ACDC（ID 33）与 DCDC（ID 1）均完成 5685 轮完整轮询，错误计数为 0，未观察到串站。
- **[已验证]** ACDC/DCDC两个方向的持续断线隔离与无需复位自动恢复测试已通过，正式记录见V1.5测试指导书。
- **[已验证]** ACDC/DCDC两个方向的CRC错误识别、异常隔离与无需复位自动恢复测试已通过，正式记录见V1.6测试指导书。
- **[已验证]** ACDC/DCDC两个方向的Exception 06异常响应码识别、隔离与无需复位自动恢复测试已通过，正式记录见V1.7测试指导书。
- **[已验证]** DCDC `Skip response` 期间曾出现电池页面异常值；该问题仍未定位，继续作为独立问题挂起。
- **[已验证]** 真实DCDC（地址`0xFF`）只读联调已经通过四级接收缓冲诊断和PC侧被动RS485抓帧确认：EMS内部ISR/任务/解析缓冲复制一致；PC在DCDC端看到的2080帧完整响应CRC全部正确，但EMS仍偶发收到CRC尾字节错误，问题范围已缩小到RS485物理链路、EMS侧收发器或USART采样链路。当前缺少逻辑分析仪，问题作为已知风险暂缓，不视为已解决。
- **[已验证]** 2026-08-11 已完成PC作为唯一主站、USB-RS485直连真实DCDC的两组10分钟Modbus Poll对照：`0x0404/11`为`Tx=1205、Err=15`，`0x041B/6`为`Tx=1223、Err=8`。两组日志中的核心Modbus响应帧CRC均正确，异常主要表现为完整有效帧之后偶发附加单字节以及少量无响应；测试到此暂停，根因仍保留为待硬件仪器定位事项。
- **[已验证]** 2026-07-22 已对当前工作区执行完整 Keil Rebuild：0 errors、10 warnings，成功链接并生成 `.axf`、`.hex`、`.map`。

### 1.3 最终需要实现的功能

- 同一条二线 RS485 上由 EMS 作为唯一 Modbus RTU 主站，可靠轮询 ACDC（地址 33）和真实 DCDC（当前实机地址 `0xFF`；此前 PC 模拟使用地址 1）。
- 将两个设备的寄存器数据按协议缩放、符号和字节序转换成独立工程量结构。
- 提供 DCDC 安全的异步写命令框架，包含白名单、单/多寄存器写、写后读回、状态与错误统计。
- 具备单从站断线隔离、自动恢复、CRC/异常响应处理，不污染 CAN/电池数据和 UI。
- 完成真实 ACDC、真实 DCDC 的只读联调；在互锁和范围校验完成后再开放真实 DCDC 控制。

### 1.4 明确不在当前范围内

- **[已验证]** 不因 Modbus 改造重画板或新增第二路 RS485；现阶段复用现有一路二线接口。
- **[已验证]** 当前工程没有 NetX Duo；网络栈是 LwIP。
- **[已验证]** 当前源码未发现 MQTT/MQTTS 客户端、Broker、主题或客户端标识配置；云端通道是 HTTPS。除非后续需求变更，不把 MQTT 视为现有功能。
- **[已验证]** 本阶段不执行真实 DCDC 启停/充放电控制，也不在异常根因未解决时将 PC 测试入口用于真实设备。
- **[待确认]** ACDC 写控制是否属于最终范围；目前实现和测试重点是 ACDC 只读。

## 2. 开发环境

| 项目 | 当前信息 | 状态/来源 |
|---|---|---|
| MCU | STM32H750IBTx，LQFP176，Cortex-M7 | **[已验证]** `BSMU_H750IB.ioc`、Keil 工程 |
| 工程/板卡名 | BSMU_H750IB / STM32H750 EMS 板卡 | **[已验证]** 工程与测试文档；板卡硬件版本待确认 |
| 主频 | CPU/SYSCLK 480 MHz，HCLK 240 MHz，APB 120 MHz | **[已验证]** `.ioc` 与 `SystemClock_Config()` |
| Ethernet | RMII；CubeMX 配置和驱动使用 LAN8742 | **[已验证]** `.ioc`、`Drivers/BSP/Components/lan8742/` |
| PHY 名称差异 | `README.md` 历史记录称 LAN8720，当前 `.ioc` 称 LAN8742 | **[待确认]** 以实物丝印/原理图最终确认 |
| RS485 | USART2，二线、半双工物理总线；8N1，无流控 | **[已验证]** 源码、既有硬件检查和 PC 测试 |
| RS485 收发器型号 | 未在当前源码中明确 | **[待确认]** 需查原理图/BOM/实物 |
| CAN | FDCAN2，用于 BCMU/BMU 数据链路 | **[已验证]** `main.c`、`CAN_Control.c` |
| 触摸 | GT911，软件 I2C | **[已验证]** `README.md`、`Drivers/BSP/Components/GT911/` |
| 外部 SDRAM | W9825G6KH 驱动，FMC | **[已验证]** BSP 目录与初始化代码 |
| EEPROM | AT24C02/AT24Cxx，存放 `bsmuSetting` | **[已验证]** `main.c`、AT24CXX 驱动 |
| QSPI Flash | README 记录 V1.0 为 W25Q64FV；代码还使用 SFUD/EasyFlash | **[待确认]** 当前实物型号及 DATA_FLASH 型号 |
| 4G 模块 | 代码 GPIO 名称和驱动指向 EC600 | **[代码推断]** 具体料号/固件版本待确认 |
| IDE | Keil MDK-ARM，CubeMX 目标工具链 `MDK-ARM V5.32` | **[已验证]** `.ioc` |
| 编译器 | ARM Compiler V5.06 update 7 build 960（未使用 AC6） | **[已验证]** `.uvprojx` |
| STM32CubeMX | 6.9.0 | **[已验证]** `.ioc` |
| Device Pack | Keil STM32H7xx DFP 2.7.0；CMSIS pack 5.9.0 | **[已验证]** `.uvprojx` |
| RTOS | FreeRTOS V10.3.1 + CMSIS-RTOS v1 API | **[已验证]** 中间件头文件、任务创建方式 |
| 网络栈 | LwIP 2.1.2 Cube | **[已验证]** `.ioc` |
| TLS | mbedTLS 2.16.2 | **[已验证]** `version.h` |
| GUI | TouchGFX 4.21.4 路径曾用于生成/模拟器 | **[已验证]** `项目记忆.md`；当前本机安装有效性待确认 |
| Modbus | Agile Modbus（仓内源码） | **[已验证]** `agile_modbus/`；精确版本待确认 |
| 持久化库 | EasyFlash + SFUD（仓内源码） | **[已验证]** `flash_storage/`；精确版本待确认 |

## 3. 系统架构

### 3.1 软件模块划分

| 模块 | 主要职责 |
|---|---|
| 启动与硬件 | `Core/Src/main.c`：Cache、MPU、时钟、EEPROM 参数、外设、TLS、TouchGFX、RTOS 启动 |
| RTOS 编排 | `Core/Src/freertos.c`：启动任务、业务模块创建、运行资源监控 |
| CAN/电池采集 | `Core/Src/CAN_Control.c`：FDCAN 收发、轮询、队列、上报信号量 |
| 数据聚合 | `Core/Src/station_ctl.c`：簇/站级数据结构和统计计算 |
| EMS 策略 | `Core/Inc/EMS_strategy.h`、`Core/Src/EMS_strategy.c`：未跟踪的新模块；已参与当前 Keil 构建，职责和功能验证仍待确认 |
| Modbus 主站 | `Drivers/BSP/Components/RS485/modbus.c/.h`：ACDC/DCDC 轮询、状态、DCDC 写队列和 PC 触发 |
| RS485 接收 | `Core/Src/usart.c`、`RS485.*`、`modbus_slave.*`：USART2 DMA/IDLE 接收与任务通知 |
| 本地网络 | `Core/Src/client.c`：从 EEPROM 读取目标 IP/端口并建立本地 TCP 客户端 |
| 云端网络 | `Core/Src/http_client.c`：DNS、TLS、HTTPS JSON 上报 |
| 时间同步 | `Core/Src/sntp_client.c`：SNTP 初始化和时间接口 |
| 持久化 | `Core/Src/data_persistence.c`、`flash_storage/`：EasyFlash/SFUD 数据保存 |
| GUI | `TouchGFX/`：界面生成代码、Model 与手工 View 逻辑 |

### 3.2 任务/线程及职责

| 任务 | 优先级 | 栈深度（代码参数） | 职责 |
|---|---:|---:|---|
| `startup_Thread` / `Startup()` | Realtime | 256 | 初始化业务模块，随后自删除 |
| `TouchGFX` / `TouchGFX_Task()` | BelowNormal | 3072 | GUI 主任务 |
| `CPU_Task_Thread` / `CPU_Task()` | Idle | 256 | 打印堆、任务栈水位和 CAN 队列状态 |
| `CAN_Rev_Thread` / `CAN_Rev()` | High | 128 | CAN 接收与组帧 |
| `CAN_Poll_Thread` / `CAN_Poll()` | Normal | 128 | BCMU/BMU 轮询 |
| `switch_ctrl_task` / `switch_task()` | Normal | 128 | 开关控制 |
| `modbus_task` | Normal | 512 | 唯一 RS485 总线拥有者；ACDC/DCDC 读写事务串行化 |
| `Client_thread` / `Client()` | Normal | 512 | 本地 TCP 连接与发送；连接后创建接收线程 |
| `TCP_Recv_thread` / `TCP_Recv()` | Normal | 512 | 本地 TCP 接收 |
| `http_client` / `httpc_task()` | Normal | 2048 | HTTPS 连接与上报 |
| `EthIf` / `ethernetif_input()` | Realtime | 配置宏 | Ethernet 输入 |
| `EthLink` / `ethernet_link_thread()` | BelowNormal | `INTERFACE_THREAD_STACK_SIZE` | 链路检测、DHCP 重启 |
| `Module4G_Thread` | Normal | 256 | 4G 模块处理 |
| `flash_save_task` | Normal | 256 | 定期保存累计数据 |

### 3.3 调用关系与关键数据流

1. **[已验证]** `main()` 设置向量表，开启 I/D Cache，初始化 HAL/MPU/时钟，从 AT24Cxx 读出 `bsmuSetting`，初始化外设、mbedTLS、TouchGFX 和 FreeRTOS。
2. **[已验证]** `Startup()` 调用 `StationDataInit()`、持久化、SNTP、CAN、4G、屏幕休眠和 `modbus_init()`；按 `bsmuSetting.local_flag`/`yunduan_flag` 决定是否启动本地 TCP/HTTPS。
3. **[已验证]** CAN 接收数据写入 `Client_Sd[]` 等结构；站级逻辑聚合后由 TouchGFX、本地 TCP 和 HTTPS 消费。
4. **[已验证]** Modbus 任务独占 USART2：依次轮询 ACDC 四块、等待 RTU 帧间隔、轮询 DCDC 两块、处理最多一条 DCDC 写命令，再等待下一轮。
5. **[已验证]** USART2 DMA + IDLE 中断计算响应长度，通过任务通知唤醒当前 Modbus 事务；Agile Modbus 校验从站地址、功能码、长度和 CRC。
6. **[已验证]** DCDC 写请求先复制进长度 1 的队列；任务根据数量选择 `0x06` 或 `0x10`，成功后用 `0x03` 读取同一区域逐项校验。
7. **[已验证]** Modbus 的 `g_acdc_modbus_data`/`g_dcdc_modbus_data` 与 CAN 的 `Client_Sd[]` 是不同数据结构。设计上 Modbus 超时不应改写电池页面数据；现有异常说明可能存在其他内存/并发问题。

### 3.4 网络连接流程

1. `MX_LWIP_Init()` 初始化 TCP/IP，IPv4 初值为 0，添加 `gnetif`，启动 Ethernet 输入和链路线程。
2. `LWIP_DHCP=1`，接口启动后调用 `dhcp_start()`；DNS 和 SNTP 已开启。
3. 本地 TCP：`Client()` 使用 EEPROM 中 `bsmuSetting.IP_ADD_1[]` 和 `bsmuSetting.port` 连接。
4. HTTPS：`httpc_task()` 等待 DHCP 地址，解析主机名，TCP 连接 443，加载 CA，配置 TLS/SNI 并握手，然后发送站级/簇级 HTTP 请求。
5. **[已验证]** 当前 TLS 认证模式为 `MBEDTLS_SSL_VERIFY_OPTIONAL`，且证书验证调用路径存在被注释的代码；上线前必须复核。

## 4. 关键配置

### 4.1 时钟、内存、MPU、Cache

- CPU/SYSCLK 480 MHz，HCLK/AXI/AHB 240 MHz，APB1~4 120 MHz；HSE 25 MHz，PLL 为系统时钟源。
- `main()` 开启 I-Cache 和 D-Cache。
- MPU 区域：
  - `0x30040000`，32 KB，不可执行、不可缓存/缓冲，用于 Ethernet 描述符/缓冲相关区域；另有 256 B 子区域配置。
  - `0x30020000`，32 KB，不可执行、不可缓存，用作 LwIP heap。
  - `0xC0000000`，4 MB，外部 SDRAM 区。
- 固定地址：`ETHSendBuffAdd=0xC0384000`、`CANRevBuffStartAdd=0xC03B0000`、`CANSndBuffStartAdd=0xC03F8000`；云端缓存还使用 `0xC0400000` 和 `0xC040E6A0`。
- **注意：** 固定地址、链接段、MPU 与 Cache/DMA 必须成套核对，不得单独移动。

### 4.2 Ethernet、LAN8742、LwIP

- RMII；RX buffer 1536；TX descriptor `0x30040060`，RX descriptor `0x30040000`。
- LwIP 2.1.2 Cube；`MEM_SIZE=32*1024`，heap 指针 `0x30020000`，`TCP_MSS=1024`，`TCP_WND=5896`，DNS/DHCP/SNTP 开启。
- Ethernet 由 `ethernetif.c` + LAN8742 BSP 驱动；`README.md` 记录 CubeMX 重新生成后需复核热插拔处的 `HAL_ETH_Start_IT()`。
- **[待确认]** 当前 PHY 实物究竟是 LAN8720 还是 LAN8742。

### 4.3 IP、DNS、DHCP

- DHCP：开启，当前无固定 IP 配置。
- DNS：开启；HTTPS 使用域名解析。
- 本地服务器地址/端口：保存在 AT24Cxx 的 `bsmuSetting.IP_ADD_1[]`、`bsmuSetting.port`。
- 4G 地址/端口：`bsmuSetting.IP_ADD_2[]`、`bsmuSetting.port_1`。
- DNS 服务器具体来源/静态覆盖：**[待确认]**。

### 4.4 HTTPS/TLS 与敏感配置

- HTTPS 服务器、端口和 API 路径宏位于 `Core/Src/http_client.c`：`HOST`、`HTTPS_PORT`、`API_STATISTICS`、`API_CLUSTERS`。
- 客户端鉴权值宏位于同文件：`SERVER_TOKEN`。**本文不记录其值。** 当前源码含明文敏感值，应迁移到安全配置。
- CA 证书位于 `Core/Inc/ca_certificates.h`，通过 `__ssl_ca_certificate` 与 `__ssl_ca_certificate_len` 传给 `mbedtls_x509_crt_parse()`。**本文不复制证书内容。**
- TLS 上下文变量包括 `server_fd`、`ssl`、`conf`、`cert`、`ctr_drbg`、`entropy`。
- 当前验证策略为 `MBEDTLS_SSL_VERIFY_OPTIONAL`；上线目标应评估改为必选验证，并确保 `mbedtls_ssl_get_verify_result()` 的结果真正参与连接判定。
- MQTT/MQTTS、Broker、topic、client ID：**[已验证] 当前源码未发现，暂无配置。**

### 4.5 Modbus RTU

| 项目 | 当前值 |
|---|---|
| 总线 | USART2，同一条二线 RS485，EMS 唯一主站 |
| 串口 | 8N1、无流控；波特率由 `bsmuSetting.RS485Bps` 选择，测试常用 9600 |
| ACDC 地址 | 33 / `0x21` |
| DCDC 地址 | 真实设备当前确认为 `0xFF`；此前 PC Modbus Slave 模拟使用 1 / `0x01` |
| 响应超时 | 500 ms |
| RTU 帧间隔 | 5 ms |
| 一轮后的延时 | 500 ms |
| 离线门限 | 连续失败 3 次 |
| DCDC 写队列 | 长度 1；最多连续 14 个寄存器 |
| PC Watch 触发 | 当前 `DCDC_MODBUS_PC_TEST_ENABLE=0`，真实设备阶段已关闭；历史模拟使用魔术值宏 `DCDC_MODBUS_PC_TEST_MAGIC` |

**安全要求：真实 DCDC 接入前，必须将 `DCDC_MODBUS_PC_TEST_ENABLE` 改为 0、重新编译，并确认没有调试器残留写操作。**

## 5. 关键文件说明

| 相对路径 | 类型 | 职责与关键入口 | CubeMX/生成风险 |
|---|---|---|---|
| `BSMU_H750IB.ioc` | 配置源 | MCU、时钟、外设、FreeRTOS、LwIP | CubeMX 配置主源；修改后会重生成大量文件 |
| `backup_BSMU_H750IB.ioc` | 备份 | 历史配置 | 不应替代当前 `.ioc`，用途待确认 |
| `MDK-ARM/BSMU_H750IB.uvprojx` | 工程配置 | Keil target、文件组、编译器 | 当前有未提交修改；CubeMX/Keil 都可能改写 |
| `Core/Src/main.c` | 混合 | `main()`、`SystemClock_Config()`、`MPU_Config()` | 仅 `USER CODE` 块通常受保护；块外手改可能覆盖 |
| `Core/Src/freertos.c` | 混合 | `MX_FREERTOS_Init()`、`Startup()`、`CPU_Task()` | CubeMX 可重生成；启动顺序手工代码须复核 |
| `Core/Src/usart.c`、`Core/Inc/usart.h` | 混合 | USART2 波特率、DMA/IDLE、任务通知 | CubeMX 可覆盖块外修改；当前均有用户差异 |
| `Core/Src/CAN_Control.c` | 手工业务 | `BSMU_CANInit()`、CAN 接收/轮询/开关任务 | 保留拓扑适配和用户修改 |
| `Core/Src/station_ctl.c`、`Core/Inc/station_ctl.h` | 手工业务 | `StationDataInit()`、`CalStationData()`；拓扑宏和数据结构 | 固定布局/数组尺寸敏感 |
| `Core/Src/EMS_strategy.c`、`Core/Inc/EMS_strategy.h` | 未跟踪手工文件 | EMS 策略模块 | **待确认** 是否已接入构建、功能是否完成 |
| `Drivers/BSP/Components/RS485/modbus.c/.h` | 手工业务 | `modbus_init()`、`modbus_task()`、ACDC/DCDC poll、DCDC 写 API | 当前 Modbus 核心，未提交且大幅修改 |
| `Drivers/BSP/Components/RS485/modbus_data.c` | 手工业务 | 传统 Modbus 数据映射/拓扑数组 | 当前有未提交修改；与新主站结构区别需保持清晰 |
| `Drivers/BSP/Components/RS485/modbus_slave.c` | 手工/旧组件 | USART2 接收复位/旧从站支持 | 当前有修改；实际职责命名容易误解 |
| `agile_modbus/` | 第三方 | RTU 序列化/反序列化 | 原则上不改库，优先在适配层处理 |
| `LWIP/App/lwip.c`、`LWIP/Target/ethernetif.c` | 生成+手工补丁 | LwIP/DHCP/链路与收包线程 | CubeMX 后复核 Ethernet 热插拔补丁 |
| `LWIP/Target/lwipopts.h` | 生成配置 | LwIP 内存、线程、DNS/DHCP/SNTP | 当前有未提交修改；CubeMX 可覆盖 |
| `Core/Src/client.c` | 手工业务 | `ClientInit()`、`Client()`、`TCP_Recv()` | 本地 TCP 参数来自 EEPROM |
| `Core/Src/http_client.c` | 手工业务 | `http_client_init()`、`httpc_task()`、TLS/HTTP | 含敏感宏；当前有大量未提交修改 |
| `Core/Inc/ca_certificates.h` | 安全配置 | CA 证书变量 | 禁止复制到日志/上下文；更新需核验证书链 |
| `Core/Src/sntp_client.c` | 手工业务 | `sntp_client_init()`、`mbedtls_get_time()` | TLS 时间依赖 |
| `Core/Src/data_persistence.c`、`flash_storage/` | 手工+第三方 | EasyFlash/SFUD 持久化 | Flash 型号/分区和锁不可随意改 |
| `TouchGFX/generated/` | 自动生成 | GUI 字体、文本、图片、基类 | 重新生成会覆盖；当前已有大量生成差异 |
| `TouchGFX/gui/` | 手工 GUI | View/Model 用户逻辑 | Designer 重生成后仍需编译和实机复核 |
| `docs/EMS_Modbus双从站读写模拟测试指导书_V1.5.docx` | 测试基线 | 最新 PC 双从站读写、稳定性、持续断线隔离与自动恢复记录 | 后续新增结果应继续版本化维护；V1.4保留为上一版 |
| `docs/EMS_Modbus双从站读写模拟测试指导书_V1.6.docx` | 最新测试基线 | 在V1.5基础上追加ACDC/DCDC双向CRC错误识别、隔离与恢复记录 | 后续新增结果应继续版本化维护；V1.5保留为上一版 |
| `docs/EMS_Modbus双从站读写模拟测试指导书_V1.7.docx` | 最新测试基线 | 在V1.6基础上追加ACDC/DCDC双向Exception 06异常响应码识别、隔离与恢复记录 | 后续新增结果应继续版本化维护；V1.6保留为上一版 |
| `项目记忆.md` | 历史上下文 | 旧阶段工程、GUI 与硬件记录 | 保留；与本文冲突时以当前源码/Git/最新实测为准 |
| `README.md` | 历史说明 | 硬件注意和 CubeMX 后修复清单 | 部分型号/结论可能过时，须交叉确认 |

## 6. 已完成工作

### 6.1 已实现/修改

- **[已验证]** 拓扑从历史多簇改为 1 BCMU、2 BMU；CAN、数据结构、HTTP、TouchGFX 均有相关适配，最近两个提交已保存主要拓扑/UI工作。
- **[已验证]** `modbus.c/.h` 已从旧逻辑扩展为 EMS RTU 主站：
  - ACDC ID 33 读取 `0x1000/13`、`0x1022/2`、`0x1100/4`、`0x5002/12`。
  - DCDC ID 1 读取 `0x0404/11`、`0x041B/6`。
  - 独立数据结构、在线/连续失败/最后错误/成功/超时/协议错误统计。
  - DCDC 异步写队列、地址白名单、`0x06`/`0x10`、写后 `0x03` 读回校验。
  - 各 RTU 事务间加入 5 ms 帧间隔。
- **[已验证]** USART2 接收完成使用任务通知唤醒 Modbus 任务；任务独占总线。
- **[已验证]** 最新测试指导书已维护到 V1.7；V1.6记录CRC双向异常隔离，V1.7记录Exception 06双向异常隔离。

### 6.2 已验证结果

| 测试 | 结果 | 证据/备注 |
|---|---|---|
| DCDC `0x03` 两块读取及缩放 | 通过 | 温度、功率、电压、电流与模拟值一致 |
| DCDC `0x06` 单写 + `0x03` 读回 | 通过 | `0x0402=1`，requested/readback 一致 |
| DCDC `0x10` 双寄存器写 + 读回 | 通过 | `0x0427=480`、`0x0428=120` 一致 |
| 写队列/状态 | 通过 | `submit_result=0`、成功状态和 sequence 可观察 |
| ACDC 四块读取及缩放 | 通过 | 模拟工程量正确 |
| ACDC+DCDC 同总线区分 | 通过 | 地址 33/1，无串站 |
| 双从站稳定运行 | 通过 | 双方 `success_count=5685`，在线且错误计数 0 |
| 复位后正常清零/恢复 | DCDC 正常场景通过 | 预先连接模拟从站，约 10 秒得到理想状态 |
| Modbus 栈/堆余量 | 通过当前观察 | 空闲堆 263216，历史最低 261488，Modbus 最小剩余栈 399/512 |
| ACDC持续失联→DCDC隔离→ACDC恢复 | 通过 | ACDC `online=0`、`last_error=-2`；DCDC持续在线且错误计数为0；ACDC无需复位恢复 |
| DCDC持续失联→ACDC隔离→DCDC恢复 | 通过 | DCDC `online=0`、`last_error=-2`；干净复测中ACDC错误计数不变；DCDC无需复位恢复 |
| ACDC CRC错误→DCDC隔离→ACDC恢复 | 通过 | ACDC `protocol_error_count: 0x0C→0x11`、`timeout_count=0`；DCDC全程在线且错误计数为0；取消注入后协议错误停止增加 |
| DCDC CRC错误→ACDC隔离→DCDC恢复 | 通过 | DCDC `protocol_error_count: 0→0x07`、`timeout_count=0`；ACDC协议错误历史值保持0x11；取消注入后DCDC错误停止增加 |
| ACDC Exception 06→DCDC隔离→ACDC恢复 | 通过 | ACDC曾进入`online=0`、`last_error=-3`，`protocol_error_count`最终稳定在0x75；DCDC错误计数不变；取消异常后无需复位恢复 |
| DCDC Exception 06→ACDC隔离→DCDC恢复 | 通过 | DCDC `protocol_error_count: 0x07→0x0F`、`timeout_count=0`；ACDC错误计数保持0x75；取消异常后DCDC成功计数继续增加 |

### 6.3 已知可正常工作的版本状态

- **[已验证]** 当前已知稳定的 Modbus 功能存在于未提交工作区，不对应一个可唯一复现的 Git commit。
- **[已验证]** Git `HEAD=826fbff` 只代表 2026-06-04 的拓扑/UI提交；不能单独代表当前 Modbus 功能。
- **[已验证]** 2026-07-22 使用 Keil ARM Compiler V5.06 update 7 build 960 对当前未提交工作区完成 Rebuild：0 errors、10 warnings；构建产物时间为本次构建时间。

## 7. 当前问题

### 7.1 DCDC `Skip response` 时电池页面异常（挂起）

- **现象 [已验证]：** 勾选 PC 从站 `Skip response` 后，电池界面出现 8.744 V、34.816 V、819.2%、-245.76 ℃、102.74 ℃等明显越界值。
- **复现：** PC Modbus Slave 模拟 DCDC，在目标窗口勾选 `Skip response`，观察通信错误和 TouchGFX 电池页面。
- **已排查 [已验证]：** Modbus 超时路径能触发；堆空间充足；Modbus 任务历史最小剩余栈 399/512，因此目前不支持“Modbus 任务栈溢出”判断。
- **代码关系 [已验证]：** 页面读取 `Client_Sd[]`，不是 `g_dcdc_modbus_data`；按设计，Modbus 超时不应修改电池数据。
- **可能原因 [待确认]：** CAN 接收长度/数组边界、底层数组被覆盖、CAN 写与 TouchGFX 读的并发一致性、UI 缺少在线/范围保护。
- **尚未验证：** 同时监视 `Client_Sd[0].BAT_VOL/SOC/TMP`；至少 5 轮异常/恢复复现；定位首次发生的写入点。
- **当前决策 [已验证]：** Deferred，不在本阶段盲改；真实 DCDC 控制开放前必须闭环。

### 7.2 异常隔离与恢复测试进度

- **[已验证]** Modbus Slave的 `Skip response` 在本次环境中表现为约每10次请求丢1次，适合间歇丢包测试，不适合制造连续3次失败；持续失联改用临时修改全部目标从站窗口ID的方法。
- **[已验证]** ACDC持续失联时DCDC保持在线且错误计数为0；ACDC恢复ID 33后无需复位自动恢复。
- **[已验证]** DCDC持续失联时ACDC保持在线；清除ACDC1残留的 `Skip response` 后干净复测，ACDC的 `timeout_count=0xBC`、`protocol_error_count=0x72`保持不变；DCDC恢复ID 1后无需复位自动恢复。
- **[已验证]** ACDC CRC注入后`protocol_error_count`由0x0C增至0x11，`timeout_count`保持0；停止注入后协议错误停止增加，DCDC全程在线且错误计数为0。
- **[已验证]** DCDC CRC注入60秒后`protocol_error_count=0x07`，`timeout_count=0`；停止注入再运行60秒后保持0x07，ACDC全程在线且历史协议错误计数保持0x11。
- **[已验证]** ACDC返回Exception 06时，`last_error=-3`、`protocol_error_count`增加、`timeout_count=0`，DCDC全程正常；取消异常后ACDC无需复位恢复，协议错误稳定在0x75。
- **[已验证]** DCDC返回Exception 06时，`protocol_error_count`由0x07增至0x0F、`timeout_count=0`，ACDC全程正常；取消异常后DCDC协议错误停止增加且成功计数继续增加。
- **[待完成]** 至少5轮异常注入重复性和恢复时间统计尚未执行。
- **[已解决的误判]** 曾以为部分窗口上电后没有 Rx，后来确认是同时打开两个相同配置窗口且被遮挡，不是 EMS 通信故障。

### 7.3 2026-07-28测试日志与工具注意事项

- **今日完成 [已验证]：**
  - 完成ACDC和DCDC两个方向的CRC/LRC错误注入、另一从站隔离和取消注入后的自动恢复测试；结果归档于`docs/EMS_Modbus双从站读写模拟测试指导书_V1.6.docx`。
  - 完成ACDC和DCDC两个方向的Exception 06（Slave Device Busy）异常响应、另一从站隔离和取消异常后的自动恢复测试；结果归档于`docs/EMS_Modbus双从站读写模拟测试指导书_V1.7.docx`。
  - 新增可重复生成文档的脚本`tools/append_crc_isolation_v16.py`和`tools/append_exception_isolation_v17.py`。
- **错误分类 [已验证]：** CRC错误和Exception 06均增加`protocol_error_count`，测试期间`timeout_count`保持不变；说明当前固件没有把收到的错误帧/异常响应误归类为无响应超时。
- **历史计数规则 [已验证]：** `success_count`、`timeout_count`和`protocol_error_count`为累计统计，恢复通信后不会自动清零；恢复验收应观察`online=1`、`consecutive_failures=0`、`last_error=0`、成功计数继续增加以及错误计数停止增加。
- **ACDC Exception 06过渡现象 [已验证]：** 取消异常后`protocol_error_count`由0x71短暂增加到0x75，继续运行30秒保持0x75；判断为在途/过渡请求，不是持续故障。同期ACDC与DCDC成功计数各增加68。
- **注入行为差异 [已验证]：**
  - ACDC的Exception 06测试中形成连续失败，ACDC进入`online=0`，`consecutive_failures=0x60`、`last_error=-3`；取消后自动恢复。
  - DCDC的Exception 06测试呈间歇异常，正常响应会把`consecutive_failures`清零，因此DCDC保持`online=1`；这不影响异常识别与隔离测试判定。
  - CRC/LRC错误注入同样可能是间歇性的，不能仅以是否离线判断通过；应以`protocol_error_count`增量、`timeout_count`不变、另一从站正常及取消注入后停止增长为准。
- **Modbus Slave配置风险 [已验证]：**
  - 每次测试前逐个检查全部窗口，确保非目标窗口的`Skip response`、`Insert CRC/LRC error`和`Return exception`全部取消；此前ACDC1残留`Skip response`曾污染DCDC断线隔离测试。
  - 只对周期读取窗口注入异常：ACDC使用ID 33、地址4096、数量13；DCDC使用ID 1、地址1028、数量11。地址1026/1063窗口主要用于控制/参数读写，周期轮询时Rx可能为0。
  - 修改Slave Definition会使该窗口Rx重新计数；Rx表示收到的请求数量，不等于成功响应数量。
- **文档QA限制 [已验证]：** V1.6和V1.7均通过python-docx结构检查，可正常解析且章节、表格和图片存在；当前环境缺少LibreOffice/soffice，未完成DOCX页面渲染视觉检查，不能宣称通过最终视觉QA。
- **今日未解决问题：** DCDC `Skip response`期间电池页面/`Client_Sd[]`异常值问题仍挂起，今天的CRC与Exception 06测试未提供其根因证据；后续重复测试需要同步Watch底层电池数组。

### 7.4 2026-08-06真实DCDC只读联调与Cache诊断

- **实机条件 [已验证]：** 当前仅轮询真实DCDC，从站地址为`0xFF`；ACDC轮询、DCDC写命令处理和PC Watch写触发均已关闭，EMS只发送功能码`0x03`。
- **读取内容 [已验证]：** 每轮读取`0x0404/11`和`0x041B/6`两个保持寄存器块；DCDC处于待机，`work_state=0`、`fault_raw=0`，B侧电压约47.8 V，实机数据能够持续更新。
- **D-Cache开启基线 [已验证]：** 连续运行约626秒，仅末尾Stop一次；`success_count=825`、`timeout_count=18`、`protocol_error_count=199`，完整轮询成功率约79.2%，失败率约20.8%。该结果排除了反复Stop/Run是主要原因。
- **临时关闭D-Cache对比 [已验证]：** 注释`Core/Src/main.c`中的`SCB_EnableDCache()`并重新编译下载，连续运行约676秒；`success_count=1047`、`timeout_count=29`、`protocol_error_count=34`，成功率约94.3%。协议错误率由约19.1%降到约3.1%，强烈支持USART2 DMA缓冲与D-Cache一致性是主要问题，但仍有约5.7%失败需要继续分类。
- **当前Cache状态 [已验证]：** `SCB_EnableDCache()`仅为诊断而临时注释；这不是正式修复，完成收帧诊断后必须恢复D-Cache并采用局部非缓存MPU区或正确Cache维护方案。
- **详细诊断代码 [已实现、已编译、待实测]：** `modbus.h/.c`新增`MODBUS_RX_DIAG_ENABLE=1`和`g_modbus_rx_diag`，仅在失败时记录状态块/实时块分类、失败阶段、期望长度、实际长度、解析结果及前32字节原始响应；设为0时通过预处理完全移除详细诊断代码和变量。
- **异常原始帧 [已验证]：** 诊断抓到一帧完整的`0x0404/11`响应：帧头`FF 03 16`、总长度27字节、数据区可正确还原当前工作状态/温度，但帧尾实际为`E7 94`；按工程相同Modbus CRC算法计算应为`E7 29`，确认该次失败是最后一个CRC字节损坏，而不是地址、功能码、字节数或DMA长度错误。
- **最小物理层/复制诊断 [已实现、已编译、已实测]：** 在原有开关`MODBUS_RX_DIAG_ENABLE`内增加USART2中断入口ISR快照、HAL错误码、DMA余量、ISR长度、DMA/ISR接收缓冲/任务接收缓冲/解析缓冲四级帧尾CRC、计算CRC、接收CRC、首个复制差异以及PE/NE/FE/ORE累计计数。`Core/Src/stm32h7xx_it.c`只在现有IDLE接收路径执行轻量采样，不改变DMA启停、任务通知和Modbus判定；开关设为0时中断采样调用一并移除。
- **四级缓冲诊断结果 [已验证]：** 最近捕获的异常事务为从站`0xFF`读取`0x0404/11`，`expected_length=received_length=isr_received_length=0x001B`，`dma_remaining=0x03E5`；异常原始帧为`FF 03 16 00 00 00 00 FD DD 00 F3 FD D1 01 0B FD CD 00 F2 01 13 01 0F 01 13 8A EA`。DMA原始缓冲、ISR接收缓冲、任务接收缓冲和解析缓冲帧尾均为`0x8AEA`，`copy_mismatch_index=0xFFFF`，证明EMS内部复制过程没有修改该帧。
- **CRC判定 [已验证]：** 对同一帧重新计算的线上CRC字节应为`8A D4`，实际携带`8A EA`；`calculated_crc=0x8AD4`、`received_crc=0x8AEA`、`parser_result=-1`、`failure_stage=7`，Agile Modbus正确拒绝该帧。
- **USART硬件现场 [已验证]：** `uart_isr_snapshot=0x006210D0`仅包含IDLE、TC、TXE/TXFNF、EOBF、CMF、TEACK和REACK标志；`uart_error_code=0`，PE/NE/FE/ORE累计计数均为0。因此当前证据排除DMA长度、Cache后的缓冲复制和解析器改坏数据，但UART无硬件错误标志仍不能区分“DCDC发出错误CRC”和“RS485物理链路把合法字节扰动成另一合法UART字节”。
- **期望帧 [根据协议和代码推断]：** `0x0404/11`正常响应长度27字节、响应头`FF 03 16`；`0x041B/6`正常响应长度17字节、响应头`FF 03 0C`。
- **PC侧被动抓帧条件 [已验证]：** 2026-08-07，EMS与真实DCDC保持连接，PC通过USB-RS485接在DCDC端，仅用串口助手按9600-8N1被动记录约10分40秒；`上电后debug前1分钟.txt`为空文件，不能作为证据，`debug10分钟.txt`有效且共记录63065个十六进制字节。
- **PC抓帧统计 [已验证]：** 有效日志识别4184帧，其中EMS请求2104帧（`0x0404/11`为1064帧、`0x041B/6`为1040帧），DCDC完整响应2080帧（27字节1052帧、17字节1028帧）；所有2080帧完整响应的Modbus CRC均正确，PC侧未捕获到一帧完整坏CRC响应。
- **超时对应关系 [已验证]：** 请求/响应配对发现25个未配对请求，其中最后1个发生在日志结束瞬间，排除该截断项后为24次真实无响应，与EMS的`timeout_count=24`完全一致；两个读取块各12次。说明EMS超时不是纯内部误判，DCDC/链路确实存在偶发无响应。
- **同载荷CRC对照 [已验证]：** EMS最后一次失败记录的载荷前缀`FF 03 0C 00 00 00 00 01 DE 00 01 00 00 FF FF`在PC日志中出现233次，PC捕获的帧尾均为正确CRC `2E 81`，错误尾字节`2E C0`出现0次；而EMS DMA曾记录同一载荷结尾为`2E C0`。因此当前证据不支持“DCDC稳定计算出错误CRC”，也不支持EMS解析/复制改坏数据。
- **10分钟EMS统计 [已验证]：** `success_count=1084`、`timeout_count=24`、`protocol_error_count=24`，总事务1132次，成功率约95.76%，总失败率约4.24%（超时和CRC错误各约2.12%），约每分钟4.5次失败；在线状态和后续自动恢复正常，CRC错误帧均被拒绝，没有发现错误工程量被提交。
- **当前定位结论 [代码与测试证据推断]：** 剩余嫌疑集中在RS485信号裕量/布线/共地/终端、电气支路和第三接收器影响、EMS侧RS485收发器接收输出，或STM32 USART采样到DMA之前；仅靠现有软件计数和PC在DCDC端的单点抓帧不能继续唯一定位。
- **问题评级与暂缓决定 [已确认]：** 对当前“真实DCDC只读采集与后续非控制功能开发”评级为**P1（中高风险，可有条件暂缓）**；对“开放DCDC写命令、闭环控制、安全联锁、现场/量产放行”升级为**P0阻断项（不可跳过）**。暂缓期间必须保持CRC严格校验、DCDC真实写控制关闭、失败计数和详细诊断保留，并把数据新鲜度、连续失败和安全降级作为后续控制接口的前置条件。
- **下一次硬件定位 [暂停、等待设备]：** 有逻辑分析仪或示波器后，在EMS的USART2_RX（PD6）/收发器RO及GND按9600-8N1捕获，并与RS485 A/B波形对照。无仪器期间可做不改变代码的低成本复测：移除USB-RS485第三接收器后运行30～60分钟、缩短EMS-DCDC连线并可靠共地、断电测量A-B终端等效电阻、将被动USB抓取点移到EMS接口端比较失败率。最终必须用硬件测量区分EMS收发器与USART采样，不能把当前问题标记为关闭。

### 7.5 USART2阻塞定长与DMA+IDLE接收A/B诊断（2026-08-11）

- **阻塞定长模式 [已验证]：** 将`MODBUS_BLOCKING_RX_DIAG_ENABLE`临时设为1，绕过DMA、IDLE中断和任务通知，约10分钟后DCDC计数为`success_count=0xBE(190)`、`timeout_count=0x73(115)`、`protocol_error_count=0x107(263)`；诊断同时记录`transmit_failure_count=0x4D(77)`、`deserialize_failure_count=0xBA(186)`、`crc_mismatch_count=0xB6(182)`。该临时实现的总体表现明显差于原DMA路径，不能用它证明DMA/IDLE是剩余错误的唯一原因，也不应作为正式接收方案。
- **恢复DMA+IDLE [已验证]：** 将`MODBUS_BLOCKING_RX_DIAG_ENABLE`恢复为0并重新测试约10分钟，DCDC计数为`success_count=0x398(920)`、`timeout_count=0x19(25)`、`protocol_error_count=0x21(33)`，总事务978次，成功率约94.07%、失败率约5.93%；说明当前DMA+IDLE模式明显优于该阻塞诊断实现。
- **拆除USB-RS485支路 [已验证，时长待确认]：** 仅保留EMS与DCDC连接后的截图计数为`success_count=0x34A(842)`、`timeout_count=0x16(22)`、`protocol_error_count=0x1B(27)`，总事务891次，成功率约94.50%、失败率约5.50%。与接有USB-RS485时的约5.93%接近，当前样本不支持把第三接收器认定为唯一根因。
- **当前代码状态 [已验证]：** `Drivers/BSP/Components/RS485/modbus.h`中`MODBUS_BLOCKING_RX_DIAG_ENABLE=0`、`MODBUS_RX_DIAG_ENABLE=1`，工程已经恢复USART2 DMA+IDLE接收并保留详细诊断；`Core/Src/main.c`中的`SCB_EnableDCache()`仍为临时注释状态，尚未完成正式Cache一致性修复。

### 7.6 PC直连真实DCDC主动轮询对照（2026-08-11）

- **测试拓扑 [已验证]：** 测试时拆除EMS，由PC上的Witte Software Modbus Poll经USB-RS485作为唯一主站直连真实DCDC；从站地址`0xFF`，Modbus RTU `9600-8-N-1`，功能码`0x03`，扫描周期500 ms。该拓扑用于排除EMS主站接收和解析路径，但不能区分DCDC、RS485电气链路、USB-RS485适配器及PC驱动。
- **`0x0404/11`结果 [已验证]：** 连续约10分钟，Modbus Poll显示`Tx=1205、Err=15`，错误率约1.245%，显示成功率约98.755%。保留的通信日志只覆盖末尾18笔事务：其中15笔为27字节完整响应且CRC正确；3笔为前27字节CRC正确后再附加`0xFF`，附加字节事务的记录时延约108～110 ms，正常帧约75～81 ms。日志不是全部1205笔事务，不能据此分类全部15次错误。
- **`0x041B/6`结果 [已验证]：** 连续约10分钟，Modbus Poll显示`Tx=1223、Err=8`，错误率约0.654%，显示成功率约99.346%。保留日志覆盖末尾64笔有响应事务和1笔未见响应请求；56笔为17字节完整响应且CRC正确，8笔为前17字节核心帧CRC正确后再附加1字节，其中`0xFF`5次、`0xFC`2次、`0xFE`1次；正常帧记录时延平均62.5 ms，附加字节记录约92～103 ms。日志片段还发现Tx序号2480未记录响应，但日志片段、截图错误计数和软件内部计数口径不能保证逐条一一对应。
- **寄存器换算 [已验证]：** `0x041B～0x0420`原始值依次为`0、0、478、0、0、-1`，对应B侧实时功率0 W、P侧实时功率0 W、B侧实时电压47.8 V、P侧实时电压0 V、B侧实时电流0 A、P侧实时电流-0.01 A；与EMS此前解析结果一致，确认地址、符号和缩放关系正确。
- **CRC结论 [已验证]：** 两个读取块在PC直连日志中保存下来的核心响应帧均可通过标准Modbus RTU CRC校验；当前证据不支持“DCDC稳定生成错误CRC”，也不支持“EMS寄存器解析导致错误”。PC直连异常更接近“有效帧后偶发额外字节/偶发无响应”。
- **比较结论 [根据测试推断]：** `0x041B/6`显示错误率低于`0x0404/11`，但两轮样本不足以确认异常与起始地址、寄存器数量或响应长度存在因果关系；不能仅凭本次比例差异修改轮询块。
- **当前处置 [已确认]：** 本轮PC直连测试到此结束，不继续追加扫描周期对照。保留现有严格CRC校验、失败计数和自动恢复；偶发通信问题继续按只读阶段P1已知风险、真实写控制/闭环控制/现场放行P0阻断项管理，等待逻辑分析仪或示波器进行物理层唯一定位。

### 7.7 编译/警告状态

- **[已验证]** 2026-08-06 增加USART2最小物理层/复制诊断后再次Rebuild：`0 Error(s), 13 Warning(s)`，Build Time约40秒；程序尺寸为`Code=592684`、`RO-data=6273024`、`RW-data=1572`、`ZI-data=492812`。公开的`g_modbus_rx_diag`为148字节，内部单事务快照为24字节；新增诊断无编译错误。
- **[已验证]** 2026-08-06 加入真实DCDC详细收帧诊断后，使用Keil目标`BSMU_H750IB`完成Rebuild：`0 Error(s), 13 Warning(s)`，Build Time约40秒；程序尺寸为`Code=592196`、`RO-data=6273024`、`RW-data=1572`、`ZI-data=492724`。本次没有诊断代码编译错误，新增的未引用警告来自当前主动关闭的`acdc_poll`、`acdc_update_communication_state`和`dcdc_process_write_command`等业务路径。
- **[已验证]** 2026-07-22 当前工作区完整 Rebuild 成功：最终汇总为 `0 Error(s), 10 Warning(s)`，Build Time 45 秒。
- **[已验证]** 程序尺寸：`Code=593824`、`RO-data=6273028`、`RW-data=1576`、`ZI-data=492680`。
- **[已验证]** 本次源码 warning 共 7 条：
  - `Core/Src/http_client.c`：不可达语句 1 条；`httpc_verify_cert()` 未引用 1 条。
  - `Drivers/BSP/Components/RS485/modbus_slave.c`：`slave_addr` 未引用 1 条。
  - `cJSON/cJSON.c`：ARMCC 不识别 GCC diagnostic pragma，共 3 条。
  - `flash_storage/port/sfud_port.c`：`qspi_read()` 未引用 1 条。
- **[已验证]** 链接 warning 共 3 条：scatter 文件中的 `CustomContainer4Base.o(RO)`、`SVGDatabase.o(RO)`、`UnmappedDataFont.o(RO)` 规则只匹配到已被移除的未使用 section。
- **[已验证]** `MDK-ARM/BSMU_H750IB/BSMU_H750IB.axf`、`.hex`、`.map` 均已生成。
- **[已验证]** After Build User command #1 仅调用 `fromelf.exe` 而未附参数，因此输出帮助文本；没有使构建失败，但该用户命令配置应后续检查，避免误导日志判断。
- 当前 `DCDC_MODBUS_PC_TEST_ENABLE=0`，PC Watch写触发入口已关闭；DCDC任务中的写命令处理调用也保持注释，当前只做真实设备只读联调。

### 7.8 其他待确认

- `EMS_strategy.*` 是未跟踪文件；本次日志确认 `EMS_strategy.c` 已参与编译，但其接口、完成度和运行测试状态仍待确认。
- 当前源码使用 LAN8742 配置，README 称 LAN8720，实物 PHY 待确认。
- HTTPS 明文鉴权配置、`VERIFY_OPTIONAL` 和证书验证路径存在安全风险。

## 8. 编译与运行方法

### 8.1 打开和编译

1. 用 Keil MDK 打开 `MDK-ARM/BSMU_H750IB.uvprojx`。
2. 选择 target `BSMU_H750IB`；当前工程配置 ARM Compiler V5.06 update 7 build 960。
3. 先执行 Rebuild，记录错误、warning、Code/RO/RW/ZI 和生成的 `BSMU_H750IB.hex`。
4. **不要**为消除工作区差异而 restore/reset；先检查 `git diff`，尤其是 `.uvprojx`、Modbus、USART、LwIP 和 TouchGFX 文件。

### 8.2 下载、复位和启动

1. 板卡上电；README 说明下载电路必须上电。
2. 通过工程现有 ST-Link/J-Link 配置下载（实际使用哪一种 **待确认**）。
3. 调试模式 Reset 后立即 Run；`main()` 从应用地址设置 VTOR，随后启动 RTOS。
4. Modbus PC 模拟时，先让 Modbus Slave 连接并建好全部窗口，再复位 EMS，避免启动阶段历史超时污染计数。

### 8.3 串口与日志

- USART2 是 RS485 Modbus，不是主要调试日志口；8N1，波特率由 `bsmuSetting.RS485Bps` 决定：0/1/2/3/4/5 对应 115200/57600/38400/19200/9600/4800，其他值回退 9600。
- PC 模拟：USB-RS485 A-A、B-B、建议共地；Witte Modbus Slave 使用 RTU/8N1/相同波特率，一个 COM 口只由一个程序占用。
- 调试打印通过 `Debug_printf` 及其 UART/互斥量路径；实际调试 UART 端口、COM 号和默认波特率 **待确认**。
- `CPU_Task()` 可观察 FreeHeap、MinFreeHeap、各任务最小剩余栈和 CAN 队列。

### 8.4 网络与 HTTPS 验证

1. 接入可提供 DHCP/DNS 的 Ethernet 网络，观察 `netif is up`、DHCP 地址日志。
2. 确认 `bsmuSetting.yunduan_flag` 开启后 `httpc_task()` 不再停留在 waiting for DHCP。
3. 验证 DNS 解析、TCP 443、TLS handshake 和 HTTP 响应日志。
4. 使用服务器端或抓包确认站级/簇级 API 请求；不要在日志中打印 `SERVER_TOKEN` 或完整证书。
5. MQTT 验证：不适用；当前工程未实现 MQTT/MQTTS。

## 9. Git 状态

### 9.1 分支与提交

- 当前分支：`httpc`，跟踪 `origin/httpc`。
- 最近相关提交：
  - `826fbff`（2026-06-04）`feat: adapt TouchGFX main UI to 1 BCMU and 2 BMUs`
  - `961d0c8`（2026-06-03）`feat: adapt battery stack topology to 1 BCMU and 2 BMUs`
  - `edb67d5`（2026-06-03）`chore: save baseline before battery stack topology change`
  - `c1cf593`（2025-12-08）`enhance httpc_recv function`

### 9.2 已修改但未提交（审计时）

`CmBacktrace/cm_backtrace/cm_backtrace.c`、`Core/Inc/FreeRTOSConfig.h`、`Core/Inc/usart.h`、`Core/Src/CAN_Control.c`、`Core/Src/freertos.c`、`Core/Src/heap_4_addition.c`、`Core/Src/http_client.c`、`Core/Src/main.c`、`Core/Src/screen_sleep.c`、`Core/Src/usart.c`、`Drivers/BSP/Components/RS485/modbus.c`、`modbus.h`、`modbus_data.c`、`modbus_slave.c`、`LWIP/Target/lwipopts.h`、`MDK-ARM/BSMU_H750IB.uvoptx`、`MDK-ARM/BSMU_H750IB.uvprojx`、`Middlewares/Third_Party/FreeRTOS/Source/tasks.c`、多个 TouchGFX generated/text/font 文件、`TouchGFX/gui/src/mainscreen_screen/MainScreenView.cpp`、`flash_storage/port/ef_port.c`、`flash_storage/port/sfud_port.c`、`项目记忆.md`。

### 9.3 删除和未跟踪

- 删除状态：`TouchGFX/generated/images/src/.idea/modules.xml`、`src.iml`、`workspace.xml`。
- 未跟踪：`Core/Inc/EMS_strategy.h`、`Core/Src/EMS_strategy.c`、`docs/` 下 5 份 Modbus 测试 docx、`tools/append_deferred_issue_record.py`、`tools/append_dual_slave_stability_v14.py`、`tools/append_skip_response_test.py`，以及本文 `PROJECT_CONTEXT.md`。

### 9.4 保护要求

- 上述差异均视为用户现有工作；不得覆盖、恢复、删除或批量格式化。
- 未经用户明确要求，不执行 commit、push、checkout/switch、reset、clean、rebase、merge 或删除操作。
- `README.md` 中的示例 `git restore ...` 不是当前可直接执行的步骤；只有确认文件无用户改动后才可考虑。

## 10. 约束与注意事项

- 任何修改前先执行 `git status --short --branch` 和针对目标文件的 `git diff -- <path>`。
- 保留 CubeMX `USER CODE BEGIN/END` 中的用户代码；对块外已有手工补丁先做差异清单。
- CubeMX 重新生成后重点复核：Ethernet 热插拔 `HAL_ETH_Start_IT()`、USART2 波特率/DMA/IDLE、TouchGFX 中断恢复、FDCAN 配置、LwIP `cc.h/lwipopts.h`、Keil 文件组。
- 不随意修改固定内存地址、linker section、MPU、Cache 属性、Ethernet 描述符和 DMA 缓冲区。
- 不在源码、Markdown、日志或提交信息中复制账号、密码、Token、私钥或完整证书；只记录配置位置/变量名。
- ACDC 与 DCDC 必须使用不同从站地址，共享一条二线总线；EMS 是唯一主站，所有 USART2 Modbus 事务必须由同一个任务串行执行。
- ACDC 地址已确认为 33；真实 DCDC 当前实机地址确认为 `0xFF`，PC Modbus Slave历史模拟地址为1，二者不得混用；模拟窗口使用原始十进制寄存器地址并关闭 PLC Base-1 显示。
- 当前 5 ms RTU 帧间隔是已确认技术决策；不要为了提速直接删除。
- 当前拓扑宏 `cluster_num=1`、`GRP_num=2`；改拓扑时必须同步 CAN、数据结构、HTTP、TouchGFX 和模拟器宏。
- 真实 DCDC 接入前关闭 `DCDC_MODBUS_PC_TEST_ENABLE`；真实写控制前完成范围校验、互锁、设备状态机和异常恢复测试。

## 11. 下一步计划

| 优先级 | 任务 | 目标/涉及文件 | 验证标准 | 依赖 |
|---:|---|---|---|---|
| P0（控制/放行）/P1（当前只读） | 外部确认DCDC异常CRC进入EMS前的来源（当前暂缓） | USART2_RX/PD6、RS485 A/B、`g_modbus_rx_diag` | 外部捕获同一事务并对照DCDC端、EMS端与DMA原始字节；明确故障位于物理链路、EMS收发器还是USART采样，随后完成修复和长稳复测 | 当前缺少逻辑分析仪/示波器；在开放写控制、闭环控制或现场放行前必须完成 |
| P2 | 整理当前构建 warning 和 After Build 命令 | `http_client.c`、`modbus_slave.c`、`sfud_port.c`、scatter/Keil User Command | 在不改变业务行为的前提下确认每条 warning；`fromelf` 命令不再无参数打印帮助 | 先完成通信异常测试；修改前逐文件核对 diff |
| P0 | 完成剩余异常测试 | `modbus.c/.h`、`usart.c`、V1.7 测试文档 | 持续无响应、CRC和Exception 06双向隔离与恢复已通过；继续完成恢复时间和至少5轮重复性 | PC Modbus Slave、USB-RS485 |
| P0 | 定位电池页面异常 | `CAN_Control.c`、`station_ctl.c`、TouchGFX Model/View、Modbus 超时路径 | 首次异常写入点可定位；`Client_Sd[]` 不再被破坏；无仅掩盖问题的 UI 修补 | 稳定复现、Watch/数据断点 |
| P1 | 完成真实DCDC只读稳定性和Cache正式修复 | `modbus.c/.h`、`RS485.c/.h`、`Core/Src/main.c`、MPU配置 | 保持`DCDC_MODBUS_PC_TEST_ENABLE=0`；恢复D-Cache后采用局部非缓存DMA区或正确Cache维护；连续运行不少于10分钟，通信失败率达到项目验收阈值（阈值待确认）且无数据错乱 | 本轮详细收帧诊断结论、内存布局与MPU方案评审 |
| P1 | 设计真实 DCDC 控制状态机 | `EMS_strategy.*`、`modbus.c/.h` | 白名单、上下限、启停前置条件、故障互锁、幂等与读回失败策略有评审和测试 | 设备协议、系统控制需求 |
| P1 | 核对 `EMS_strategy.*` | 新增策略文件、Keil 文件组 | 明确接口、调用者、完成度；编译接入一致 | 用户确认设计意图 |
| P2 | HTTPS 安全加固 | `http_client.c`、`ca_certificates.h`、配置存储 | 无明文凭据；证书必选验证；错误证书必须拒绝 | 服务器证书/安全配置方案 |
| P2 | 清理并提交前审查 | 全部 Git 差异 | 用户确认每组变更；生成文件/IDE 文件去留明确 | 前述测试通过；用户授权 Git 操作 |

**下一步最先执行：** 2026-08-11的阻塞/DMA A/B、拆除USB支路和PC直连DCDC主动轮询测试已经结束，无新测试条件时不要重复。CRC/超时偶发问题继续作为P1已知风险保留，当前可继续不依赖真实DCDC写控制的工作；保持真实DCDC地址`0xFF`、功能码`0x03`只读、DMA+IDLE接收、CRC严格校验、`MODBUS_RX_DIAG_ENABLE=1`及DCDC写入口关闭。优先完善数据新鲜度/连续失败的失效安全规则；逻辑分析仪/示波器到位后恢复物理层定位。在此之前不得开放真实写控制、闭环控制或宣布通信达到现场/量产放行条件。D-Cache当前仍为临时诊断状态，正式恢复方案也不得遗漏。

## 12. 新对话接续指令

可直接复制以下提示词到新的 Codex 对话：

```text
请接续 Hdu_EMS_H750 工程工作。工程根目录中有 PROJECT_CONTEXT.md。

开始前必须：
1. 完整读取 PROJECT_CONTEXT.md，并读取与当前任务直接相关的源码/文档；
2. 执行只读的 git status --short --branch、git diff --stat，并检查目标文件的实际 diff；
3. 以工程当前源码、Git 状态和最新测试证据为准，不要仅依据摘要假设某项修改已经完成；
4. 不要重复已经明确验证通过的正常读写和双从站稳定性测试，除非本次改动会影响它们或我明确要求回归；
5. 修改前保留所有用户已有改动，不得 restore、reset、clean、切换分支、提交或推送，除非我明确授权；
6. 无法从工程、Git、日志或测试记录确认的信息标为“待确认”，不要编造；
7. 不要输出或复制账号、密码、Token、私钥或完整证书内容。

当前优先事项是：真实DCDC从站地址已经确认并配置为0xFF，当前仅执行功能码0x03只读轮询，ACDC轮询、DCDC写命令处理和PC Watch写触发均已关闭。SCB_EnableDCache()仅为USART2 DMA一致性对照测试而临时注释，不能作为正式修复。当前已经恢复DMA+IDLE接收（`MODBUS_BLOCKING_RX_DIAG_ENABLE=0`），阻塞定长A/B诊断明显更差，不采用为正式方案。内部诊断确认DMA、ISR、任务和解析缓冲复制一致；2026-08-07 PC被动抓取的2080帧完整响应CRC全部正确。2026-08-11 PC作为唯一主站直连DCDC时，`0x0404/11`约10分钟为`1205次/15错`，`0x041B/6`约10分钟为`1223次/8错`；日志中核心响应帧CRC正确，异常主要为有效帧后偶发附加字节及少量无响应。测试到此暂停，剩余嫌疑仍包括RS485物理链路、收发器、USB适配器和USART采样，需要逻辑分析仪/示波器才能唯一定位。该问题按只读阶段P1有条件暂缓，但对写控制、闭环控制和现场/量产放行是P0阻断项；不得把它标记为已解决。暂缓期间继续保持严格CRC、详细计数、只读和DCDC写入口关闭，下一步优先做数据新鲜度/连续失败的失效安全设计。不要重复已经通过的PC双从站正常读写、持续失联、CRC、Exception 06以及本轮PC直连DCDC测试；DCDC Skip response期间Client_Sd[]/电池页面偶发异常仍是独立待办。
```

---

### 维护提示

每完成一项功能或测试，应同步更新：当前阶段、已验证结果、当前问题、Git 状态、下一步计划和最近核对日期。若本文与源码冲突，先修正文档，不要为了匹配文档而改源码。
