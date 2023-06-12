## STM32+USART+DMA+EC600N调试

> 用到的工具：
>
> ​		公用的TCP/IP服务器：http://tongxinmao.com
>
> ​		串口调试助手：sscom33
>
> ​		网络调试助手：通信猫调试软件

### 1.USART+DMA调试

1. 在stm32Cube中，打开DMA发送中断和接收中断，打开usart全局中断。

2. 主要调试功能：（1）使用DMA发送固定长度数据给串口，（2）使用DMA接收不定长度帧数据。

3. （1）利用DMA传输，发送固定大小数据

   ```c
   HAL_UART_Transmit(&huart1,BigTxData,2,100);
   ```

   换成

   ```c
   HAL_UART_Transmit_DMA(&huart1,BigTxData,1023);
   ```

   包装代码如下：

   ```c
   //串口1的DMA发送
   void UART1_TX_DMA_Send(uint8_t *buffer, uint16_t length)
   {
       //等待上一次的数据发送完毕
   	while(HAL_DMA_GetState(&hdma_usart1_tx) != HAL_DMA_STATE_READY);
       //while(__HAL_DMA_GET_COUNTER(&hdma_usart1_tx));
   	
       //关闭DMA
       __HAL_DMA_DISABLE(&hdma_usart1_tx);
   
       //开始发送数据
       HAL_UART_Transmit_DMA(&huart1, buffer, length);
   }
   
   //串口1的DMA发送printf
   void Debug_printf(const char *format, ...)
   {
   	uint32_t length = 0;
   	va_list args;
   	
   	__va_start(args, format);
   	
   	length = vsnprintf((char*)usart1_tx_buffer, sizeof(usart1_tx_buffer), (char*)format, args);
   	
   	UART1_TX_DMA_Send(usart1_tx_buffer, length);
   }
   ```

   （2）利用DMA传输，接收大小可变的数据

   利用串口空闲中断，识别一帧的数据，参考链接：

   [CSDN]: https://blog.csdn.net/qq_30267617/article/details/118877845?spm=1001.2101.3001.6650.1&amp;utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-118877845-blog-81638676.pc_relevant_layerdownloadsortv1&amp;depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-118877845-blog-81638676.pc_relevant_layerdownloadsortv1&amp;utm_relevant_index=2	"注意开启串口初始化中开启串口空闲中断"

   ```c
   //串口接收空闲中断
   void HAL_UART_ReceiveIdle(UART_HandleTypeDef *huart)
   {
   	//当触发了串口空闲中断
       if((__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)) 
       {
   
   				if(huart->Instance == USART1)
   				{
   					/* 1.清除标志 */
   					__HAL_UART_CLEAR_IDLEFLAG(huart); //清除空闲标志
   
   					/* 2.读取DMA */
   					HAL_UART_DMAStop(huart); //先停止DMA，暂停接收
   					//这里应注意数据接收不要大于 USART_DMA_RX_BUFFER_MAXIMUM
   					usart1_rx_len = USART_DMA_RX_BUFFER_MAXIMUM - (__HAL_DMA_GET_COUNTER(&hdma_usart1_rx)); //接收个数等于接收缓冲区总大小减剩余计数
   					
   					/* 3.搬移数据进行其他处理 */
   					memcpy(receive_data, usart1_rx_buffer, usart1_rx_len); 
   					usart1_rx_flag = 1; //标志已经成功接收到一包等待处理
   				}
       }
   }
   ```

   

4. 注意：空闲中断结束后，记得重新开启DMA接收。

### 2.EC600N调试

> EC600N-CN模块使用AT指令控制，可以使用UBS和串口模式通信。这里只研究串口模式通信过程。

