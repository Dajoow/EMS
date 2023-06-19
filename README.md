# BSMU_H750IB

# 使用注意

## 以太网(LAN8720)

- 以太网复位脚pin15引出，低电平复位
- pin10为该芯片的地址，再硬件Reset时会锁存该引脚的电平，作为地址（内部有下拉，地址默认为0）
- 注意上电时序



## LCD

- 板子上的FPC接口与屏幕的FPC接口引脚顺序相反，得使用同向得FPC软排线



## QSPI

- Version1.0使用的是W25Q64FV ，最高速为**104Mhz**
- GPIO的速度改为 very high
- qspi flash是否工作在QSPI模式
- QSPI 的工作频率



## 下载电路

- 必须得上电





## CORETEX_M7

- 打开cache,CPU运行速度比内存快太多
- MPU





## CAN_BUS

- VCC与GND画反，**不可使用**，第二版修改金升阳电源模块的封装



## LCD

- 上电有延迟，开机屏幕会不稳定一会
- 触摸芯片为GT911，通讯方式为I2C通信，建议通信率速400kbps
- T_CS(在GT911中为RST)持续拉高
- T_PEN(在GT911中为INT)设置为上拉输入
- 注意触摸芯片的上电时序（有关从设备地址有关）



## I2C

- IC2引脚输出模式为开漏输出，需要上拉电阻（根据输出电流大小确定阻值一般为2k 4.7k）
- 单片机内部的上拉为弱上拉，电流输出能力有限，一般不用
- 上升沿和下降沿延时拉满（硬件I2C还不是很了解是不是有bug）



## touchGFX

- 	程序运行MX_TouchGFX_Init();时，会调用临界保护代码
      	taskENTER_CRITICAL()；和	taskEXIT_CRITICAL();
      	但是在退出临界保护阶段，并没有重新开启中断，（具体原因待查）。
      	*/
   portENABLE_INTERRUPTS();      //重新开启中断



# 修改意见

## 以太网(LAN8720)

- 以太网复位脚pin15加**上拉**，IO输出1



## LCD

- 将板子上得FPC引脚顺序调换



## QSPI

- 将W25Q64FV改为W25Q64JV，速率最高支持133**Mhz**



## 下载电路

- 将VCC3V0改为VCC_VORE_3V3



## USART

- 两个IO加上拉



## CAN_BUS

- VCC与GND画反，将PIN3与PIN5调换



## 电源开关

- 改为侧面开关



## 按键

- 改为侧面按键，或者改变按键位置



## LCD

- 最好加以阻抗控制







# 软件调试记录

1. Undefined symbol __aeabi_assert (referred from qrcodegen.o).

   https://www.pudn.com/news/630a3c8d88df2007aad9d747.html

   **禁用 assert()**+调用**MicroLIB**

2. LCD时钟不能太快，现在45Mhz

3. 以太网：

   - netif->mtu=1500;

     最大传输单元为1500，建议数据长度不要大于1472，还有28个字节IP头

   - 网线热插拔的实现：

     在**ethernet_link_thread（）**线程中，网线拔出后关闭了以太网中断，再次插入时未开启中断（CUBEMX生成的代码有问题）（信号量有待学习）

         if(linkchanged)
         {
           /* Get MAC Config MAC */
           HAL_ETH_GetMACConfig(&heth, &MACConf);
           MACConf.DuplexMode = duplex;
           MACConf.Speed = speed;
           HAL_ETH_SetMACConfig(&heth, &MACConf);
           HAL_ETH_Start_IT(&heth);     //此处
           //HAL_ETH_Start(&heth);
           netif_set_up(netif);
           netif_set_link_up(netif);
         }

   - 心跳包实现：

     https://blog.csdn.net/Chuangke_Andy/article/details/114164812?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_baidulandingword~default-1-114164812-blog-110439520.pc_relevant_recovery_v2&spm=1001.2101.3001.4242.2&utm_relevant_index=4

     /* 心跳重发次数 3次没有响应进入**哪里未知***/	（有待查看）

   - **以太网收发描述符和收发缓存区内存地址在SRAM3里，每个内存地址不能有重叠**

   - 在**low_level_init**

         HAL_ETH_GetMACConfig(&heth, &MACConf);
         MACConf.DuplexMode = duplex;
         MACConf.Speed = speed;
         HAL_ETH_SetMACConfig(&heth, &MACConf);
         	
         HAL_ETH_Start_IT(&heth);
         HAL_ETH_Start(&heth);     //这里
         netif_set_up(netif);
         netif_set_link_up(netif);

4. FREERTOS：有些函数得使能了才能用

5. 线程优先级ETHIF>TCPIP>CLIENT>OTHER



# 外部SDRAM内存使用情况

| 地址范围                                         |         用途          |
| :----------------------------------------------- | :-------------------: |
| 0xC000 0000——0xC012 C000-1                       |       LCDBuff_1       |
| 0xC012 C000——0xC025 8000-1                       |       LCDBuff_2       |
| 0xC025 8000——0xC038 4000-1                       |       LCDBuff_3       |
| 0xC038 4000——0xC039 2330-1(56kB)(20簇x30组x12个) | ETHBuff(20簇电池数据) |
| 0xC039 2330——0xC03A F000-1(115kB)                |     ETHBuff(预留)     |
| 0xC03A F000——0xC03B 0000-1(4kB)                  |  ETHBuff(电站总数据)  |
| 0xC03B 0000——0xC03D 8000-1(160kB)(8KB*20)        |    FDCAN_REV_Buff     |
| 0xC03D 8000——0xC03F 8000-1(128kB)                | FDCAN_REV_Buff(预留)  |
| 0xC03F 8000——0xC040 0000-1(32kB)                 |    FDCAN_SND_Buff     |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
|                                                  |                       |
| 0xC03E 0000                                      |                       |
|                                                  |                       |
| ——0xC3FF FFFF                                    |       暂未使用        |



# 功耗记录：

- 不驱动LCD：1.7-1.8w（其他功能都打开）
- 驱动LCD：3.5-3.7w（其他功能都打开）





# CUBEMX重新生成后操作：

- 注释QSPI

- ETHLINK线程中增加  HAL_ETH_Start_IT(&heth);     //此处

- DMA初始化要在串口初始化之前

- UART2可以在软件层面进行了TX\RX交换

- \#if  TOUCHGFX_ENABLE

  MX_TouchGFX_PreOSInit();

- 注释掉FDCAN1





![image-20230530182729084](D:\documents\STM32_prj\BSMU_H750IBT6\image-20230530182729084.png)

can处的注释