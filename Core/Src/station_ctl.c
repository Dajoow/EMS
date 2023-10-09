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

#include "station_ctl.h"
#include "CAN_Control.h"
#include "main.h"
#include "string.h"
#include "modbus_data.h"

extern BCMU_Mail_t BCMU[cluster_num];

#if defined(__CC_ARM)
// 这20个结构体，包括其中的成员符合内存对齐且没有成员之间的无效值填充
Client_Sd_t Client_Sd[cluster_num] __attribute__ ((at (ETHSendBuffAdd)));
// 电站整理数据
Client_Sd_Station_t Client_Sd_Station __attribute__ ((at (StationBuffAdd)));
#elif defined(__GNUC__)
// 这20个结构体，包括其中的成员符合内存对齐且没有成员之间的无效值填充
Client_Sd_t Client_Sd[cluster_num] __attribute__ ((section (".ETHSendBuff")));
// 电站整理数据
Client_Sd_Station_t Client_Sd_Station
    __attribute__ ((section (".StationBuff")));
#endif

uint8_t bmu_offline[cluster_num][GRP_num];
error_info_t Client_errors[cluster_num][MAX_ERROR];

// 硬件到UI数据
ModelToViewData modelToViewData;

// 32位累加取反
uint32_t
TX_CheckSum (uint32_t *buf, uint16_t len) // buf为数组，len为数组长度
{
  uint32_t i, ret = 0;
  for (i = 0; i < len; i++)
    ret += *(buf + i);
  ret = ~ret;
  return ret;
}

// 初始化电站所有数据
void
StationDataInit (void)
{
#if use_test_data
  for (uint8_t i = 0; i < cluster_num; i++)
    {
      Client_Sd[i].frame_header = FRAME_HEADER; // 帧头
      Client_Sd[i].cluster_No = i + 1;          // 簇编号
      Client_Sd[i].work_state = 0x00;           // 工作状态
      Client_Sd[i].cluster_VOL = 6000;          // 簇电压
      Client_Sd[i].cluster_CUR = 100;
      Client_Sd[i].cluster_SOC = 900;
      Client_Sd[i].cluster_SOH = 900;
      Client_Sd[i].insulation_res = 1000; // 绝缘电阻
      Client_Sd[i].grp_num = GRP_num;
      Client_Sd[i].grp_bat_num = GRP_BAT_num;

      for (uint16_t j = 0; j < TOTOL_BAT_num; j++)
        {
          Client_Sd[i].BAT_VOL[j] = 200;
        }
      for (uint16_t j = 0; j < TOTOL_BAT_num; j++)
        {
          Client_Sd[i].BAT_TMP[j] = 250;
        }
      for (uint16_t j = 0; j < TOTOL_BAT_num; j++)
        {
          Client_Sd[i].BAT_SOC[j] = 900;
        }
      for (uint16_t j = 0; j < TOTOL_BAT_num; j++)
        {
          Client_Sd[i].BAT_FAULT[j] = 0;
        }
      Client_Sd[i].checksum = TX_CheckSum ((uint32_t *)&Client_Sd[i],
                                           (sizeof (Client_Sd_t) - 8) / 0x04);
      Client_Sd[i].frame_tail = FRAME_TAIL; //\r\n
    }
#else
  memset (Client_Sd, 0x00, sizeof (Client_Sd_t) * cluster_num);
  memset (&Client_Sd_Station, 0x00, sizeof (Client_Sd_Station_t));
#endif
}

