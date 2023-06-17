/**
  ******************************************************************************
  * File Name          : CAN_Control.c
  * Description        : CAN poll and receive
  ******************************************************************************
  * @attention
  *
  *
  * @author:yhb
  *
  *
  ******************************************************************************
  */
#include "CAN_Control.h"
#include "cmsis_os.h"
//#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "string.h"
#include "fdcan.h"
#include <stdio.h>
#include "station_ctl.h"
#include "at24cxx.h"

CANTxMsg_t TxMsg;  //定义发送邮件实体
CANRxMsg_t RxMsg;  //定义接收邮件实体

osMessageQId CANQueueHandle[cluster_num];
osTimerId CANTimer01Handle;
osThreadId CAN_Rev_TaskHandle;
osThreadId CAN_Poll_TaskHandle;
osSemaphoreId ETHSndSemHandle;
osSemaphoreId ViewUpdateSemHandle;

extern Client_Sd_t Client_Sd[cluster_num];

#if MasterOrSlave==Master
uint8_t FDCAN_REV_Buff[cluster_num][FDCAN_REV_BuffSize] __attribute__((at(CANRevBuffStartAdd)));            //FDCAN接收缓存区  8k*20
uint8_t FDCAN_SND_Buff[FDCAN_SND_BuffSize] 							__attribute__((at(CANSndBuffStartAdd)));						//FDCAN发送缓存区  32k
BCMU_Mail_t BCMU[cluster_num];
uint8_t BCMU_ID[cluster_num][2] = {BCMU1_ID>>8,BCMU1_ID&0x00ff,BCMU2_ID>>8,BCMU2_ID&0x00ff,BCMU3_ID>>8,BCMU3_ID&0x00ff,
	BCMU4_ID>>8,BCMU4_ID&0x00ff,BCMU5_ID>>8,BCMU5_ID&0x00ff,BCMU6_ID>>8,BCMU6_ID&0x00ff,BCMU7_ID>>8,BCMU7_ID&0x00ff,
	BCMU8_ID>>8,BCMU8_ID&0x00ff,BCMU9_ID>>8,BCMU9_ID&0x00ff,BCMU10_ID>>8,BCMU10_ID&0x00ff,BCMU11_ID>>8,BCMU11_ID&0x00ff,
	BCMU12_ID>>8,BCMU12_ID&0x00ff,BCMU13_ID>>8,BCMU13_ID&0x00ff,BCMU14_ID>>8,BCMU14_ID&0x00ff,BCMU15_ID>>8,BCMU15_ID&0x00ff,
	BCMU16_ID>>8,BCMU16_ID&0x00ff,BCMU17_ID>>8,BCMU17_ID&0x00ff,BCMU18_ID>>8,BCMU18_ID&0x00ff,BCMU19_ID>>8,BCMU19_ID&0x00ff,
	BCMU20_ID>>8,BCMU20_ID&0x00ff};
#else
uint8_t FDCAN_REV_Buff[FDCAN_REV_BuffSize];              //FDCAN接收缓存区  8k
uint8_t FDCAN_SND_Buff[FDCAN_SND_BuffSize]; 			 //FDCAN发送缓存区  32k
#endif

static const unsigned char aucCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40
};

static const unsigned char aucCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,
    0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
    0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
    0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
    0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
    0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
    0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
    0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 
    0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
    0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
    0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
    0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
    0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
    0x41, 0x81, 0x80, 0x40
};

