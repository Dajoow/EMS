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
#define  CLU_CAP_KWH              (CELL_CAP_AH * 2 * 3600 / 1000)                        // 每簇容量 kWh

#define  MAX_ERROR                400

#define  CLU_CAP_FULL_SOC         90
#define  CLU_CAP_EMPTY_SOC        10

#define  MAX_ERROR                400

#define  FRAME_HEADER             0x01											 //TCP段中帧头(电池簇)
#define  FRAME_TAIL               0x0A0D0000 							  //TCP段中帧尾  

#define  FRAME_STATION_HEADER     0x02											 //TCP段中帧头(整个电站的帧头)

#define  CLIENT_SD_ERR_OFT        2908
#define  CLIENT_SD_TAIL_OFT       2912
#define  CLIENT_SD_TAIL_LEN       8

#define  MODBUS_STA_STATUS_PROHIBIT_CHARGE      0x1111
#define  MODBUS_STA_STATUS_PROHIBIT_DISCHARGE   0x2222
#define  MODBUS_STA_STATUS_ALARM                0x5555
#define  MODBUS_STA_STATUS_BREAK                0xAAAA
#define  MODBUS_STA_STATUS_NORMAL               0xBBBB
#define  MODBUS_STA_STATUS_WARN                 0xCCCC

#define  MODBUS_CLU_STATUS_OPEN                 1        
#define  MODBUS_CLU_STATUS_IDLE                 2        
#define  MODBUS_CLU_STATUS_CHARGE               3        
#define  MODBUS_CLU_STATUS_DISCHARGE            4        

#define  CLU_STATE_CHARGE                       1
#define  CLU_STATE_DISCHARGE                    2
#define  CLU_STATE_IDLE                         3
#define  CLU_STATE_BALANCE                      4
#define  CLU_STATE_OPEN_CUR                     5

#define CLU_CUR_UNIT                            0.01  // 10mA

#define CLU_CHARGE_STAGE_SW_VOL                 2.45  // <= 2.45
#define CLU_CHARGE_STAGE1_CUR                   175   // <= 2.45
#define CLU_CHARGE_STAGE2_CUR                   5     // > 2.45
#define CLU_DISCHARGE_STAGE1_CUR                165