// todo
void
CalStationData (void)
{
  uint8_t OnlineNUM = 0;

  Client_Sd_Station.frame_header = FRAME_STATION_HEADER;
  Client_Sd_Station.station_state = 20;

  // cal station voltage, current, SOC, SOH
  Client_Sd_Station.station_VOL = 0;
  Client_Sd_Station.station_CUR = 0;
  for (uint8_t i = 0; i < cluster_num; i++)
    {
      if (BCMU[i].OnlineOrOffline == Online)
        {
          OnlineNUM++;
          Client_Sd_Station.station_VOL += Client_Sd[i].cluster_VOL;
          Client_Sd_Station.station_CUR += Client_Sd[i].cluster_CUR;

          Client_Sd_Station.station_SOC += Client_Sd[i].cluster_SOC;
          Client_Sd_Station.station_SOH += Client_Sd[i].cluster_SOH;
        }
    }
  Client_Sd_Station.station_VOL /= OnlineNUM;
  Client_Sd_Station.station_CUR /= OnlineNUM;
  Client_Sd_Station.station_SOC /= OnlineNUM;
  Client_Sd_Station.station_SOH /= OnlineNUM;

  Client_Sd_Station.charge_power = 0;
  Client_Sd_Station.discharge_power = 0;
  for (uint8_t i = 0; i < cluster_num; i++)
  {
    if (BCMU[i].OnlineOrOffline == Online)
      {
      if(Client_Sd[i].cluster_CUR < 0){
        float cur_tmp = -Client_Sd[i].cluster_CUR * 0.2; // 200mA -> 1A
        float vol_tmp = Client_Sd[i].cluster_VOL * 0.1; // 0.1V -> 1V

        Client_Sd_Station.discharge_power += cur_tmp * vol_tmp;
      }else{
        float cur_tmp = Client_Sd[i].cluster_CUR * 0.2; // 200mA -> 1A
        float vol_tmp = Client_Sd[i].cluster_VOL * 0.1; // 0.1V -> 1V

        Client_Sd_Station.discharge_power += cur_tmp * vol_tmp;
      }
		}
	}

  Client_Sd_Station.checksum = TX_CheckSum (
      (uint32_t *)&Client_Sd_Station, sizeof (Client_Sd_Station_t) / 4 - 2);
  Client_Sd_Station.frame_tail = FRAME_TAIL;
}

typedef struct {
  uint16_t cluster_id;
  uint16_t cell_id;
  uint16_t val;
} cell_info_t;

cell_info_t cell_max_vol[20];
cell_info_t cell_min_vol[20];
cell_info_t cell_max_temp[20];
cell_info_t cell_min_temp[20];
cell_info_t cell_max_soc[20];
cell_info_t cell_min_soc[20];
uint32_t cell_avg_vol[20];
uint32_t cell_avg_temp[20];
uint32_t cell_avg_soc[20];

extern cluster_info_u16_u cluster_info_u16[20];
extern cluster_info_f32_u cluster_info_f32[20];

extern modbus_float_u cell_vol[20][360];
extern modbus_float_u cell_temp[20][360];
extern modbus_float_u cell_soc[20][360];

