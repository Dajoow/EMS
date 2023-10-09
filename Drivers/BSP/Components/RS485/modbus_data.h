#ifndef __MODBUS_DATA_H__
#define __MODBUS_DATA_H__

#include "modbus_slave.h"

typedef union{
    float f32;
    uint16_t u16[2];
} modbus_float_u;

typedef struct 
{
    int slave_addr;
    agile_modbus_slave_util_t* slave_util;
} bsmu_modbus_data_t;

typedef union
{
  struct
  {
    uint16_t protocol_version;
    uint16_t station_operating_status;
    uint16_t installed_cluster_count;
    uint16_t managed_cluster_count;
    uint16_t is_cluster_operational_h;
    uint16_t is_cluster_operational_l;
    uint16_t max_soc_cluster;
    uint16_t min_soc_cluster;
    uint16_t max_vol_cluste;
    uint16_t min_vol_cluster;
    uint16_t max_vol_cell_cluster;
    uint16_t max_vol_cell_index;
    uint16_t min_vol_cell_cluster;
    uint16_t min_vol_cell_index;
    uint16_t max_temp_cluster;
    uint16_t max_temp_cell_index;
    uint16_t min_temp_cluster;
    uint16_t min_temp_cell_index;
  } data;

  uint16_t reg[17];
} station_info_u16_u;

typedef union
{
  struct
  {
    float station_vol;
    float station_current;
    float station_soc;
    float station_soh;
    float station_charge_capacity;
    float station_discharge_capacity;
    float station_single_charge_capacity;
    float station_single_discharge_capacity;
    float station_cumulative_charge;
    float station_cumulative_discharge;
    float station_cell_vol_delta;
    float max_single_cell_vol;
    float min_single_cell_vol;
    float station_cell_temp_delta;
    float max_single_cell_temp;
    float min_single_cell_temp;
    float cluster_soc_delta;
    float max_cluster_soc;
    float min_cluster_soc;
    float cluster_vol_delta;
    float max_cluster_vol;
    float min_cluster_vol;
    float station_max_charging_power;
    float station_max_discharging_power;
    float station_max_charging_current;
    float station_max_discharging_current;
  } data;

  modbus_float_u reg[26];
} station_info_f32_u;

typedef union{
  struct{
    uint16_t cluster_status;
    uint16_t charge_discharge_status;
    uint16_t battery_count;
    uint16_t temp_count;
    uint16_t pack_count;
    uint16_t charge_count;
    uint16_t discharge_count;
    uint16_t max_vol_cell;
    uint16_t min_vol_cell;
    uint16_t max_temp_cell;
    uint16_t min_temp_cell;
    uint16_t max_res_cell;
    uint16_t min_res_cell;
    uint16_t max_soc_cell;
    uint16_t min_soc_cell;
    uint16_t max_soh_cell;
    uint16_t min_soh_cell;
  } data;
  uint16_t reg[17];
} cluster_info_u16_u;

typedef union {
  struct{
    uint16_t cluster_voltage;
    uint16_t cluster_current;
    uint16_t cluster_soc;
    uint16_t cluster_soh;
    uint16_t ambient_temp;
    uint16_t pos_insulation_res;
    uint16_t neg_insulation_res;
    uint16_t chargeable_capacity;
    uint16_t dischargeable_capacity;
    uint16_t single_charge_capacity;
    uint16_t single_discharge_capacity;
    uint16_t accumulated_charge_capacity;
    uint16_t accumulated_discharge_capacity;
    uint16_t voltage_delta_max;
    uint16_t avg_cell_voltage;
    uint16_t max_cell_voltage;
    uint16_t min_cell_voltage;
    uint16_t temperature_delta_max;
    uint16_t avg_cell_temperature;
    uint16_t max_cell_temperature;
    uint16_t min_cell_temperature;
    uint16_t avg_cell_resistance;
    uint16_t max_cell_resistance;
    uint16_t min_cell_resistance;
    uint16_t avg_cell_soc;
    uint16_t max_cell_soc;
    uint16_t min_cell_soc;
    uint16_t avg_cell_soh;
    uint16_t max_cell_soh;
    uint16_t min_cell_soh;
    uint16_t max_charge_power;
    uint16_t max_discharge_power;
    uint16_t max_charge_current;
    uint16_t max_discharge_current;
  } data;

  modbus_float_u reg[34];
} cluster_info_f32_u;

// warn < alarm < protect
typedef union{
  struct {
    uint16_t clus_v_high_warn;
    uint16_t clus_v_low_warn;
    uint16_t clus_chg_curr_high;
    uint16_t clus_disch_curr_high;
    uint16_t cell_v_high_warn;
    uint16_t cell_v_low_warn;
    uint16_t clus_chg_temp_high;
    uint16_t clus_chg_temp_low;
    uint16_t clus_disch_temp_high;
    uint16_t clus_disch_temp_low;
    uint16_t cell_temp_high_warn;
    uint16_t cell_temp_low_warn;
    uint16_t ambient_temp_high;
    uint16_t ambient_temp_low;
    uint16_t powerline_temp_high;
    uint16_t clus_soc_high_warn;
    uint16_t clus_soc_low_warn;
    uint16_t pos_insul_res_low;
    uint16_t neg_insul_res_low;
    uint16_t clus_v_high_alarm;
    uint16_t clus_v_low_alarm;
    uint16_t clus_chg_curr_high_alarm;
    uint16_t clus_disch_curr_high_alarm;
    uint16_t cell_v_high_alarm;
    uint16_t cell_v_low_alarm;
    uint16_t clus_chg_temp_high_alarm;
    uint16_t clus_chg_temp_low_alarm;
    uint16_t clus_disch_temp_high_alarm;
    uint16_t clus_disch_temp_low_alarm;
    uint16_t cell_temp_high_alarm;
    uint16_t cell_temp_low_alarm;
    uint16_t powerline_temp_high_alarm;
    uint16_t clus_soc_high_alarm;
    uint16_t clus_soc_low_alarm;
    uint16_t pos_insul_res_low_alarm;
    uint16_t neg_insul_res_low_alarm;
    uint16_t clus_v_high_protect;
    uint16_t clus_v_low_protect;
    uint16_t clus_chg_curr_high_protect;
    uint16_t clus_disch_curr_high_protect;
    uint16_t cell_v_high_protect;
    uint16_t cell_v_low_protect;
    uint16_t clus_chg_temp_high_protect;
    uint16_t clus_chg_temp_low_protect;
    uint16_t clus_disch_temp_high_protect;
    uint16_t clus_disch_temp_low_protect;
    uint16_t cell_temp_high_protect;
    uint16_t cell_temp_low_protect;
    uint16_t powerline_temp_high_protect;
    uint16_t clus_soc_high_protect;
    uint16_t clus_soc_low_protect;
    uint16_t pos_insul_res_low_protect;
    uint16_t neg_insul_res_low_protect;
    uint16_t rapid_temp_rise_cell;
    uint16_t powerline_temp_rise;
    uint16_t main_pos_contactor_st;
    uint16_t main_neg_contactor_st;
    uint16_t precharge_contactor_st;
    uint16_t main_pos_contactor_open;
    uint16_t main_neg_contactor_open;
    uint16_t precharge_contactor_open;
    uint16_t hall_comm_fault;
    uint16_t hall_acquisition_fault;
    uint16_t pos_fuse_open_fault;
    uint16_t neg_fuse_open_fault;
  } data;

  uint8_t reg[65];
} cluster_warning_u;

void bsmu_modbus_data_init (void);

#endif
