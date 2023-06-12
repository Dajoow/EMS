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

#ifndef GUITRANSTYPES_HPP
#define GUITRANSTYPES_HPP

#include "stdint.h"

//UI到硬件数据
class ViewToModelData{
public:
  bool reflashFlag;
  uint8_t BCMU_SEL;  //UI中选择的BCMU
  uint8_t BMU_SEL;   //UI中选择的BCMU

  ViewToModelData(): reflashFlag(false), BCMU_SEL(1), BMU_SEL(1){}
};


//写在station_ctl.h中
// //硬件到UI数据
// struct ModelToViewData{
//   uint8_t frameRate;
//   //电站数据
//   uint32_t station_state;
//   uint16_t station_VOL;
//   int16_t  station_CUR;
//   uint16_t station_SOC;
//   uint16_t station_SOH;
//   uint32_t charge_power;
//   uint32_t discharge_power;
//   //20个BCMU状态
//   uint8_t  BCMU_state[20];     //BCMU的在线离线情况 1为online
//   //具体一个BCMU数据
//   uint16_t cluster_VOL;
//   int16_t  cluster_CUR;
//   uint16_t cluster_SOC;
//   uint16_t cluster_SOH;
//   uint16_t insulation_res;
//   //具体一组电池信息
//   uint16_t BAT_VOL[12];
//   uint16_t BAT_TMP[12];
//   uint16_t BAT_SOC[12];
//   uint16_t BAT_FAULT[12];
// };


#endif