//查表法计算crc
uint16_t CRC16_modbus( unsigned char * pucFrame, uint16_t usLen )
{
    unsigned char           ucCRCHi = 0xFF;
    unsigned char           ucCRCLo = 0xFF;
    int             iIndex;

    while( usLen-- )
    {
        iIndex = ucCRCLo ^ *( pucFrame++ );
        ucCRCLo = ( unsigned char )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    return ( uint16_t )( ucCRCHi << 8 | ucCRCLo );
}


//标准帧滤波器, 扩展帧滤波器设置, 开启新消息接收中断, 开启Bus-Off中断, 设置发送传输延时补偿等
void fdcan_config(void)
{
	FDCAN_FilterTypeDef sFilterConfig1;
  sFilterConfig1.IdType = FDCAN_STANDARD_ID;
  sFilterConfig1.FilterIndex = 0;
  sFilterConfig1.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig1.FilterID1 = 0x000;
  sFilterConfig1.FilterID2 = 0x000;                        //BCMU只接收BSMU   BSMU全接收
  HAL_FDCAN_ConfigFilter(&Myfdcan, &sFilterConfig1);

//  sFilterConfig1.IdType = FDCAN_EXTENDED_ID;
//  sFilterConfig1.FilterIndex = 0;
//  sFilterConfig1.FilterType = FDCAN_FILTER_MASK;
//  sFilterConfig1.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
//  sFilterConfig1.FilterID1 = 0;
//  sFilterConfig1.FilterID2 = 0;
//  HAL_FDCAN_ConfigFilter(&Myfdcan, &sFilterConfig1);   
  
  /* Configure global filter to reject all non-matching frames */
	//下面这一句是配置全局滤波器配置寄存器的，一定要写，否则配置了也没用
  HAL_FDCAN_ConfigGlobalFilter(&Myfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  
  /* Configure Rx FIFO 0 watermark to 2 */
  //HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 2);

  /* Activate Rx FIFO 0 new message notification on both FDCAN instances */
  HAL_FDCAN_ActivateNotification(&Myfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  HAL_FDCAN_ActivateNotification(&Myfdcan, FDCAN_IT_BUS_OFF, 0);

  /* Configure and enable Tx Delay Compensation, required for BRS mode.
        TdcOffset default recommended value: DataTimeSeg1 * DataPrescaler
        TdcFilter default recommended value: 0 */
  HAL_FDCAN_ConfigTxDelayCompensation(&Myfdcan, Myfdcan.Init.DataPrescaler * Myfdcan.Init.DataTimeSeg1, 0);
  HAL_FDCAN_EnableTxDelayCompensation(&Myfdcan);

  HAL_FDCAN_Start(&Myfdcan);
}


//Bus-Off处理
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs)
{
  //__HAL_FDCAN_CLEAR_FLAG(hfdcan, FDCAN_FLAG_BUS_OFF);
  if(hfdcan->Instance == FDCAN1) {
//    MX_FDCAN1_Init();
//    fdcan_config();
  } else if(hfdcan->Instance == FDCAN2) {
    MX_FDCAN2_Init();
    fdcan_config();
  } else {
  }
}

//返回BCMU_Mail_t结构体
BCMU_Mail_t* ReturnBCMU_Mail(CANRxMsg_t* CANRxMsg){
	uint32_t StdId;
	StdId = CANRxMsg->RxHeader.Identifier;
	switch(StdId){																			
			case BCMU1_ID:				return (&BCMU[0]);																								
			case BCMU2_ID:				return (&BCMU[1]);																								
			case BCMU3_ID:				return (&BCMU[2]);																											
			case BCMU4_ID:				return (&BCMU[3]);																														
			case BCMU5_ID:				return (&BCMU[4]);						
			case BCMU6_ID:				return (&BCMU[5]);																									
			case BCMU7_ID:				return (&BCMU[6]);																									
			case BCMU8_ID:				return (&BCMU[7]);																												
			case BCMU9_ID:				return (&BCMU[8]);																															
			case BCMU10_ID:				return (&BCMU[9]);	
			case BCMU11_ID:				return (&BCMU[10]);																								
			case BCMU12_ID:				return (&BCMU[11]);																								
			case BCMU13_ID:				return (&BCMU[12]);																											
			case BCMU14_ID:				return (&BCMU[13]);																														
			case BCMU15_ID:				return (&BCMU[14]);			
			case BCMU16_ID:				return (&BCMU[15]);																									
			case BCMU17_ID:				return (&BCMU[16]);																									
			case BCMU18_ID:				return (&BCMU[17]);																												
			case BCMU19_ID:				return (&BCMU[18]);																															
			case BCMU20_ID:				return (&BCMU[19]);	
		
			default: 				return NULL;
	}
}


//FDCAN接收回调处理
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0) {   
    if (hfdcan->Instance == FDCAN1) {
//			HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &(RxMsg.RxHeader), RxMsg.payload);
//			for(uint8_t i=0; i<CAN_Rev_DLC; i++)
//				xQueueSendFromISR(CANQueueHandle, &RxMsg.payload[i], NULL);
    } 
		if (hfdcan->Instance == FDCAN2) {
			HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &(RxMsg.RxHeader), RxMsg.payload);
			BCMU_Mail_t* BCMUn= ReturnBCMU_Mail(&RxMsg);    //返回BCMU_Mail_t结构体
			if(BCMUn!=NULL){
			for(uint8_t i=0; i<CAN_Rev_DLC; i++)
				xQueueSendFromISR(*(BCMUn->BCMUQueue), &RxMsg.payload[i], NULL);    //进相应的消息队列
			}
    }
  }
}