void cal_modbus_cluster_data (void){

  for (int i = 0; i < cluster_num; i++){
    cell_max_vol[i].val = 0;
    cell_max_temp[i].val = 0;
    cell_max_soc[i].val = 0;
    cell_min_vol[i].val = 0xffff;
    cell_min_temp[i].val = 0xffff;
    cell_min_soc[i].val = 0xffff;
    cell_avg_vol[i] = 0;
    cell_avg_temp[i] = 0;
    cell_avg_soc[i] = 0;

    int cell_cnt = 0;

    if (BCMU[i].OnlineOrOffline == Offline) continue;
    for (int j = 0; j < GRP_num; j++)
    {
      if (bmu_offline[i][j]) continue;
      for (int k = 0; k < GRP_BAT_num; k++){
        int index = j * GRP_BAT_num + k;

        cell_vol[i][index].f32 = Client_Sd[i].BAT_VOL[index] * 0.001;
        cell_temp[i][index].f32 = Client_Sd[i].BAT_TMP[index] * 0.01;
        cell_soc[i][index].f32 = Client_Sd[i].BAT_SOC[index] * 0.1;

        if (Client_Sd[i].BAT_VOL[index] > cell_max_vol->val){
            cell_max_vol[i].cluster_id = i + 1;
            cell_max_vol[i].cell_id = index + 1;
            cell_max_vol[i].val = Client_Sd[i].BAT_VOL[index];
        }
        if (Client_Sd[i].BAT_TMP[index] > cell_max_temp->val){
            cell_max_temp[i].cluster_id = i + 1;
            cell_max_temp[i].cell_id = index + 1;
            cell_max_temp[i].val = Client_Sd[i].BAT_TMP[index];
        }
        if (Client_Sd[i].BAT_SOC[index] > cell_max_soc->val){
            cell_max_soc[i].cluster_id = i + 1;
            cell_max_soc[i].cell_id = index + 1;
            cell_max_soc[i].val = Client_Sd[i].BAT_SOC[index];
        }

        if (Client_Sd[i].BAT_VOL[index] < cell_min_vol->val){
            cell_min_vol[i].cluster_id = i + 1;
            cell_min_vol[i].cell_id = index + 1;
            cell_min_vol[i].val = Client_Sd[i].BAT_VOL[index];
        }
        if (Client_Sd[i].BAT_TMP[index] < cell_min_temp->val){
            cell_min_temp[i].cluster_id = i + 1;
            cell_min_temp[i].cell_id = index + 1;
            cell_min_temp[i].val = Client_Sd[i].BAT_TMP[index];
        }
        if (Client_Sd[i].BAT_SOC[index] < cell_min_soc->val){
            cell_min_soc[i].cluster_id = i + 1;
            cell_min_soc[i].cell_id = index + 1;
            cell_min_soc[i].val = Client_Sd[i].BAT_SOC[index];
        }

        cell_avg_vol[i] += Client_Sd[i].BAT_VOL[index];
        cell_avg_temp[i] += Client_Sd[i].BAT_TMP[index];
        cell_avg_soc[i] += Client_Sd[i].BAT_SOC[index];
        cell_cnt++;
      }
    }

    cell_avg_vol[i] /= cell_cnt;
    cell_avg_temp[i] /= cell_cnt;
    cell_avg_soc[i] /= cell_cnt;

    cluster_info_u16[i].data.max_vol_cell = cell_max_vol[i].cell_id;
    cluster_info_u16[i].data.min_vol_cell = cell_min_vol[i].cell_id;
    cluster_info_u16[i].data.max_temp_cell = cell_max_temp[i].cell_id;
    cluster_info_u16[i].data.min_temp_cell = cell_min_temp[i].cell_id;
    cluster_info_u16[i].data.max_soc_cell = cell_max_soc[i].cell_id;
    cluster_info_u16[i].data.min_soc_cell = cell_min_soc[i].cell_id;

    cluster_info_f32[i].data.voltage_delta_max = (cell_max_vol[i].val - cell_min_vol[i].val) * 0.001;
    cluster_info_f32[i].data.avg_cell_voltage = cell_avg_vol[i] * 0.001;
    cluster_info_f32[i].data.max_cell_voltage = cell_max_vol[i].val * 0.001;
    cluster_info_f32[i].data.min_cell_voltage = cell_min_vol[i].val * 0.001;

    cluster_info_f32[i].data.temperature_delta_max
        = (cell_max_temp[i].val - cell_min_temp[i].val) * 0.01;
    cluster_info_f32[i].data.avg_cell_temperature = cell_avg_temp[i] * 0.01;
    cluster_info_f32[i].data.max_cell_temperature = cell_max_temp[i].val * 0.01;
    cluster_info_f32[i].data.min_cell_temperature = cell_min_temp[i].val * 0.01;

    cluster_info_f32[i].data.avg_cell_soc = cell_avg_soc[i] * 0.1;
    cluster_info_f32[i].data.max_cell_soc = cell_max_soc[i].val * 0.1;
    cluster_info_f32[i].data.min_cell_soc = cell_min_soc[i].val * 0.1;

    cluster_info_f32[i].data.cluster_voltage = Client_Sd[i].cluster_VOL * 0.1;
    cluster_info_f32[i].data.cluster_current = Client_Sd[i].cluster_CUR * 0.2;
    cluster_info_f32[i].data.cluster_soc = Client_Sd[i].cluster_SOC * 0.1;
    cluster_info_f32[i].data.cluster_soh = Client_Sd[i].cluster_SOH * 0.1;

    cluster_info_f32[i].data.pos_insulation_res
        = Client_Sd[i].insulation_res_p / 1000; // Mohm
    cluster_info_f32[i].data.neg_insulation_res
        = Client_Sd[i].insulation_res_n / 1000; // Mohm
  }
}

void cal_modbus_sta_data (void){

}

void cal_modbus_data(void){

}
