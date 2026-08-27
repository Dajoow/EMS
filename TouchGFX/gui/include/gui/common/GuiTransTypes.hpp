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

enum DeviceUiState
{
  DEVICE_UI_OFFLINE = 0,
  DEVICE_UI_STALE,
  DEVICE_UI_STOPPED,
  DEVICE_UI_RUNNING,
  DEVICE_UI_FAULT,
  DEVICE_UI_UNKNOWN
};

struct DeviceStatusData
{
  uint8_t acdcOnline;
  uint8_t acdcAlarm;
  uint32_t acdcAgeMs;
  float acdcDcVoltage;
  float acdcDcCurrent;
  float acdcAcVoltage;
  float acdcAcCurrent;
  uint16_t acdcFrequency;
  uint16_t acdcRectifierCount;
  uint32_t acdcRectifierPower;

  uint8_t dcdcState;
  uint16_t dcdcFaultRaw;
  uint32_t dcdcAgeMs;
  float dcdcBVoltage;
  float dcdcBCurrent;
  uint16_t dcdcBPower;
  float dcdcPVoltage;
  float dcdcPCurrent;
  uint16_t dcdcPPower;
  float dcdcMaxTemperature;

  uint8_t writeState;
  uint8_t writeAttempts;
  uint8_t readbackConfirmed;
  int16_t writeError;
  uint32_t writeSequence;

  DeviceStatusData()
    : acdcOnline(0), acdcAlarm(0), acdcAgeMs(0xFFFFFFFFUL),
      acdcDcVoltage(0), acdcDcCurrent(0), acdcAcVoltage(0), acdcAcCurrent(0),
      acdcFrequency(0), acdcRectifierCount(0), acdcRectifierPower(0),
      dcdcState(DEVICE_UI_OFFLINE), dcdcFaultRaw(0), dcdcAgeMs(0xFFFFFFFFUL),
      dcdcBVoltage(0), dcdcBCurrent(0), dcdcBPower(0),
      dcdcPVoltage(0), dcdcPCurrent(0), dcdcPPower(0), dcdcMaxTemperature(0),
      writeState(0), writeAttempts(0), readbackConfirmed(0), writeError(0), writeSequence(0) {}
};

//class err_info {
//public:
//	bool err_reflashFlag;
//	unsigned char err_num;
//	unsigned char err_type;
//
//	err_info(): err_reflashFlag(false), err_num(0), err_type(' ') {}
//};

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




