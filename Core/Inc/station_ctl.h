/**
  ******************************************************************************
  * File Name          : station_ctl.h
  * Description        : calculate total station information 
  ******************************************************************************
  * @attention
  *
  *
  * @author:yhb
  *
  *
  ******************************************************************************
  */

#ifndef STATION_CTL_H
#define STATION_CTL_H

#include "stdint.h"

#define use_test_data             0                         //测试状态（发送模拟数据）

#define  cluster_num              20                        //电池簇个数
#define  GRP_num                  30												//一簇中电池组数
#define  GRP_BAT_num              12												//一组中电池数
#define  TOTOL_BAT_num            GRP_num*GRP_BAT_num       //总电池数，为了内存对齐这个值为2的倍数

#define  FRAME_HEADER             0x01											 //TCP段中帧头(电池簇)
#define  FRAME_TAIL               0x0A0D0000 							  //TCP段中帧尾  

#define  FRAME_STATION_HEADER     0x02											 //TCP段中帧头(整个电站的帧头)


//单簇数据包结构
//注意内存对齐   现在这个结构体的大小为4的倍数
typedef struct 
{
  /* data */
  uint16_t frame_header;
  uint16_t cluster_No;
  uint16_t work_state;
  uint16_t cluster_VOL;
  int16_t  cluster_CUR;
  uint16_t cluster_SOC;
  uint16_t cluster_SOH;
  uint16_t insulation_res_p;
  uint16_t insulation_res_n;
  uint8_t  grp_num;
  uint8_t  grp_bat_num;
  uint16_t BAT_VOL[TOTOL_BAT_num];
  uint16_t BAT_TMP[TOTOL_BAT_num];
  uint16_t BAT_SOC[TOTOL_BAT_num];
  uint16_t BAT_FAULT[TOTOL_BAT_num];
	uint32_t checksum;
  uint32_t frame_tail;
}Client_Sd_t;

//电站数据包结构
//注意内存对齐   现在这个结构体的大小为4的倍数
typedef struct 
{
  /* data */
  uint16_t frame_header;
  uint16_t station_state;
  uint16_t station_VOL;
  int16_t  station_CUR;
  uint16_t station_SOC;
  uint16_t station_SOH;
  uint32_t charge_power;
  uint32_t discharge_power;
	uint32_t checksum;
  uint32_t frame_tail;
}Client_Sd_Station_t;


//硬件到UI数据
typedef struct{
  uint8_t frameRate;
  uint8_t frameRateCount;
  //电站数据
  uint32_t station_state;
  uint16_t station_VOL;
  int16_t  station_CUR;
  uint16_t station_SOC;
  uint16_t station_SOH;
  uint32_t charge_power;
  uint32_t discharge_power;
  //20个BCMU状态
  uint8_t  BCMU_state[20];     //BCMU的在线离线情况 1为online
  //具体一个BCMU数据
  uint16_t cluster_VOL;
  int16_t  cluster_CUR;
  uint16_t cluster_SOC;
  uint16_t cluster_SOH;
  uint16_t insulation_res_p;
  uint16_t insulation_res_n;
  //具体一组电池信息
  uint16_t BAT_VOL[12];
  uint16_t BAT_TMP[12];
  uint16_t BAT_SOC[12];
  uint16_t BAT_FAULT[12];
}ModelToViewData;

extern Client_Sd_t Client_Sd[cluster_num];
extern Client_Sd_Station_t Client_Sd_Station;
extern ModelToViewData modelToViewData;

void StationDataInit(void);
void CalStationData(void);
#endif