//软件定时器回调
void CANTimerCallback01(void const * argument)
{
	if(1)
	{}
}

/***********************************************
*函数名称：CANFrameSend
*函数功能：CAN发送数据组包
*入口参数：NULL
*返回参数：-1：发送失败    >0:发送字节数
*说明：如果len = 0,pdata无效
*作用域：内部
***********************************************/
int8_t CANFrameSend(uint8_t cmd,uint8_t *pdata, uint8_t *msg, uint16_t len)
{
	uint16_t index = 0;
	uint16_t crc_temp = 0;
	uint16_t count = 0;
	
	msg[index++] = FRAME_HEAD1;
	msg[index++] = FRAME_HEAD2;
	msg[index++] = cmd;
	msg[index++] = len>>8 & 0x00ff;
	msg[index++] = len & 0x00ff;
	for(uint16_t i = 0;i<len;i++)
	{
	msg[index++] = pdata[i];
	}
	
	crc_temp = CRC16_modbus(msg, index);
	msg[index++] = crc_temp>>8 & 0x00FF;
	msg[index++] = crc_temp & 0x00FF;
	
	for(uint16_t i=0; i<index/CAN_Snd_DLC+1; i++){
		memcpy(TxMsg.payload, msg+i*CAN_Snd_DLC, CAN_Snd_DLC);
		while(HAL_FDCAN_GetTxFifoFreeLevel(&Myfdcan) < 3){
			count++;
			if(count>100)
				return -1;
		}//wait the FiFo free
		HAL_FDCAN_AddMessageToTxFifoQ(&Myfdcan, &TxMsg.TxHeader, TxMsg.payload);
		
		memset(TxMsg.payload, 0x00, sizeof(TxMsg.payload));
	}
	memset(msg, 0x00, FDCAN_SND_BuffSize);
	return  index;
}



//成功接收后处理函数
void CAN_DataHandle(uint8_t Queue_NUM_t, uint8_t cmd, void *data, uint16_t len)
{
	uint8_t Queue_NUM = Queue_NUM_t;
	
  	memcpy((void*)&Client_Sd[Queue_NUM], data, len);

}


