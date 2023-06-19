/**
  ******************************************************************************
  * File Name          : CAN_Control.h
  * Description        : CAN send receive
  ******************************************************************************
  * @attention
  *
  *
  * @author:yhb
  *
  *
  ******************************************************************************
  */

#ifndef CAN_CONTROL_H_
#define CAN_CONTROL_H_

#include "main.h"
#include "cmsis_os.h"


#define Master							1														//主机
#define Slave						  	0														//从机
#define MasterOrSlave     	Master                       //Master=1,Slave=0

#if MasterOrSlave == Master
#define CAN_ID              0x000												//范围0x000-0x7ff  主机为0x000
#else
#define CAN_ID              0x101												//范围0x000-0x7ff  主机为0x000
#endif

#define BCMU1_ID 		0x101
#define BCMU2_ID 		0x102
#define BCMU3_ID 		0x103
#define BCMU4_ID 		0x104
#define BCMU5_ID 		0x105
#define BCMU6_ID 		0x106
#define BCMU7_ID 		0x107
#define BCMU8_ID 		0x108
#define BCMU9_ID 		0x109
#define BCMU10_ID		0x10a
#define BCMU11_ID 	0x10b
#define BCMU12_ID 	0x10c
#define BCMU13_ID 	0x10d
#define BCMU14_ID 	0x10e
#define BCMU15_ID 	0x10f
#define BCMU16_ID 	0x110
#define BCMU17_ID 	0x111
#define BCMU18_ID 	0x112
#define BCMU19_ID	 	0x113
#define BCMU20_ID	 	0x114

#define BCMUx(x)    					BCMU##x 		

typedef struct
{
		uint32_t mailbox;
		FDCAN_TxHeaderTypeDef TxHeader;
		uint8_t payload[64];
}CANTxMsg_t;

typedef struct
{
		FDCAN_RxHeaderTypeDef RxHeader;
		uint8_t payload[64];
}CANRxMsg_t;

typedef enum {
 frame_head1status = 0,
 frame_head2status = 0x01,
 frame_cmdstatus = 0x02,
 frame_len1status = 0x03,
 frame_len2status = 0x04,
 frame_datastatus = 0x05,
 frame_crc1status = 0x06,
 frame_crc2status = 0x07,
 handle = 0x08
}FRAME_STATUS_t;

#if MasterOrSlave == Master
#define Online								1														//在线
#define Offline						  		0														//离线
typedef struct{
	uint32_t ID;                  //对应FDCAN ID
	uint8_t  OnlineOrOffline;     //BCMU的在线离线情况 1为online
	uint32_t Request_Cnt;					//从机被请求计数
	uint32_t Frame_Cnt;           //累计接收数据包的总数
  uint32_t EffectiveFrame_Cnt;  //累计接收数据包的总数中完成校验有效的帧数
	osMessageQId* BCMUQueue;      //对应的消息队列指针
	uint8_t*  Data_buf;						//暂存的8k字节的数据数组指针
}BCMU_Mail_t;
#endif

extern FDCAN_HandleTypeDef  hfdcan2;
#define Myfdcan						  hfdcan2

#define CAN_Rev_DLC     		64                      //实际值
#define CAN_Snd_DLC     		64											//配置参数值

#define FDCAN_REV_BuffSize 		1024*8                 //接收缓存区大小
#define FDCAN_SND_BuffSize 		1024*32								//发送缓存区大小

//CAN传输层协议相关参数 
#define FRAME_HEAD1 				0xAA                   //帧头高8
#define FRAME_HEAD2 				0x55				   //帧头低8

#define ACK_ERROR_TIME_MAX			5
#define WAIT_PACK_TIME				100						//500k 500k速率以上的等待时间
#define LENGTH_MAX					0x1400   							//5k

/*命令码*/
#define CMD_TRANS_START  		0xC0
#define CMD_TRANS_STOP  		0xC1
#define CMD_TRANS_ACK     		0xC2

#define DATA_NOMAL        		0X80								   //普通数据上传
#define DATA_URGENT       		0X81									 //紧急数据上传，如报警信息



void BSMU_CANInit(void);

#endif