// WARN < ALARM < PROTECT
#define CLU_HIGH_VOLT_WARN                      (modbus_threshold_flash.clu_high_volt_warn)
#define CLU_HIGH_VOLT_ALARM                     (modbus_threshold_flash.clu_high_volt_alarm)
#define CLU_HIGH_VOLT_PROTECT                   (modbus_threshold_flash.clu_high_volt_protect)
#define CLU_LOW_VOLT_WARN                       (modbus_threshold_flash.clu_low_volt_warn)
#define CLU_LOW_VOLT_ALARM                      (modbus_threshold_flash.clu_low_volt_alarm)
#define CLU_LOW_VOLT_PROTECT                    (modbus_threshold_flash.clu_low_volt_protect)
#define CLU_HIGH_CHARGE_CUR_WARN                (modbus_threshold_flash.clu_high_charge_cur_warn)
#define CLU_HIGH_CHARGE_CUR_ALARM               (modbus_threshold_flash.clu_high_charge_cur_alarm)
#define CLU_HIGH_CHARGE_CUR_PROTECT             (modbus_threshold_flash.clu_high_charge_cur_protect)
#define CLU_HIGH_DISCHARGE_CUR_WARN             (modbus_threshold_flash.clu_high_discharge_cur_warn)
#define CLU_HIGH_DISCHARGE_CUR_ALARM            (modbus_threshold_flash.clu_high_discharge_cur_alarm)
#define CLU_HIGH_DISCHARGE_CUR_PROTECT          (modbus_threshold_flash.clu_high_discharge_cur_protect)
#define CELL_HIGH_VOLT_WARN                     (modbus_threshold_flash.cell_high_volt_warn)
#define CELL_HIGH_VOLT_ALARM                    (modbus_threshold_flash.cell_high_volt_alarm)
#define CELL_HIGH_VOLT_PROTECT                  (modbus_threshold_flash.cell_high_volt_protect)
#define CELL_LOW_VOLT_WARN                      (modbus_threshold_flash.cell_low_volt_warn)
#define CELL_LOW_VOLT_ALARM                     (modbus_threshold_flash.cell_low_volt_alarm)
#define CELL_LOW_VOLT_PROTECT                   (modbus_threshold_flash.cell_low_volt_protect)
#define CLU_HIGH_CHARGE_TEMP_WARN               (modbus_threshold_flash.clu_high_charge_temp_warn)
#define CLU_HIGH_CHARGE_TEMP_ALARM              (modbus_threshold_flash.clu_high_charge_temp_alarm)
#define CLU_HIGH_CHARGE_TEMP_PROTECT            (modbus_threshold_flash.clu_high_charge_temp_protect)
#define CLU_LOW_CHARGE_TEMP_WARN                (modbus_threshold_flash.clu_low_charge_temp_warn)
#define CLU_LOW_CHARGE_TEMP_ALARM               (modbus_threshold_flash.clu_low_charge_temp_alarm)
#define CLU_LOW_CHARGE_TEMP_PROTECT             (modbus_threshold_flash.clu_low_charge_temp_protect)
#define CLU_HIGH_DISCHARGE_TEMP_WARN            (modbus_threshold_flash.clu_high_discharge_temp_warn)
#define CLU_HIGH_DISCHARGE_TEMP_ALARM           (modbus_threshold_flash.clu_high_discharge_temp_alarm)
#define CLU_HIGH_DISCHARGE_TEMP_PROTECT         (modbus_threshold_flash.clu_high_discharge_temp_protect)
#define CLU_LOW_DISCHARGE_TEMP_WARN             (modbus_threshold_flash.clu_low_discharge_temp_warn)
#define CLU_LOW_DISCHARGE_TEMP_ALARM            (modbus_threshold_flash.clu_low_discharge_temp_alarm)
#define CLU_LOW_DISCHARGE_TEMP_PROTECT          (modbus_threshold_flash.clu_low_discharge_temp_protect)
#define CELL_HIGH_TEMP_WARN                     (modbus_threshold_flash.cell_high_temp_warn)
#define CELL_HIGH_TEMP_ALARM                    (modbus_threshold_flash.cell_high_temp_alarm)
#define CELL_HIGH_TEMP_PROTECT                  (modbus_threshold_flash.cell_high_temp_protect)
#define CELL_LOW_TEMP_WARN                      (modbus_threshold_flash.cell_low_temp_warn)
#define CELL_LOW_TEMP_ALARM                     (modbus_threshold_flash.cell_low_temp_alarm)
#define CELL_LOW_TEMP_PROTECT                   (modbus_threshold_flash.cell_low_temp_protect)
#define ENV_HIGH_TEMP_WARN                      0
#define ENV_HIGH_TEMP_ALARM                     0
#define ENV_HIGH_TEMP_PROTECT                   0
#define ENV_LOW_TEMP_WARN                       0
#define ENV_LOW_TEMP_ALARM                      0
#define ENV_LOW_TEMP_PROTECT                    0
#define PWRLINE_HIGH_TEMP_WARN                  0
#define PWRLINE_HIGH_TEMP_ALARM                 0
#define PWRLINE_HIGH_TEMP_PROTECT               0
#define CLU_HIGH_SOC_WARN                       (modbus_threshold_flash.clu_high_soc_warn)
#define CLU_HIGH_SOC_ALARM                      (modbus_threshold_flash.clu_high_soc_alarm)
#define CLU_HIGH_SOC_PROTECT                    (modbus_threshold_flash.clu_high_soc_protect)
#define CLU_LOW_SOC_WARN                        (modbus_threshold_flash.clu_low_soc_warn)
#define CLU_LOW_SOC_ALARM                       (modbus_threshold_flash.clu_low_soc_alarm)
#define CLU_LOW_SOC_PROTECT                     (modbus_threshold_flash.clu_low_soc_protect)
#define CLU_INSUL_RES_P_LOW_WARN                (modbus_threshold_flash.clu_insul_res_p_low_warn)
#define CLU_INSUL_RES_P_LOW_ALARM               (modbus_threshold_flash.clu_insul_res_p_low_alarm)
#define CLU_INSUL_RES_P_LOW_PROTECT             (modbus_threshold_flash.clu_insul_res_p_low_protect)
#define CLU_INSUL_RES_N_LOW_WARN                (modbus_threshold_flash.clu_insul_res_n_low_warn)
#define CLU_INSUL_RES_N_LOW_ALARM               (modbus_threshold_flash.clu_insul_res_n_low_alarm)
#define CLU_INSUL_RES_N_LOW_PROTECT             (modbus_threshold_flash.clu_insul_res_n_low_protect)



//单簇数据包结构
//注意内存对齐   现在这个结构体的大小为4的倍数
typedef struct 
{
  /* data */
  uint16_t frame_header;
  uint16_t cluster_No;
  uint16_t work_state;
  uint16_t cluster_VOL; // 0.1V
  int16_t  cluster_CUR; // 10mA
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
  uint32_t bmu_sw_state; // offset: 2904
  // uint16_t BAT_FAULT[TOTOL_BAT_num];
  uint32_t error_count; // offset: 2908
  uint32_t checksum; // offset: 2912
  uint32_t frame_tail;
}Client_Sd_t;

typedef struct
{
  uint8_t error_id_l;
  uint8_t error_id_h;
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