//FDCAN接收 拼包+解包	
void CAN_Rev(void const * argument)
{
	BaseType_t xReturn = pdTRUE;/* 定义一个创建信息返回值，默认为pdTRUE */
	uint8_t data;
	uint16_t crc_temp = 0;
# if MasterOrSlave==Slave

#else
	static uint8_t frame_status[cluster_num];
	static uint16_t index[cluster_num];
	static uint8_t comand[cluster_num];
	static uint16_t len[cluster_num];
	static uint16_t crc[cluster_num];
	uint8_t Queue_NUM = 0;
	for(uint8_t i=0; i<cluster_num; i++){
		frame_status[i] = frame_head1status;
		index[i] = 0;
		len[i] = 0x00;
		crc[i] = 0x00;
		comand[i] = 0x00;
	}
#endif
  for(;;)
  {	
#if MasterOrSlave==Slave

#else
			for(Queue_NUM=0; Queue_NUM<cluster_num; Queue_NUM++){
				xReturn = xQueueReceive(CANQueueHandle[Queue_NUM], &data, 0);
			if(xReturn == pdTRUE){
				while(xReturn == pdTRUE){
						switch(frame_status[Queue_NUM]){
						case frame_head1status://判断数据头1
								if(data == FRAME_HEAD1){
								frame_status[Queue_NUM] = frame_head2status;
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								index[Queue_NUM]++;
							 }
								else{
								frame_status[Queue_NUM] = frame_head1status;
								index[Queue_NUM] = 0;
								len[Queue_NUM] = 0;
								memset(BCMU[Queue_NUM].Data_buf,0x00,FDCAN_REV_BuffSize);
							 }
						break;
						case frame_head2status://判断数据头2
							 if(data == FRAME_HEAD2)
							 {
								BCMU[Queue_NUM].Frame_Cnt++;//收到数据包数+1
								frame_status[Queue_NUM] = frame_cmdstatus;
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								index[Queue_NUM]++;
							 }
							 else
							 {
								frame_status[Queue_NUM] = frame_head1status;
								index[Queue_NUM] = 0;
								len[Queue_NUM] = 0;
								memset(BCMU[Queue_NUM].Data_buf,0x00,FDCAN_REV_BuffSize);
							 }
						break;
						case frame_cmdstatus://接收cmd
								frame_status[Queue_NUM] = frame_len1status;
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								comand[Queue_NUM] = data;
								index[Queue_NUM]++;
						break;
						case frame_len1status://记录len高8位
								frame_status[Queue_NUM] = frame_len2status;
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								len[Queue_NUM] |= (uint16_t)data<<8;
								index[Queue_NUM]++;
						break; 
						case frame_len2status://记录len低8位
								frame_status[Queue_NUM] = frame_datastatus;
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								len[Queue_NUM] |= (uint16_t)data;
								if(len[Queue_NUM]==0)
									frame_status[Queue_NUM] = frame_crc1status;
								index[Queue_NUM]++;
						break; 
						case frame_datastatus://接收数据段
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								index[Queue_NUM]++;
								if(index[Queue_NUM]==len[Queue_NUM]+5){
									frame_status[Queue_NUM] = frame_crc1status;
								}
						break; 
						case frame_crc1status://校验CRC 高8位
								frame_status[Queue_NUM] = frame_crc2status;
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								crc[Queue_NUM] |= (uint16_t)data<<8;
								index[Queue_NUM]++;
						break; 
						case frame_crc2status://校验CRC 低8位
								BCMU[Queue_NUM].Data_buf[index[Queue_NUM]] = data;
								crc[Queue_NUM] |= (uint16_t)data;
								index[Queue_NUM]++;
								//校验CRC是否一致
								crc_temp = CRC16_modbus(BCMU[Queue_NUM].Data_buf, index[Queue_NUM]-2);     
								if(crc_temp == crc[Queue_NUM]){
									 frame_status[Queue_NUM] = handle;
								}
								else{
									 frame_status[Queue_NUM] = frame_head1status;
									 memset(BCMU[Queue_NUM].Data_buf, 0x00, FDCAN_REV_BuffSize);
									 index[Queue_NUM] = 0;
									 len[Queue_NUM] = 0;
									 crc[Queue_NUM] = 0;
									 comand[Queue_NUM] = 0;
								}
						break;
						case handle://校验CRC 低8位
							 frame_status[Queue_NUM] = frame_head1status;
							 //成功接收后处理函数
							 CAN_DataHandle(Queue_NUM, comand[Queue_NUM], (void*)(FDCAN_REV_Buff[Queue_NUM]+5), len[Queue_NUM]);	
							 //有用包数+1
							 BCMU[Queue_NUM].EffectiveFrame_Cnt++;
							 memset(BCMU[Queue_NUM].Data_buf, 0x00, FDCAN_REV_BuffSize);    		
							 index[Queue_NUM] = 0;
							 len[Queue_NUM] = 0;
							 crc[Queue_NUM] = 0;
							 comand[Queue_NUM] = 0;
						break;
						default:
							 frame_status[Queue_NUM] = frame_head1status;
							 memset(BCMU[Queue_NUM].Data_buf, 0x00, FDCAN_REV_BuffSize);
							 index[Queue_NUM] = 0;
							 len[Queue_NUM] = 0;
							 crc[Queue_NUM] = 0;
							 comand[Queue_NUM] = 0;
						break;
					}
					xReturn = xQueueReceive(CANQueueHandle[Queue_NUM], &data, 0);//消息队列中还有数据，则继续出队
				}
				
				break;
			}	
			else{
				continue;
			}
		}
#endif
    osDelay(1);
  }
}

