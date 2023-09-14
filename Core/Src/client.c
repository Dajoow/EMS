/**
  ******************************************************************************
  * File Name          : client.c
  * Description        : serve as client
  ******************************************************************************
  * @attention
  *
  *
  * @author:yhb
  *
  *
  ******************************************************************************
  */
#include "main.h"
#include "client.h"
#include "cmsis_os.h"
#include "sockets.h"
#include "task.h"
#include "CAN_Control.h"
#include "station_ctl.h"
#include "usart.h"
#include "at24cxx.h"

osThreadId Client_threadHandle;
osThreadId TCP_Recv_threadHandle;

extern osSemaphoreId ETHSndSemHandle;
extern BCMU_Mail_t BCMU[cluster_num];

//LAN8720硬件复位
void LAN8720_RESET(void)
{
	HAL_GPIO_WritePin(ETH_RESET_GPIO_Port, ETH_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(55);
	HAL_GPIO_WritePin(ETH_RESET_GPIO_Port, ETH_RESET_Pin, GPIO_PIN_SET);
}


//客户端数据包初始化
void clientFrameInit()
{
	StationDataInit();
}


void TCP_Recv(void const * conn_fd)
{
	int conn_fd_t = *(int*)conn_fd;
	char *recv_data;
	int recv_data_len;
	
	recv_data = (char *)pvPortMalloc(RECV_DATA_MAX); 
	for(;;)
  	{
			recv_data_len = recv(conn_fd_t, recv_data, RECV_DATA_MAX, 0);                  //还未考虑TCP断开连接发送不成功和未接收到数据情况
			if(recv_data_len>0)
			{
	//			write(conn_fd_t,recv_data,recv_data_len);
	//			write(conn_fd_t,(void*)0xC0400000,4100*20);
				// HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
				Debug_printf("%s\n", recv_data);
				memset(recv_data, 0x00, recv_data_len);
			}
		osDelay(1);
  	}
}



//进行TCP连接，并创建TCP收发两个线程
void Client(void const * argument)
{
	struct sockaddr_in conn_addr;				//服务器地址 
	int conn_fd;								        //服务器的 socked fd=file descriptor
#if keepAlive
	int so_keepalive_val = 1;
 	int tcp_keepalive_idle = 5;					/* 5S没有数据交互 开始发送心跳包 S*/	
 	int tcp_keepalive_intvl = 1;				/* 发送心跳间隔 S */
 	int tcp_keepalive_cnt = 3;					/* 心跳重发次数 3次没有响应进入*/	
#endif
	int tcp_nodelay = 1;								//禁用Nagle算法
//	void *p = (void*)0xC0400000;
//	void *p = (void*)0x24000000;
//	char bTmpSendBuf[]="hello server!This is BSMU connect!\n";
	
	for(;;)
  {
		conn_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(conn_fd<0)
		{
			Debug_printf("Socket error\n");
			vTaskDelay(100);
			continue;
		}
#if keepAlive
		//使能socket层的心跳检测
		setsockopt(conn_fd, SOL_SOCKET, SO_KEEPALIVE, &so_keepalive_val, sizeof(int));
#endif
		conn_addr.sin_family = AF_INET;
		conn_addr.sin_port = htons(bsmuSetting.port);
		// conn_addr.sin_port = htons(SERVER_PORT);
		uint32_t IP_ADDR = (bsmuSetting.IP_ADD_1[3]<<24)|(bsmuSetting.IP_ADD_1[2]<<16)|\
								(bsmuSetting.IP_ADD_1[1]<<8)|(bsmuSetting.IP_ADD_1[0]);
		conn_addr.sin_addr.s_addr = IP_ADDR;
		// conn_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
		memset(&(conn_addr.sin_zero), 0, sizeof(conn_addr.sin_zero));    //此段保留未用
		
		if( connect(conn_fd, (struct sockaddr *)&conn_addr, sizeof(struct sockaddr_in)) == -1)
		{
			Debug_printf("connect error\n");
			closesocket(conn_fd);
			vTaskDelay(100);
			continue;
		}
		//配置心跳检测参数
#if keepAlive
		setsockopt(conn_fd, IPPROTO_TCP, TCP_KEEPIDLE, &tcp_keepalive_idle, sizeof(int));	
 		setsockopt(conn_fd, IPPROTO_TCP, TCP_KEEPINTVL, &tcp_keepalive_intvl, sizeof(int));	
 		setsockopt(conn_fd, IPPROTO_TCP, TCP_KEEPCNT, &tcp_keepalive_cnt, sizeof(int));	
#endif
		setsockopt(conn_fd, IPPROTO_TCP, TCP_NODELAY, &tcp_nodelay, sizeof(int));	
		Debug_printf("connect successful!conn_fd=%d\r\n",conn_fd);  
		clientFrameInit();                                                //初始化数据
		if(TCP_Recv_threadHandle==NULL)
		{
			osThreadDef(TCP_Recv_thread, TCP_Recv, osPriorityNormal, 0, 512);
			TCP_Recv_threadHandle = osThreadCreate(osThread(TCP_Recv_thread), &conn_fd);
		}
		while(1)      //在此处完成发送
		{	
		  if(xSemaphoreTake(ETHSndSemHandle,portMAX_DELAY)== pdTRUE)//等待获取信号量(等待FDCAN轮询完大概1s)
			{
				//发送电池簇数据
				for(uint8_t i=0; i<cluster_num; i++){
					if(BCMU[i].OnlineOrOffline == Online){
//						Client_Sd[i].cluster_No = i+1;//这里BCMU发送粗编号不对，BCMU程序要更改，这里临时改下
//						Client_Sd[i].frame_tail = FRAME_TAIL; //这里BCMU发送数据帧尾不对，BCMU程序要更改，这里临时改下
						if(write(conn_fd, (void*)&Client_Sd[i], sizeof(Client_Sd_t))>0)  //write的返回值是发送数据的字节数
						{

						}
						else
						{
							Debug_printf("Send FAIL!\r\n");  
							goto remake;   //发送失败重新连接服务器
						}
					}
				}
				//发送整个电站数据
				if(write(conn_fd, (void*)&Client_Sd_Station, sizeof(Client_Sd_Station_t))>0)  //write的返回值是发送数据的字节数
				{

				}
				else
				{
					Debug_printf("Send FAIL!\r\n");  
					goto remake;   //发送失败重新连接服务器
				}
				HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
			}
		}
		remake:
		closesocket(conn_fd);
		vTaskDelay(50);
    osDelay(1);
  }
}



//创建一个任务来进行TCP连接
void ClientInit()
{
	osThreadDef(Client_thread, Client, osPriorityNormal, 0, 512);
	Client_threadHandle = osThreadCreate(osThread(Client_thread), NULL);
}












