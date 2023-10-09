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
#include "station_ctl.h"
#include "string.h"
#include "CAN_Control.h"

extern BCMU_Mail_t BCMU[cluster_num];

#if defined(__CC_ARM)
//这20个结构体，包括其中的成员符合内存对齐且没有成员之间的无效值填充
Client_Sd_t Client_Sd[cluster_num] __attribute__((at(ETHSendBuffAdd))); 
//电站整理数据
Client_Sd_Station_t Client_Sd_Station __attribute__((at(StationBuffAdd)));
#elif  defined(__GNUC__)
//这20个结构体，包括其中的成员符合内存对齐且没有成员之间的无效值填充
Client_Sd_t Client_Sd[cluster_num] __attribute__((section(".ETHSendBuff")));
//电站整理数据
Client_Sd_Station_t Client_Sd_Station __attribute__((section(".StationBuff")));
#endif

error_info_t Client_errors[cluster_num][MAX_ERROR];

//硬件到UI数据
ModelToViewData modelToViewData;

//32位累加取反
uint32_t TX_CheckSum(uint32_t *buf,uint16_t len)//buf为数组，len为数组长度
{
	uint32_t i,ret = 0;
	for(i=0; i<len; i++)
		ret +=*(buf+i);
	ret = ~ret;
	return ret;
}

//初始化电站所有数据
void StationDataInit(void){
#if use_test_data
	for(uint8_t i=0;i<cluster_num;i++)
	{
		Client_Sd[i].frame_header = FRAME_HEADER;       //帧头
		Client_Sd[i].cluster_No = i+1;          //簇编号
		Client_Sd[i].work_state = 0x00;				  //工作状态
		Client_Sd[i].cluster_VOL = 6000;        //簇电压   
		Client_Sd[i].cluster_CUR = 100;
		Client_Sd[i].cluster_SOC = 900;
		Client_Sd[i].cluster_SOH = 900;
		Client_Sd[i].insulation_res = 1000;     //绝缘电阻
		Client_Sd[i].grp_num = GRP_num;
		Client_Sd[i].grp_bat_num = GRP_BAT_num;
		
		for(uint16_t j=0;j<TOTOL_BAT_num;j++)
		{
			Client_Sd[i].BAT_VOL[j] = 200;
		}
		for(uint16_t j=0;j<TOTOL_BAT_num;j++)
		{
			Client_Sd[i].BAT_TMP[j] = 250;
		}
		for(uint16_t j=0;j<TOTOL_BAT_num;j++)
		{
			Client_Sd[i].BAT_SOC[j] = 900;
		}
		for(uint16_t j=0;j<TOTOL_BAT_num;j++)
		{
			Client_Sd[i].BAT_FAULT[j] = 0;
		}
		Client_Sd[i].checksum = TX_CheckSum((uint32_t*)&Client_Sd[i], (sizeof(Client_Sd_t)-8)/0x04);
	  Client_Sd[i].frame_tail = FRAME_TAIL;         //\r\n
	}
#else
	memset(Client_Sd, 0x00, sizeof(Client_Sd_t)*cluster_num);
    memset(&Client_Sd_Station, 0x00, sizeof(Client_Sd_Station_t));
#endif
}
	
// todo
void CalStationData(void){
  	uint8_t OnlineNUM = 0;

	Client_Sd_Station.frame_header = FRAME_STATION_HEADER;
	Client_Sd_Station.station_state = 20;

	Client_Sd_Station.station_VOL = 0;
	Client_Sd_Station.station_CUR = 0;
	for(uint8_t i=0; i < cluster_num; i++){
		if(BCMU[i].OnlineOrOffline == Online){
			OnlineNUM++;
			Client_Sd_Station.station_VOL += Client_Sd[i].cluster_VOL;
			Client_Sd_Station.station_CUR += Client_Sd[i].cluster_CUR;
		}
	}
	Client_Sd_Station.station_VOL /= OnlineNUM;
	Client_Sd_Station.station_CUR /= OnlineNUM;

	Client_Sd_Station.station_SOC = 900;
	Client_Sd_Station.station_SOH = 900;
	Client_Sd_Station.charge_power = 3000;
	Client_Sd_Station.discharge_power = 2500;
	Client_Sd_Station.checksum = TX_CheckSum((uint32_t *)&Client_Sd_Station, sizeof(Client_Sd_Station_t)/4 - 2);
	Client_Sd_Station.frame_tail = FRAME_TAIL;
}
	