//从机在线离线判断
void SlaveOnlineCheck()
{
	static uint32_t Last_Cnt[cluster_num];
	uint32_t Curr_Cnt[cluster_num];
	for(uint8_t i=0; i<cluster_num; i++)
	{
		Curr_Cnt[i] = BCMU[i].Request_Cnt - BCMU[i].Frame_Cnt;
		if(Last_Cnt[i]==Curr_Cnt[i]){
			BCMU[i].OnlineOrOffline = Online;
		}
		else{
			BCMU[i].OnlineOrOffline = Offline;
			//BCMU工作状态
			Client_Sd[i].work_state = Offline;
		}
		Last_Cnt[i] = Curr_Cnt[i];
	}
}



//FDCAN轮询（BSMU）
void CAN_Poll(void const * argument)
{
	/* 配置发送参数 */
	TxMsg.TxHeader.Identifier = CAN_ID;                     /* 设置接收帧消息的ID 11位帧ID范围0x000-0x7ff*/
	TxMsg.TxHeader.IdType = FDCAN_STANDARD_ID;             	/* 标准ID */
	TxMsg.TxHeader.TxFrameType = FDCAN_DATA_FRAME;         	/* 数据帧 */
	TxMsg.TxHeader.DataLength = FDCAN_DLC_BYTES_64;      	/* 发送数据长度 */   //现在固定为64
	TxMsg.TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE; 	/* 设置错误状态指示 */
	TxMsg.TxHeader.BitRateSwitch = FDCAN_BRS_ON;           	/* 开启可变波特率 */
	TxMsg.TxHeader.FDFormat = FDCAN_FD_CAN;                	/* FDCAN格式 */
	TxMsg.TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;	/* 用于发送事件FIFO控制, 不存储 */
	TxMsg.TxHeader.MessageMarker = 0;    					/* 用于复制到TX EVENT FIFO的消息Maker来识别消息状态，范围0到0xFF */ 
//	osTimerStart(CANTimer01Handle, 1000);					//开启FREERTOS软件定时器once
  for(;;)
  {
	for(uint8_t Queue_NUM=0; Queue_NUM<cluster_num; Queue_NUM++){
		if(CANFrameSend(CMD_TRANS_START,(unsigned char*)&BCMU_ID[Queue_NUM], FDCAN_SND_Buff, 2)>0)
			BCMU[Queue_NUM].Request_Cnt++;
		else{
			printf("CAN SEND FAIL!\n");
			BCMU[Queue_NUM].Request_Cnt++;
		}
		vTaskDelay(bsmuSetting.poll_T*10);
	}
	//等待两个命令周期
	vTaskDelay(bsmuSetting.poll_T*10*2);
	SlaveOnlineCheck();//判断从机离线情况
	CalStationData();//计算整个电站数据
	xSemaphoreGive(ETHSndSemHandle);//释放信号量（用来发送以太网数据）
	xSemaphoreGive(ViewUpdateSemHandle);//释放信号量（用来更新LCD显示数据）

    osDelay(1);
  }
}




