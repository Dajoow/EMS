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
    uint16_t max_vol_cluster;
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
    float cluster_voltage;
    float cluster_current;
    float cluster_soc;
    float cluster_soh;
    float ambient_temp;
    float pos_insulation_res;
    float neg_insulation_res;
    float chargeable_capacity;
    float dischargeable_capacity;
    float single_charge_capacity;
    float single_discharge_capacity;
    float accumulated_charge_capacity;
    float accumulated_discharge_capacity;
    float voltage_delta_max;
    float avg_cell_voltage;
    float max_cell_voltage;
    float min_cell_voltage;
    float temperature_delta_max;
    float avg_cell_temperature;
    float max_cell_temperature;
    float min_cell_temperature;
    float avg_cell_resistance;
    float max_cell_resistance;
    float min_cell_resistance;
    float avg_cell_soc;
    float max_cell_soc;
    float min_cell_soc;
    float avg_cell_soh;
    float max_cell_soh;
    float min_cell_soh;
    float max_charge_power;
    float max_discharge_power;
    float max_charge_current;
    float max_discharge_current;
  } data;

  modbus_float_u reg[34];
} cluster_info_f32_u;

// warn < alarm < protect
typedef union{
  struct {
    uint8_t clus_v_high_warn;
    uint8_t clus_v_low_warn;
    uint8_t clus_chg_curr_high;
    uint8_t clus_disch_curr_high;
    uint8_t cell_v_high_warn;
    uint8_t cell_v_low_warn;
    uint8_t clus_chg_temp_high;
    uint8_t clus_chg_temp_low;
    uint8_t clus_disch_temp_high;
    uint8_t clus_disch_temp_low;
    uint8_t cell_temp_high_warn;
    uint8_t cell_temp_low_warn;
    uint8_t ambient_temp_high;
    uint8_t ambient_temp_low;
    uint8_t powerline_temp_high;
    uint8_t clus_soc_high_warn;
    uint8_t clus_soc_low_warn;
    uint8_t pos_insul_res_low;
    uint8_t neg_insul_res_low;
    uint8_t clus_v_high_alarm;
    uint8_t clus_v_low_alarm;
    uint8_t clus_chg_curr_high_alarm;
    uint8_t clus_disch_curr_high_alarm;
    uint8_t cell_v_high_alarm;
    uint8_t cell_v_low_alarm;
    uint8_t clus_chg_temp_high_alarm;
    uint8_t clus_chg_temp_low_alarm;
    uint8_t clus_disch_temp_high_alarm;
    uint8_t clus_disch_temp_low_alarm;
    uint8_t cell_temp_high_alarm;
    uint8_t cell_temp_low_alarm;
    uint8_t powerline_temp_high_alarm;
    uint8_t clus_soc_high_alarm;
    uint8_t clus_soc_low_alarm;
    uint8_t pos_insul_res_low_alarm;
    uint8_t neg_insul_res_low_alarm;
    uint8_t clus_v_high_protect;
    uint8_t clus_v_low_protect;
    uint8_t clus_chg_curr_high_protect;
    uint8_t clus_disch_curr_high_protect;
    uint8_t cell_v_high_protect;
    uint8_t cell_v_low_protect;
    uint8_t clus_chg_temp_high_protect;
    uint8_t clus_chg_temp_low_protect;
    uint8_t clus_disch_temp_high_protect;
    uint8_t clus_disch_temp_low_protect;
    uint8_t cell_temp_high_protect;
    uint8_t cell_temp_low_protect;
    uint8_t powerline_temp_high_protect;
    uint8_t clus_soc_high_protect;
    uint8_t clus_soc_low_protect;
    uint8_t pos_insul_res_low_protect;
    uint8_t neg_insul_res_low_protect;
    uint8_t rapid_temp_rise_cell;
    uint8_t powerline_temp_rise;
    uint8_t main_pos_contactor_st;
    uint8_t main_neg_contactor_st;
    uint8_t precharge_contactor_st;
    uint8_t main_pos_contactor_open;
    uint8_t main_neg_contactor_open;
    uint8_t precharge_contactor_open;
    uint8_t hall_comm_fault;
    uint8_t hall_acquisition_fault;
    uint8_t pos_fuse_open_fault;
    uint8_t neg_fuse_open_fault;
  } data;

  uint8_t reg[65];
} cluster_warning_u;

void bsmu_modbus_data_init (void);

#endif
