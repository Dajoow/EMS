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
#include "at24cxx.h"
#include "data_persistence.h"
#include "main.h"
#include "modbus_data.h"
#include "string.h"

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
Client_Sd_Station_t Client_Sd_Station __attribute__ ((section (".StationBuff")));
#endif

uint8_t bmu_offline[cluster_num][GRP_num];
error_info_t Client_errors[cluster_num][MAX_ERROR];
extern cluster_flash_t cluster_flash[cluster_num];
extern modbus_threshold_flash_t modbus_threshold_flash;

// 硬件到UI数据
ModelToViewData modelToViewData;

// 32位累加取反
uint32_t
TX_CheckSum (uint32_t *buf, uint16_t len) // buf为数组，len为数组长度
{
    uint32_t i, ret = 0;
    for (i = 0; i < len; i++) ret += *(buf + i);
    ret = ~ret;
    return ret;
}

// 初始化电站所有数据
void
StationDataInit (void)
{
#if use_test_data
    for (uint8_t i = 0; i < cluster_num; i++) {
        Client_Sd[i].frame_header   = FRAME_HEADER; // 帧头
        Client_Sd[i].cluster_No     = i + 1;        // 簇编号
        Client_Sd[i].work_state     = 0x00;         // 工作状态
        Client_Sd[i].cluster_VOL    = 6000;         // 簇电压
        Client_Sd[i].cluster_CUR    = 100;
        Client_Sd[i].cluster_SOC    = 900;
        Client_Sd[i].cluster_SOH    = 900;
        Client_Sd[i].insulation_res = 1000; // 绝缘电阻
        Client_Sd[i].grp_num        = GRP_num;
        Client_Sd[i].grp_bat_num    = GRP_BAT_num;

        for (uint16_t j = 0; j < TOTOL_BAT_num; j++) { Client_Sd[i].BAT_VOL[j] = 200; }
        for (uint16_t j = 0; j < TOTOL_BAT_num; j++) { Client_Sd[i].BAT_TMP[j] = 250; }
        for (uint16_t j = 0; j < TOTOL_BAT_num; j++) { Client_Sd[i].BAT_SOC[j] = 900; }
        for (uint16_t j = 0; j < TOTOL_BAT_num; j++) { Client_Sd[i].BAT_FAULT[j] = 0; }
        Client_Sd[i].checksum   = TX_CheckSum ((uint32_t *)&Client_Sd[i], (sizeof (Client_Sd_t) - 8) / 0x04);
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

    // cal station voltage, current, SOC, SOH
    Client_Sd_Station.station_VOL = 0;
    Client_Sd_Station.station_CUR = 0;
    Client_Sd_Station.station_SOC = 0;
    Client_Sd_Station.station_SOH = 0;
    for (uint8_t i = 0; i < cluster_num; i++) {
        if (BCMU[i].OnlineOrOffline == Online) {
            OnlineNUM++;
            Client_Sd_Station.station_VOL += Client_Sd[i].cluster_VOL;
            Client_Sd_Station.station_CUR += Client_Sd[i].cluster_CUR;

            Client_Sd_Station.station_SOC += Client_Sd[i].cluster_SOC;
            Client_Sd_Station.station_SOH += Client_Sd[i].cluster_SOH;
        }
    }

    if (OnlineNUM > 0) {
        Client_Sd_Station.station_VOL /= OnlineNUM;
        Client_Sd_Station.station_CUR /= OnlineNUM;
        Client_Sd_Station.station_SOC /= OnlineNUM;
        Client_Sd_Station.station_SOH /= OnlineNUM;
    }

    Client_Sd_Station.charge_power    = 0;
    Client_Sd_Station.discharge_power = 0;
    for (uint8_t i = 0; i < cluster_num; i++) {
        if (BCMU[i].OnlineOrOffline == Online) {
            if (Client_Sd[i].cluster_CUR < 0) {
                float cur_tmp = -Client_Sd[i].cluster_CUR * CLU_CUR_UNIT; // 10mA -> 1A
                float vol_tmp = Client_Sd[i].cluster_VOL * 0.1;           // 0.1V -> 1V

                Client_Sd_Station.discharge_power += cur_tmp * vol_tmp;
            } else {
                float cur_tmp = Client_Sd[i].cluster_CUR * CLU_CUR_UNIT; // 10mA -> 1A
                float vol_tmp = Client_Sd[i].cluster_VOL * 0.1;          // 0.1V -> 1V

                Client_Sd_Station.charge_power += cur_tmp * vol_tmp;
            }
        }
    }

    if (Client_Sd_Station.charge_power > 0) {
        Client_Sd_Station.station_state = CLU_STATE_CHARGE;
    } else if (Client_Sd_Station.discharge_power > 0) {
        Client_Sd_Station.station_state = CLU_STATE_DISCHARGE;
    } else {
        Client_Sd_Station.station_state = CLU_STATE_IDLE;
    }

    Client_Sd_Station.checksum   = TX_CheckSum ((uint32_t *)&Client_Sd_Station, sizeof (Client_Sd_Station_t) / 4 - 2);
    Client_Sd_Station.frame_tail = FRAME_TAIL;
}

typedef struct {
    uint16_t cluster_id;
    uint16_t cell_id;
    uint16_t val;
} cell_info_t;

cell_info_t cell_max_vol[cluster_num];
cell_info_t cell_min_vol[cluster_num];
cell_info_t cell_max_temp[cluster_num];
cell_info_t cell_min_temp[cluster_num];
cell_info_t cell_max_soc[cluster_num];
cell_info_t cell_min_soc[cluster_num];
cell_info_t cell_max_soh[cluster_num];
cell_info_t cell_min_soh[cluster_num];
uint32_t cell_avg_vol[cluster_num];
uint32_t cell_avg_temp[cluster_num];
uint32_t cell_avg_soc[cluster_num];
uint32_t cell_avg_soh[cluster_num];
float clu_charge_cap[cluster_num];    // kwh
float clu_discharge_cap[cluster_num]; // kwh

extern cluster_info_u16_u cluster_info_u16[cluster_num];
extern cluster_info_f32_u cluster_info_f32[cluster_num];

extern modbus_float_u cell_vol[cluster_num][TOTOL_BAT_num];
extern modbus_float_u cell_temp[cluster_num][TOTOL_BAT_num];
extern modbus_float_u cell_soc[cluster_num][TOTOL_BAT_num];
extern modbus_float_u cell_soh[cluster_num][TOTOL_BAT_num];

extern uint8_t cell_charge_balance_status[cluster_num][TOTOL_BAT_num];
extern uint8_t cell_discharge_balance_status[cluster_num][TOTOL_BAT_num];

extern cluster_flash_t cluster_flash[cluster_num];

void
cal_modbus_cluster_data (void)
{

    for (int i = 0; i < cluster_num; i++) {
        cell_max_vol[i].val  = 0;
        cell_max_temp[i].val = 0;
        cell_max_soc[i].val  = 0;
        cell_min_vol[i].val  = 0xffff;
        cell_min_temp[i].val = 0xffff;
        cell_min_soc[i].val  = 0xffff;
        cell_avg_vol[i]      = 0;
        cell_avg_temp[i]     = 0;
        cell_avg_soc[i]      = 0;

        clu_charge_cap[i]    = 0; // kwh
        clu_discharge_cap[i] = 0; // kwh

        int cell_cnt = 0;

        if (BCMU[i].OnlineOrOffline == Offline) continue;
        for (int j = 0; j < GRP_num; j++) {
            if (bmu_offline[i][j]) continue;
            for (int k = 0; k < GRP_BAT_num; k++) {
                int index = j * GRP_BAT_num + k;

                cell_vol[i][index].f32  = Client_Sd[i].BAT_VOL[index] * 0.001;
                cell_temp[i][index].f32 = Client_Sd[i].BAT_TMP[index] * 0.01;
                cell_soc[i][index].f32  = Client_Sd[i].BAT_SOC[index] * 0.1;
                cell_soh[i][index].f32  = Client_Sd[i].BAT_SOH[index] * 0.1;

                if (Client_Sd[i].BAT_VOL[index] > cell_max_vol[i].val) {
                    cell_max_vol[i].cluster_id = i + 1;
                    cell_max_vol[i].cell_id    = index + 1;
                    cell_max_vol[i].val        = Client_Sd[i].BAT_VOL[index];
                }
                if (Client_Sd[i].BAT_TMP[index] > cell_max_temp[i].val) {
                    cell_max_temp[i].cluster_id = i + 1;
                    cell_max_temp[i].cell_id    = index + 1;
                    cell_max_temp[i].val        = Client_Sd[i].BAT_TMP[index];
                }
                if (Client_Sd[i].BAT_SOC[index] > cell_max_soc[i].val) {
                    cell_max_soc[i].cluster_id = i + 1;
                    cell_max_soc[i].cell_id    = index + 1;
                    cell_max_soc[i].val        = Client_Sd[i].BAT_SOC[index];
                }
                if (Client_Sd[i].BAT_SOH[index] > cell_max_soh[i].val) {
                    cell_max_soh[i].cluster_id = i + 1;
                    cell_max_soh[i].cell_id    = index + 1;
                    cell_max_soh[i].val        = Client_Sd[i].BAT_SOH[index];
                }

                if (Client_Sd[i].BAT_VOL[index] < cell_min_vol[i].val) {
                    cell_min_vol[i].cluster_id = i + 1;
                    cell_min_vol[i].cell_id    = index + 1;
                    cell_min_vol[i].val        = Client_Sd[i].BAT_VOL[index];
                }
                if (Client_Sd[i].BAT_TMP[index] < cell_min_temp[i].val) {
                    cell_min_temp[i].cluster_id = i + 1;
                    cell_min_temp[i].cell_id    = index + 1;
                    cell_min_temp[i].val        = Client_Sd[i].BAT_TMP[index];
                }
                if (Client_Sd[i].BAT_SOC[index] < cell_min_soc[i].val) {
                    cell_min_soc[i].cluster_id = i + 1;
                    cell_min_soc[i].cell_id    = index + 1;
                    cell_min_soc[i].val        = Client_Sd[i].BAT_SOC[index];
                }
                if (Client_Sd[i].BAT_SOH[index] < cell_min_soh[i].val) {
                    cell_min_soh[i].cluster_id = i + 1;
                    cell_min_soh[i].cell_id    = index + 1;
                    cell_min_soh[i].val        = Client_Sd[i].BAT_SOH[index];
                }

                cell_avg_vol[i] += Client_Sd[i].BAT_VOL[index];
                cell_avg_temp[i] += Client_Sd[i].BAT_TMP[index];
                cell_avg_soc[i] += Client_Sd[i].BAT_SOC[index];
                cell_avg_soh[i] += Client_Sd[i].BAT_SOH[index];
                cell_cnt++;
            }

            // todo
            //   uint32_t grp_balance_state = 0;
            //   grp_balance_state = (Client_Sd[i].bal_state >> (j + 1)) &
            //   0x00000001; if (grp_balance_state){
            //     memset (&cell_charge_balance_status[i][j * GRP_BAT_num], 0xff,
            //     GRP_BAT_num); memset (&cell_discharge_balance_status[i][j *
            //     GRP_BAT_num], 0xff, GRP_BAT_num);
            //   }else{
            //     memset (&cell_charge_balance_status[i][j * GRP_BAT_num], 0x00,
            //     GRP_BAT_num); memset (&cell_discharge_balance_status[i][j *
            //     GRP_BAT_num], 0x00, GRP_BAT_num);
            //   }
        }

        cell_avg_vol[i] /= cell_cnt;
        cell_avg_temp[i] /= cell_cnt;
        cell_avg_soc[i] /= cell_cnt;
        cell_avg_soh[i] /= cell_cnt;

        switch (Client_Sd[i].work_state) {
            case CLU_STATE_CHARGE:
                cluster_info_u16[i].data.charge_discharge_status = MODBUS_CLU_STATUS_CHARGE;
                break;
            case CLU_STATE_DISCHARGE:
                cluster_info_u16[i].data.charge_discharge_status = MODBUS_CLU_STATUS_DISCHARGE;
                break;
            case CLU_STATE_IDLE:
                cluster_info_u16[i].data.charge_discharge_status = MODBUS_CLU_STATUS_IDLE;
                break;
            case CLU_STATE_OPEN_CUR:
                cluster_info_u16[i].data.charge_discharge_status = MODBUS_CLU_STATUS_OPEN;
                break;

            default:
                cluster_info_u16[i].data.charge_discharge_status = MODBUS_CLU_STATUS_IDLE;
                break;
        }

        clu_charge_cap[i] = (CLU_CAP_FULL_SOC - Client_Sd[i].cluster_SOC / 10.0) * CLU_CAP_KWH / 100;
        if (clu_charge_cap[i] < 0) clu_charge_cap[i] = 0;
        clu_discharge_cap[i] = (Client_Sd[i].cluster_SOC / 10.0 - CLU_CAP_EMPTY_SOC) * CLU_CAP_KWH / 100;
        if (clu_discharge_cap[i] < 0) clu_discharge_cap[i] = 0;

        if (cluster_flash[i].charging_sum > 0) {
            cluster_info_u16[i].data.charge_count = cluster_flash[i].charging_sum / (CLU_CAP_KWH / 2);
        }
        if (cluster_flash[i].discharging_sum > 0) {
            cluster_info_u16[i].data.discharge_count = cluster_flash[i].discharging_sum / (CLU_CAP_KWH / 2);
        }

        cluster_info_u16[i].data.max_vol_cell  = cell_max_vol[i].cell_id;
        cluster_info_u16[i].data.min_vol_cell  = cell_min_vol[i].cell_id;
        cluster_info_u16[i].data.max_temp_cell = cell_max_temp[i].cell_id;
        cluster_info_u16[i].data.min_temp_cell = cell_min_temp[i].cell_id;
        cluster_info_u16[i].data.max_soc_cell  = cell_max_soc[i].cell_id;
        cluster_info_u16[i].data.min_soc_cell  = cell_min_soc[i].cell_id;
        cluster_info_u16[i].data.max_soh_cell  = cell_max_soh[i].cell_id;
        cluster_info_u16[i].data.min_soh_cell  = cell_min_soh[i].cell_id;

        cluster_info_f32[i].data.voltage_delta_max = (cell_max_vol[i].val - cell_min_vol[i].val) * 0.001;
        cluster_info_f32[i].data.avg_cell_voltage  = cell_avg_vol[i] * 0.001;
        cluster_info_f32[i].data.max_cell_voltage  = cell_max_vol[i].val * 0.001;
        cluster_info_f32[i].data.min_cell_voltage  = cell_min_vol[i].val * 0.001;

        cluster_info_f32[i].data.temperature_delta_max = (cell_max_temp[i].val - cell_min_temp[i].val) * 0.01;
        cluster_info_f32[i].data.avg_cell_temperature  = cell_avg_temp[i] * 0.01;
        cluster_info_f32[i].data.max_cell_temperature  = cell_max_temp[i].val * 0.01;
        cluster_info_f32[i].data.min_cell_temperature  = cell_min_temp[i].val * 0.01;

        cluster_info_f32[i].data.avg_cell_soc = cell_avg_soc[i] * 0.1;
        cluster_info_f32[i].data.max_cell_soc = cell_max_soc[i].val * 0.1;
        cluster_info_f32[i].data.min_cell_soc = cell_min_soc[i].val * 0.1;

        cluster_info_f32[i].data.avg_cell_soh = cell_avg_soh[i] * 0.1;
        cluster_info_f32[i].data.max_cell_soh = cell_max_soh[i].val * 0.1;
        cluster_info_f32[i].data.min_cell_soh = cell_min_soh[i].val * 0.1;

        cluster_info_f32[i].data.cluster_voltage = Client_Sd[i].cluster_VOL * 0.1;
        cluster_info_f32[i].data.cluster_current = Client_Sd[i].cluster_CUR * CLU_CUR_UNIT;
        cluster_info_f32[i].data.cluster_soc     = Client_Sd[i].cluster_SOC * 0.1;
        cluster_info_f32[i].data.cluster_soh     = Client_Sd[i].cluster_SOH * 0.1;

        cluster_info_f32[i].data.pos_insulation_res = Client_Sd[i].insulation_res_p / 1000.0; // Mohm
        cluster_info_f32[i].data.neg_insulation_res = Client_Sd[i].insulation_res_n / 1000.0; // Mohm

        cluster_info_f32[i].data.chargeable_capacity    = clu_charge_cap[i];
        cluster_info_f32[i].data.dischargeable_capacity = clu_discharge_cap[i];

        cluster_info_u16[i].data.pack_count    = Client_Sd[i].grp_num;
        cluster_info_u16[i].data.temp_count    = Client_Sd[i].grp_bat_num;
        cluster_info_u16[i].data.battery_count = Client_Sd[i].grp_bat_num;

        if (cell_max_vol[i].val * 0.001 <= CLU_CHARGE_STAGE_SW_VOL) {
            cluster_info_f32[i].data.max_charge_current = CLU_CHARGE_STAGE1_CUR;
            cluster_info_f32[i].data.max_charge_power
                = CLU_CHARGE_STAGE1_CUR * Client_Sd[i].cluster_VOL * 0.1 * 0.001; // kW
        } else {
            cluster_info_f32[i].data.max_charge_current = CLU_CHARGE_STAGE2_CUR;
            cluster_info_f32[i].data.max_charge_power
                = CLU_DISCHARGE_STAGE1_CUR * Client_Sd[i].cluster_VOL * 0.1 * 0.001; // kW
        }
        cluster_info_f32[i].data.max_discharge_current = CLU_DISCHARGE_STAGE1_CUR;
        cluster_info_f32[i].data.max_discharge_power
            = CLU_DISCHARGE_STAGE1_CUR * Client_Sd[i].cluster_VOL * 0.1 * 0.001; // kW

        cluster_info_f32[i].data.accumulated_charge_capacity    = cluster_flash[i].charging_sum;
        cluster_info_f32[i].data.accumulated_discharge_capacity = cluster_flash[i].discharging_sum;
    }
}

extern station_info_u16_u station_info_u16;
extern station_info_f32_u station_info_f32;

void
cal_modbus_sta_data (void)
{
    int cluster_min_soc_idx = 0;
    int cluster_max_soc_idx = 0;
    int cluster_min_vol_idx = 0;
    int cluster_max_vol_idx = 0;

    float sta_charge_cap            = 0;
    float sta_discharge_cap         = 0;
    float sta_max_charge_current    = 0;
    float sta_max_charge_power      = 0;
    float sta_max_discharge_current = 0;
    float sta_max_discharge_power   = 0;
    float sta_single_charge         = 0;
    float sta_single_discharge      = 0;
    float sta_charge_sum            = 0;
    float sta_discharge_sum         = 0;

    cell_info_t cluster_cell_max_vol  = { 0, 0, 0 };      // max in cluster
    cell_info_t cluster_cell_min_vol  = { 0, 0, 0xffff }; // min in cluster
    cell_info_t cluster_cell_max_temp = { 0, 0, 0 };      // max in cluster
    cell_info_t cluster_cell_min_temp = { 0, 0, 0xffff }; // min in cluster

    sta_single_charge    = 0;
    sta_single_discharge = 0;
    sta_charge_sum       = 0;
    sta_discharge_sum    = 0;

    for (int i = 0; i < cluster_num; i++) {
        if (BCMU[i].OnlineOrOffline == Offline) continue;

        if (Client_Sd[i].cluster_SOC < Client_Sd[cluster_min_soc_idx].cluster_SOC) { cluster_min_soc_idx = i; }
        if (Client_Sd[i].cluster_SOC > Client_Sd[cluster_max_soc_idx].cluster_SOC) { cluster_max_soc_idx = i; }

        if (Client_Sd[i].cluster_VOL < Client_Sd[cluster_min_vol_idx].cluster_VOL) { cluster_min_vol_idx = i; }
        if (Client_Sd[i].cluster_VOL > Client_Sd[cluster_max_vol_idx].cluster_VOL) { cluster_max_vol_idx = i; }

        if (cell_max_vol[i].val > cluster_cell_max_vol.val) {
            memcpy (&cluster_cell_max_vol, &cell_max_vol[i], sizeof (cell_info_t));
        }
        if (cell_min_vol[i].val < cluster_cell_min_vol.val) {
            memcpy (&cluster_cell_min_vol, &cell_min_vol[i], sizeof (cell_info_t));
        }

        if (cell_max_temp[i].val > cluster_cell_max_temp.val) {
            memcpy (&cluster_cell_max_temp, &cell_max_temp[i], sizeof (cell_info_t));
        }
        if (cell_min_temp[i].val < cluster_cell_min_temp.val) {
            memcpy (&cluster_cell_min_temp, &cell_min_temp[i], sizeof (cell_info_t));
        }

        sta_charge_cap += clu_charge_cap[i];
        sta_discharge_cap += clu_discharge_cap[i];

        sta_max_charge_current += cluster_info_f32[i].data.max_charge_current;
        sta_max_charge_power += cluster_info_f32[i].data.max_charge_power;
        sta_max_discharge_current += cluster_info_f32[i].data.max_discharge_current;
        sta_max_discharge_power += cluster_info_f32[i].data.max_discharge_power;

        sta_single_charge += cluster_info_f32[i].data.single_charge_capacity;
        sta_single_discharge += cluster_info_f32[i].data.single_discharge_capacity;
        sta_charge_sum += cluster_info_f32[i].data.accumulated_charge_capacity;
        sta_discharge_sum += cluster_info_f32[i].data.accumulated_discharge_capacity;
    }

    station_info_u16.data.min_soc_cluster = cluster_min_soc_idx + 1;
    station_info_u16.data.max_soc_cluster = cluster_max_soc_idx + 1;
    station_info_u16.data.min_vol_cluster = cluster_min_vol_idx + 1;
    station_info_u16.data.max_vol_cluster = cluster_max_vol_idx + 1;

    station_info_u16.data.max_vol_cell_cluster = cluster_cell_max_vol.cluster_id;
    station_info_u16.data.max_vol_cell_index   = cluster_cell_max_vol.cell_id;
    station_info_u16.data.min_vol_cell_cluster = cluster_cell_min_vol.cluster_id;
    station_info_u16.data.min_vol_cell_index   = cluster_cell_min_vol.cell_id;
    station_info_u16.data.max_temp_cluster     = cluster_cell_max_temp.cluster_id;
    station_info_u16.data.max_temp_cell_index  = cluster_cell_max_temp.cell_id;
    station_info_u16.data.min_temp_cluster     = cluster_cell_min_temp.cluster_id;
    station_info_u16.data.min_temp_cell_index  = cluster_cell_min_temp.cell_id;

    station_info_f32.data.station_cell_vol_delta  = (cluster_cell_max_vol.val - cluster_cell_min_vol.val) * 0.001;
    station_info_f32.data.max_single_cell_vol     = cluster_cell_max_vol.val * 0.001;
    station_info_f32.data.min_single_cell_vol     = cluster_cell_min_vol.val * 0.001;
    station_info_f32.data.station_cell_temp_delta = (cluster_cell_max_temp.val - cluster_cell_min_temp.val) * 0.1;
    station_info_f32.data.max_single_cell_temp    = cluster_cell_max_temp.val * 0.1;
    station_info_f32.data.min_single_cell_temp    = cluster_cell_min_temp.val * 0.1;
    station_info_f32.data.cluster_soc_delta
        = (Client_Sd[cluster_max_soc_idx].cluster_SOC - Client_Sd[cluster_min_soc_idx].cluster_SOC) * 0.1;
    station_info_f32.data.max_cluster_soc = Client_Sd[cluster_max_soc_idx].cluster_SOC * 0.1;
    station_info_f32.data.min_cluster_soc = Client_Sd[cluster_min_soc_idx].cluster_SOC * 0.1;
    station_info_f32.data.cluster_vol_delta
        = (Client_Sd[cluster_max_vol_idx].cluster_VOL - Client_Sd[cluster_min_vol_idx].cluster_VOL) * 0.1;
    station_info_f32.data.max_cluster_vol = Client_Sd[cluster_max_vol_idx].cluster_VOL * 0.1;
    station_info_f32.data.min_cluster_vol = Client_Sd[cluster_min_vol_idx].cluster_VOL * 0.1;

    station_info_f32.data.station_vol     = Client_Sd_Station.station_VOL * 0.1;
    station_info_f32.data.station_current = Client_Sd_Station.station_CUR * CLU_CUR_UNIT;
    station_info_f32.data.station_soc     = Client_Sd_Station.station_SOC * 0.1;
    station_info_f32.data.station_soh     = Client_Sd_Station.station_SOH * 0.1;

    station_info_f32.data.station_charge_capacity    = sta_charge_cap;
    station_info_f32.data.station_discharge_capacity = sta_discharge_cap;

    station_info_f32.data.station_max_charging_current    = sta_max_charge_current;
    station_info_f32.data.station_max_charging_power      = sta_max_charge_power;
    station_info_f32.data.station_max_discharging_current = sta_max_discharge_current;
    station_info_f32.data.station_max_discharging_power   = sta_max_discharge_power;

    station_info_f32.data.station_single_charge_capacity    = sta_single_charge;
    station_info_f32.data.station_single_discharge_capacity = sta_single_discharge;
    station_info_f32.data.station_cumulative_charge         = sta_charge_sum;
    station_info_f32.data.station_cumulative_discharge      = sta_discharge_sum;

    uint32_t cluster_online = 0;
    int cluster_online_cnt  = 0;
    for (int i = cluster_num - 1; i >= 0; i--) {
        uint32_t mask = 1 << i;
        if (BCMU[i].OnlineOrOffline == Online) {
            cluster_online |= mask;
            cluster_online_cnt++;
        }
    }
    station_info_u16.data.is_cluster_operational_l = cluster_online & 0x0000ffff;
    station_info_u16.data.is_cluster_operational_h = (cluster_online & 0xffff0000) >> 16U;
    station_info_u16.data.managed_cluster_count    = cluster_online_cnt;
    station_info_u16.data.installed_cluster_count  = bsmuSetting.cu_num;
}

extern cluster_warning_u cluster_warning[cluster_num];

void
cal_modbus_warning_data (void)
{
    for (int i = 0; i < cluster_num; i++) {
        if (BCMU[i].OnlineOrOffline == Offline) continue;

        cluster_warning[i].data.clus_v_high_warn    = (Client_Sd[i].cluster_VOL * 0.1 > CLU_HIGH_VOLT_WARN);
        cluster_warning[i].data.clus_v_low_warn     = (Client_Sd[i].cluster_VOL * 0.1 < CLU_LOW_VOLT_WARN);
        cluster_warning[i].data.cell_v_high_warn    = (cell_max_vol[i].val * 0.001 > CELL_HIGH_VOLT_WARN);
        cluster_warning[i].data.cell_v_low_warn     = (cell_min_vol[i].val * 0.001 < CELL_LOW_VOLT_WARN);
        cluster_warning[i].data.cell_temp_high_warn = (cell_max_temp->val * 0.01 > CELL_HIGH_TEMP_WARN);
        cluster_warning[i].data.cell_temp_low_warn  = (cell_min_temp->val * 0.01 < CELL_LOW_TEMP_WARN);
        // cluster_warning[i].data.ambient_temp_high_warn = ();
        // cluster_warning[i].data.ambient_temp_low_warn = ();
        // cluster_warning[i].data.powerline_temp_high_warn = ();
        cluster_warning[i].data.clus_soc_high_warn     = (Client_Sd[i].cluster_SOC * 0.1 > CLU_HIGH_SOC_WARN);
        cluster_warning[i].data.clus_soc_low_warn      = (Client_Sd[i].cluster_SOC * 0.1 < CLU_LOW_SOC_WARN);
        cluster_warning[i].data.pos_insul_res_low_warn = (Client_Sd[i].insulation_res_p < CLU_INSUL_RES_P_LOW_WARN);
        cluster_warning[i].data.neg_insul_res_low_warn = (Client_Sd[i].insulation_res_n < CLU_INSUL_RES_N_LOW_WARN);

        cluster_warning[i].data.clus_v_high_alarm    = (Client_Sd[i].cluster_VOL * 0.1 > CLU_HIGH_VOLT_ALARM);
        cluster_warning[i].data.clus_v_low_alarm     = (Client_Sd[i].cluster_VOL * 0.1 < CLU_LOW_VOLT_ALARM);
        cluster_warning[i].data.cell_v_high_alarm    = (cell_max_vol[i].val * 0.001 > CELL_HIGH_VOLT_ALARM);
        cluster_warning[i].data.cell_v_low_alarm     = (cell_min_vol[i].val * 0.001 < CELL_LOW_VOLT_ALARM);
        cluster_warning[i].data.cell_temp_high_alarm = (cell_max_temp->val * 0.01 > CELL_HIGH_TEMP_ALARM);
        cluster_warning[i].data.cell_temp_low_alarm  = (cell_min_temp->val * 0.01 < CELL_LOW_TEMP_ALARM);
        // cluster_warning[i].data.powerline_temp_high_alarm = ();
        cluster_warning[i].data.clus_soc_high_alarm     = (Client_Sd[i].cluster_SOC * 0.1 > CLU_HIGH_SOC_ALARM);
        cluster_warning[i].data.clus_soc_low_alarm      = (Client_Sd[i].cluster_SOC * 0.1 < CLU_LOW_SOC_ALARM);
        cluster_warning[i].data.pos_insul_res_low_alarm = (Client_Sd[i].insulation_res_p < CLU_INSUL_RES_P_LOW_ALARM);
        cluster_warning[i].data.neg_insul_res_low_alarm = (Client_Sd[i].insulation_res_n < CLU_INSUL_RES_N_LOW_ALARM);

        cluster_warning[i].data.clus_v_high_protect    = (Client_Sd[i].cluster_VOL * 0.1 > CLU_HIGH_VOLT_PROTECT);
        cluster_warning[i].data.clus_v_low_protect     = (Client_Sd[i].cluster_VOL * 0.1 < CLU_LOW_VOLT_PROTECT);
        cluster_warning[i].data.cell_v_high_protect    = (cell_max_vol[i].val * 0.001 > CELL_HIGH_VOLT_PROTECT);
        cluster_warning[i].data.cell_v_low_protect     = (cell_min_vol[i].val * 0.001 < CELL_LOW_VOLT_PROTECT);
        cluster_warning[i].data.cell_temp_high_protect = (cell_max_temp->val * 0.01 > CELL_HIGH_TEMP_PROTECT);
        cluster_warning[i].data.cell_temp_low_protect  = (cell_min_temp->val * 0.01 < CELL_LOW_TEMP_PROTECT);
        // cluster_warning[i].data.powerline_temp_high_protect = ();
        cluster_warning[i].data.clus_soc_high_protect = (Client_Sd[i].cluster_SOC * 0.1 > CLU_HIGH_SOC_PROTECT);
        cluster_warning[i].data.clus_soc_low_protect  = (Client_Sd[i].cluster_SOC * 0.1 < CLU_LOW_SOC_PROTECT);
        cluster_warning[i].data.pos_insul_res_low_protect
            = (Client_Sd[i].insulation_res_p < CLU_INSUL_RES_P_LOW_PROTECT);
        cluster_warning[i].data.neg_insul_res_low_protect
            = (Client_Sd[i].insulation_res_n < CLU_INSUL_RES_N_LOW_PROTECT);

        float cluster_current;
        // charging
        if (Client_Sd[i].cluster_CUR >= 0) {
            cluster_current = Client_Sd[i].cluster_CUR * CLU_CUR_UNIT;

            cluster_warning[i].data.clus_chg_curr_high_warn = (cluster_current > CLU_HIGH_CHARGE_CUR_WARN);
            cluster_warning[i].data.clus_chg_temp_high_warn = (cell_max_temp[i].val * 0.01 > CLU_HIGH_CHARGE_TEMP_WARN);
            cluster_warning[i].data.clus_chg_temp_low_warn  = (cell_min_temp[i].val * 0.01 < CLU_LOW_CHARGE_TEMP_WARN);

            cluster_warning[i].data.clus_chg_curr_high_alarm = (cluster_current > CLU_HIGH_CHARGE_CUR_ALARM);
            cluster_warning[i].data.clus_chg_temp_high_alarm
                = (cell_max_temp[i].val * 0.01 > CLU_HIGH_CHARGE_TEMP_ALARM);
            cluster_warning[i].data.clus_chg_temp_low_alarm = (cell_min_temp[i].val * 0.01 < CLU_LOW_CHARGE_TEMP_ALARM);

            cluster_warning[i].data.clus_chg_curr_high_protect = (cluster_current > CLU_HIGH_CHARGE_CUR_PROTECT);
            cluster_warning[i].data.clus_chg_temp_high_protect
                = (cell_max_temp[i].val * 0.01 > CLU_HIGH_CHARGE_TEMP_PROTECT);
            cluster_warning[i].data.clus_chg_temp_low_protect
                = (cell_min_temp[i].val * 0.01 < CLU_LOW_CHARGE_TEMP_PROTECT);
        } else { // discharging
            cluster_current = Client_Sd[i].cluster_CUR * -CLU_CUR_UNIT;

            cluster_warning[i].data.clus_disch_curr_high_warn = (cluster_current > CLU_HIGH_DISCHARGE_CUR_WARN);
            cluster_warning[i].data.clus_disch_temp_high_warn
                = (cell_max_temp[i].val * 0.01 > CLU_HIGH_DISCHARGE_TEMP_WARN);
            cluster_warning[i].data.clus_disch_temp_low_warn
                = (cell_min_temp[i].val * 0.01 < CLU_LOW_DISCHARGE_TEMP_WARN);

            cluster_warning[i].data.clus_disch_curr_high_alarm = (cluster_current > CLU_HIGH_DISCHARGE_CUR_ALARM);
            cluster_warning[i].data.clus_disch_temp_high_alarm
                = (cell_max_temp[i].val * 0.01 > CLU_HIGH_DISCHARGE_TEMP_ALARM);
            cluster_warning[i].data.clus_disch_temp_low_alarm
                = (cell_min_temp[i].val * 0.01 < CLU_LOW_DISCHARGE_TEMP_ALARM);

            cluster_warning[i].data.clus_disch_curr_high_protect = (cluster_current > CLU_HIGH_DISCHARGE_CUR_PROTECT);
            cluster_warning[i].data.clus_disch_temp_high_protect
                = (cell_max_temp[i].val * 0.01 > CLU_HIGH_DISCHARGE_TEMP_PROTECT);
            cluster_warning[i].data.clus_disch_temp_low_protect
                = (cell_min_temp[i].val * 0.01 < CLU_LOW_DISCHARGE_TEMP_PROTECT);
        }
    }
}

#define STATE_CHARGE    2
#define STATE_DISCHARGE 1
#define STATE_OTHER     0

uint8_t clu_status[cluster_num] = { 0 };
float clu_charge_soc_start[cluster_num];
float clu_discharge_soc_start[cluster_num];

void
update_modbus_state (void)
{
    station_info_u16.data.station_operating_status = MODBUS_STA_STATUS_NORMAL;
    for (int i = 0; i < cluster_num; i++) {
        if (BCMU[i].OnlineOrOffline == Offline) {
            clu_status[i] = STATE_OTHER;
            continue;
        }

        switch (clu_status[i]) {
            case STATE_OTHER:
                if (Client_Sd[i].work_state == CLU_STATE_CHARGE) {
                    clu_charge_soc_start[i] = Client_Sd[i].cluster_SOC * 0.1;
                    clu_status[i]           = STATE_CHARGE;
                } else if (Client_Sd[i].work_state == CLU_STATE_DISCHARGE) {
                    clu_discharge_soc_start[i] = Client_Sd[i].cluster_SOC * 0.1;
                    clu_status[i]              = STATE_DISCHARGE;
                }
                break;
            case STATE_DISCHARGE:
                if (Client_Sd[i].work_state != CLU_STATE_DISCHARGE) {
                    cluster_info_f32[i].data.single_discharge_capacity
                        = (clu_discharge_soc_start[i] - Client_Sd[i].cluster_SOC * 0.1) / 100 * CLU_CAP_KWH;
                    cluster_flash[i].discharging_sum += cluster_info_f32[i].data.single_discharge_capacity;

                    if (Client_Sd[i].work_state == CLU_STATE_CHARGE) {
                        clu_charge_soc_start[i] = Client_Sd[i].cluster_SOC * 0.1;
                        clu_status[i]           = STATE_CHARGE;
                    } else {
                        clu_status[i] = STATE_OTHER;
                    }
                }
                break;
            case STATE_CHARGE:
                if (Client_Sd[i].work_state != CLU_STATE_CHARGE) {
                    cluster_info_f32[i].data.single_charge_capacity
                        = (Client_Sd[i].cluster_SOC * 0.1 - clu_charge_soc_start[i]) / 100 * CLU_CAP_KWH;
                    cluster_flash[i].charging_sum += cluster_info_f32[i].data.single_charge_capacity;

                    if (Client_Sd[i].work_state == CLU_STATE_DISCHARGE) {
                        clu_discharge_soc_start[i] = Client_Sd[i].cluster_SOC * 0.1;
                        clu_status[i]              = STATE_DISCHARGE;
                    } else {
                        clu_status[i] = STATE_OTHER;
                    }
                }
                break;

            default:
                clu_status[i] = STATE_OTHER;
                break;
        }

        cluster_info_u16[i].data.cluster_status = MODBUS_STA_STATUS_NORMAL;
        for (int j = 0; j < 19; j++) {
            if (cluster_warning[i].reg[j]) {
                cluster_info_u16[i].data.cluster_status        = MODBUS_STA_STATUS_WARN;
                station_info_u16.data.station_operating_status = MODBUS_STA_STATUS_WARN;
                break;
            }
        }
        for (int j = 19; j < 36; j++) {
            if (cluster_warning[i].reg[j]) {
                cluster_info_u16[i].data.cluster_status        = MODBUS_STA_STATUS_ALARM;
                station_info_u16.data.station_operating_status = MODBUS_STA_STATUS_ALARM;
                break;
            }
        }
        for (int j = 36; j < 65; j++) {
            if (cluster_warning[i].reg[j]) {
                cluster_info_u16[i].data.cluster_status        = MODBUS_STA_STATUS_BREAK;
                station_info_u16.data.station_operating_status = MODBUS_STA_STATUS_BREAK;
                break;
            }
        }
    }
}

void
cal_modbus_data (void)
{
    cal_modbus_cluster_data ();
    cal_modbus_sta_data ();
    cal_modbus_warning_data ();
    update_modbus_state ();
}