//创建FDCAN轮询线程和接收线程并开启FDCAN
void BSMU_CANInit()
{ 
#if MasterOrSlave==Master
	memset((void*)CANRevBuffStartAdd, 0x00, cluster_num*FDCAN_REV_BuffSize);
	memset((void*)CANSndBuffStartAdd, 0x00, FDCAN_SND_BuffSize);
#else
	memset((void*)FDCAN_REV_Buff, 0x00, FDCAN_REV_BuffSize);
	memset((void*)FDCAN_SND_Buff, 0x00, FDCAN_SND_BuffSize);
#endif
	
	//创建FREERTOS软件定时器
	osTimerDef(CANTimer01, CANTimerCallback01);
  	CANTimer01Handle = osTimerCreate(osTimer(CANTimer01), osTimerPeriodic, NULL);
	//创建CAN接收的消息队列

	osMessageQDef(CANQueue1, 5120, uint8_t);
  	CANQueueHandle[0] = osMessageCreate(osMessageQ(CANQueue1), NULL);
	
	osMessageQDef(CANQueue2, 5120, uint8_t);
  	CANQueueHandle[1] = osMessageCreate(osMessageQ(CANQueue2), NULL);
	
	osMessageQDef(CANQueue3, 5120, uint8_t);
  	CANQueueHandle[2] = osMessageCreate(osMessageQ(CANQueue3), NULL);
	
	osMessageQDef(CANQueue4, 5120, uint8_t);
  	CANQueueHandle[3] = osMessageCreate(osMessageQ(CANQueue4), NULL);
	
	osMessageQDef(CANQueue5, 5120, uint8_t);
  	CANQueueHandle[4] = osMessageCreate(osMessageQ(CANQueue5), NULL);
	
	osMessageQDef(CANQueue6, 5120, uint8_t);
  	CANQueueHandle[5] = osMessageCreate(osMessageQ(CANQueue6), NULL);
	
	osMessageQDef(CANQueue7, 5120, uint8_t);
  	CANQueueHandle[6] = osMessageCreate(osMessageQ(CANQueue7), NULL);
	
	osMessageQDef(CANQueue8, 5120, uint8_t);
  	CANQueueHandle[7] = osMessageCreate(osMessageQ(CANQueue8), NULL);
	
	osMessageQDef(CANQueue9, 5120, uint8_t);
  	CANQueueHandle[8] = osMessageCreate(osMessageQ(CANQueue9), NULL);
	
	osMessageQDef(CANQueue10, 5120, uint8_t);
  	CANQueueHandle[9] = osMessageCreate(osMessageQ(CANQueue10), NULL);
	
	osMessageQDef(CANQueue11, 5120, uint8_t);
  	CANQueueHandle[10] = osMessageCreate(osMessageQ(CANQueue11), NULL);
	
	osMessageQDef(CANQueue12, 5120, uint8_t);
  	CANQueueHandle[11] = osMessageCreate(osMessageQ(CANQueue12), NULL);
	
	osMessageQDef(CANQueue13, 5120, uint8_t);
  	CANQueueHandle[12] = osMessageCreate(osMessageQ(CANQueue13), NULL);
	
	osMessageQDef(CANQueue14, 5120, uint8_t);
  	CANQueueHandle[13] = osMessageCreate(osMessageQ(CANQueue14), NULL);
	
	osMessageQDef(CANQueue15, 5120, uint8_t);
  	CANQueueHandle[14] = osMessageCreate(osMessageQ(CANQueue15), NULL);
	
	osMessageQDef(CANQueue16, 5120, uint8_t);
  	CANQueueHandle[15] = osMessageCreate(osMessageQ(CANQueue16), NULL);
	
	osMessageQDef(CANQueue17, 5120, uint8_t);
  	CANQueueHandle[16] = osMessageCreate(osMessageQ(CANQueue17), NULL);
	
	osMessageQDef(CANQueue18, 5120, uint8_t);
  	CANQueueHandle[17] = osMessageCreate(osMessageQ(CANQueue18), NULL);
	
	osMessageQDef(CANQueue19, 5120, uint8_t);
  	CANQueueHandle[18] = osMessageCreate(osMessageQ(CANQueue19), NULL);
	
	osMessageQDef(CANQueue20, 5120, uint8_t);
  	CANQueueHandle[19] = osMessageCreate(osMessageQ(CANQueue20), NULL);
	
	for(uint8_t i=0; i<cluster_num; i++){
		BCMU[i].ID = BCMU_ID[i][0]<<8 | BCMU_ID[i][1];
		BCMU[i].Request_Cnt = 0;
		BCMU[i].Frame_Cnt = 0;
		BCMU[i].EffectiveFrame_Cnt = 0;
		BCMU[i].BCMUQueue = &CANQueueHandle[i];
		BCMU[i].Data_buf = FDCAN_REV_Buff[i];
		BCMU[i].OnlineOrOffline = Offline;
	}
	
	//创建一个二值信号量
	osSemaphoreDef(ETHSndSem);
	ETHSndSemHandle = osSemaphoreCreate(osSemaphore(ETHSndSem), 1);
	//创建一个二值信号量
	osSemaphoreDef(ViewUpdateSem);
	ViewUpdateSemHandle = osSemaphoreCreate(osSemaphore(ViewUpdateSem), 1);
	
	osThreadDef(CAN_Rev_Thread, CAN_Rev, osPriorityHigh, 0, 128);
	CAN_Rev_TaskHandle = osThreadCreate(osThread(CAN_Rev_Thread), NULL);
	
	osThreadDef(CAN_Poll_Thread, CAN_Poll, osPriorityNormal, 0, 128);
	CAN_Poll_TaskHandle = osThreadCreate(osThread(CAN_Poll_Thread), NULL);
	
	fdcan_config();            //这里配置CAN的筛选器和开启CAN
}

