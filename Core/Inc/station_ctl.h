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
#define  TOTOL_BAT_num            (GRP_num*GRP_BAT_num)     //总电池数，为了内存对齐这个值为2的倍数
#define  CELL_CAP_AH              500
#define  CLU_CAP_KWH              (CELL_CAP_AH * TOTOL_BAT_num * 2 * 3600 / 1000)                        // 每簇容量 kWh
<<<<<<< HEAD

#define  MAX_ERROR                400
=======
>>>>>>> e1a5ace (facan feat: add cell SOH)

#define  CLU_CAP_FULL_SOC         90
#define  CLU_CAP_EMPTY_SOC        10

#define  MAX_ERROR                400

#define  FRAME_HEADER             0x01											 //TCP段中帧头(电池簇)
#define  FRAME_TAIL               0x0A0D0000 							  //TCP段中帧尾  

#define  FRAME_STATION_HEADER     0x02											 //TCP段中帧头(整个电站的帧头)

#define  CLIENT_SD_ERR_OFT        2904
#define  CLIENT_SD_TAIL_OFT       2908
#define  CLIENT_SD_TAIL_LEN       8

#define CLU_CHARGE_STAGE_SW_VOL       2.45   // <= 2.45
#define CLU_CHARGE_STAGE1_CUR         175   // <= 2.45
#define CLU_CHARGE_STAGE2_CUR         5     // > 2.45
#define CLU_DISCHARGE_STAGE1_CUR      165

// WARN < ALARM < PROTECT
#define CLU_HIGH_VOLT_WARN                      (2.45*GRP_num*GRP_BAT_num)       
#define CLU_HIGH_VOLT_ALARM                     (2.478*GRP_num*GRP_BAT_num)       
#define CLU_HIGH_VOLT_PROTECT                   0xffff
#define CLU_LOW_VOLT_WARN                       (1.86*GRP_num*GRP_BAT_num)      
#define CLU_LOW_VOLT_ALARM                      (1.82*GRP_num*GRP_BAT_num)      
#define CLU_LOW_VOLT_PROTECT                    0
#define CLU_HIGH_CHARGE_CUR_WARN                0xffff
#define CLU_HIGH_CHARGE_CUR_ALARM               0xffff
#define CLU_HIGH_CHARGE_CUR_PROTECT             0xffff
#define CLU_HIGH_DISCHARGE_CUR_WARN             0xffff
#define CLU_HIGH_DISCHARGE_CUR_ALARM            0xffff
#define CLU_HIGH_DISCHARGE_CUR_PROTECT          0xffff
#define CELL_HIGH_VOLT_WARN                     2.45
#define CELL_HIGH_VOLT_ALARM                    2.48
#define CELL_HIGH_VOLT_PROTECT                  0xffff
#define CELL_LOW_VOLT_WARN                      1.85
#define CELL_LOW_VOLT_ALARM                     1.83
#define CELL_LOW_VOLT_PROTECT                   1.5
#define CLU_HIGH_CHARGE_TEMP_WARN               43
#define CLU_HIGH_CHARGE_TEMP_ALARM              46
#define CLU_HIGH_CHARGE_TEMP_PROTECT            50
#define CLU_LOW_CHARGE_TEMP_WARN                10
#define CLU_LOW_CHARGE_TEMP_ALARM               5
#define CLU_LOW_CHARGE_TEMP_PROTECT             0
#define CLU_HIGH_DISCHARGE_TEMP_WARN            43
#define CLU_HIGH_DISCHARGE_TEMP_ALARM           46
#define CLU_HIGH_DISCHARGE_TEMP_PROTECT         50
#define CLU_LOW_DISCHARGE_TEMP_WARN             10
#define CLU_LOW_DISCHARGE_TEMP_ALARM            5
#define CLU_LOW_DISCHARGE_TEMP_PROTECT          0
#define CELL_HIGH_TEMP_WARN                     43
#define CELL_HIGH_TEMP_ALARM                    46
#define CELL_HIGH_TEMP_PROTECT                  50
#define CELL_LOW_TEMP_WARN                      10
#define CELL_LOW_TEMP_ALARM                     5
#define CELL_LOW_TEMP_PROTECT                   0
#define ENV_HIGH_TEMP_WARN                      0xffff
#define ENV_HIGH_TEMP_ALARM                     0xffff
#define ENV_HIGH_TEMP_PROTECT                   0xffff
#define ENV_LOW_TEMP_WARN                       0
#define ENV_LOW_TEMP_ALARM                      0
#define ENV_LOW_TEMP_PROTECT                    0
#define PWRLINE_HIGH_TEMP_WARN                  0xffff
#define PWRLINE_HIGH_TEMP_ALARM                 0xffff
#define PWRLINE_HIGH_TEMP_PROTECT               0xffff
#define CLU_HIGH_SOC_WARN                       93
#define CLU_HIGH_SOC_ALARM                      96
#define CLU_HIGH_SOC_PROTECT                    99
#define CLU_LOW_SOC_WARN                        7
#define CLU_LOW_SOC_ALARM                       4
#define CLU_LOW_SOC_PROTECT                     1
#define CLU_INSUL_RES_P_LOW_WARN                0
#define CLU_INSUL_RES_P_LOW_ALARM               0
#define CLU_INSUL_RES_P_LOW_PROTECT             0
#define CLU_INSUL_RES_N_LOW_WARN                0
#define CLU_INSUL_RES_N_LOW_ALARM               0
#define CLU_INSUL_RES_N_LOW_PROTECT             0


//单簇数据包结构
//注意内存对齐   现在这个结构体的大小为4的倍数
typedef struct 
{
  /* data */
  uint16_t frame_header;
  uint16_t cluster_No;
  uint16_t work_state;
  uint16_t cluster_VOL; // 0.1V
  int16_t  cluster_CUR; // 200mA
  uint16_t cluster_SOC; // 0.1%
  uint16_t cluster_SOH; // 0.1%
  uint16_t insulation_res_p; // 1k
  uint16_t insulation_res_n; // 1k
  uint8_t  grp_num;
  uint8_t  grp_bat_num; // offset: 19
  uint32_t bal_state; // [29:0] grp30-grp1 balance state offset:20
  uint16_t BAT_VOL[TOTOL_BAT_num]; // 0.1mV offset: 24
  uint16_t BAT_TMP[TOTOL_BAT_num]; // 0.01°C offset: 744
  uint16_t BAT_SOC[TOTOL_BAT_num]; // 0.1% offset: 1464
  uint16_t BAT_SOH[TOTOL_BAT_num]; // 0.1% offset: 2184
  // uint16_t BAT_FAULT[TOTOL_BAT_num];
  uint32_t error_count; // offset: 2904
  uint32_t checksum; // offset: 2908
  uint32_t frame_tail;
}Client_Sd_t;

typedef struct
{
  uint8_t error_id_h;
  uint8_t error_id_l;
  uint16_t error_code;
} error_info_t;

extern uint8_t bmu_offline[cluster_num][GRP_num];

//电站数据包结构
//注意内存对齐   现在这个结构体的大小为4的倍数
typedef struct 
{
  /* data */
  uint16_t frame_header;
  uint16_t station_state;
  uint16_t station_VOL;
  int16_t  station_CUR;
  uint16_t station_SOC; // 0.1%
  uint16_t station_SOH; // 0.1%
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
void cal_modbus_data(void);

#endif