1. 指令解析

   ```
   AT+CPIN?\r\n
   查询SIM卡状态，返回1帧
   >+CPIN: READY\r\nOK\r\n
   
   AT+CREG?\r\n
   查询CS业务状态，返回1帧
   >+CREG: 0,1\r\nOK\r\n
   
   
   
   AT+CGREG?\r\n
   查询PS状态，返回1帧
   >+CGREG: 0,1\r\nOK\r\n
   
   
   AT+QICSGP=1\r\n
   查询PDP场景，场景1，返回1帧
   +QICSGP: 1,"CMIOT","","",1\r\nOK\r\n
   
   
   AT+QICSGP=1,1,"CMIOT","","",1
   配置PDP场景1，移动物联网，账号密码不用配置，在wifi中可能需要配置，暂时没有配置QoS，大数据发送可能需要配置阻塞。
   
   >\r\nOK\r\n
   
   
   AT+QIACT=1\r\n
   激活PDP场景1
   >\r\nOK\r\n
   
   
   AT+QIOPEN=1,0,"TCP",120.76.100.197,10002,0,2\r\n
   打开socket连接，透传模式。
   >CONNECT
   
   +++
   退出透传模式,返回两帧
   >OK
   >+QIURC: "recv",0
   
   ATO/r/n
   重新连接
   >CONNECT
   ```
   
   2. AT执行逻辑
   
   每个AT指令执行成功，则继续下一条，如果本条AT指令执行失败，则重复执行，最多执行10次，
   
   如果10全部失败，则本轮结束，从第一条指令开始执行，如果5轮全部失败，则重新执行4G模块
   
   硬件初始化流程（电源复位（可跳过），4G模块复位，开机），然后继续执行AT指令；
   
   3. 主要程序

​	使用消息地图进行顺序状态机的执行

```c
typedef struct 											//定义状态机结构体
{
	unsigned char msg_data[20];
	
	FUN_P fnprocess;
	
}Message_Item;
/*************************消息地图*********************************/
const static Message_Item c_tMSGMap[]  =  {
    {"ATE0\r\n",                lte_4g_protocol_default_send},			//关闭回显
    {"ATE0\r\n",                lte_4g_protocol_default_send},
    {"ATE0\r\n",                lte_4g_protocol_default_send},
    {"ATE0\r\n",                lte_4g_protocol_default_send},
    {"AT+CPIN?\r\n",            lte_4g_protocol_CPIN},							//查询SIM卡状态，返回1帧
    {"AT+CSQ\r\n",              lte_4g_protocol_CSQ},								//查询信号强度
    {"AT+CREG?\r\n",            lte_4g_protocol_CREG},							//查询CS状态，返回1帧
    {"AT+QICSGP",               lte_4g_protocol_QICSGP},            //设置APN,用户名和密码
    {"AT+QIDEACT=1\r\n",        lte_4g_protocol_default_send},          //去激活
    {"AT\r\n",                  lte_4g_protocol_default_send},			
    {"AT\r\n",                  lte_4g_protocol_default_send},
    {"AT+QIACT=1\r\n",          lte_4g_protocol_default_send},      //激活
    {"AT+QIACT?\r\n",           lte_4g_protocol_default_send},      //是否激活成功
   // {"AT+QICLOSE=0\r\n",        lte_4g_protocol_MYNETCLOSE},        //关闭socket
    {"AT+QIOPEN",               lte_4g_protocol_QIOPEN},            //创建socket
};
```

其中一个状态机中的发送处理函数

```c
unsigned char lte_4g_protocol_default_send(void)
{
		unsigned char success_flag=0;
		Debug_printf("%s",c_tMSGMap[count3].msg_data);			//发送命令
		success_flag=findStr((char *)receive_data, "OK",200);		//等待接收并检测
				
		memset(receive_data,'\0',sizeof(receive_data));					//清空数组
		usart1_rx_flag=0;																				//复位空闲标志位---可以加一个判断。
	
		if(success_flag)				
		return 1;
		else
		return 0;
		
}
```

等待与查询函数

```
static unsigned char findStr(char* dest,char* src,unsigned int retry_cn)
{
    unsigned int retry = retry_cn;                   //超时时间
    unsigned char result_flag = 0;                     //查找结果

    while(strstr(dest,src)==0 && --retry!=0)//等待串口接收完毕或超时退出
    {
        HAL_Delay(10);
    }

   if(retry > 0)                           
   {
       result_flag = 1;										 //执行到这里说明一切正常, 表示查找成功
   }                      

   return result_flag;


}
```

