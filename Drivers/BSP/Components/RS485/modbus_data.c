#include "modbus_data.h"
#include "modbus_slave.h"
#include "string.h"
#include "stdint.h"
#include "station_ctl.h"
/**
 * @brief get date from regs and put into buffer
 * @param map reg data map
 * @param buf destination buffer
 * @param adde read start address
 * @param len number of regs needed
 * @return 0
 */
static int get_map_buf(const agile_modbus_slave_util_map_t *map, void *buf,
                       int index, int len) {
  modbus_data_type_e modbus_data_type = map->data_type;
  int start_addr = map->start_addr;

  switch (modbus_data_type) {
  case MODBUS_FLOAT: {
    uint16_t *ptr = (uint16_t *)buf;
    modbus_float_u *data = map->data;

    for (int i = 0; i < len; i++) {
      // Note: Modbus transfers data in big-endian, which needed to be
      // swapped to fit the little-endian ISA.
      ptr[i * 2] = data[(index - start_addr) + i].u16[1];
      ptr[i * 2 + 1] = data[(index - start_addr) + i].u16[0];
    }
    break;
  }
  default:{
      const uint8_t *src = (uint8_t *)map->data;
      int offset = index - start_addr;

      if(modbus_data_type == MODBUS_BIT)
      {
        memcpy(buf, src + offset, len);
      }
      else
      {
        memcpy( buf,
                src+offset * sizeof(uint16_t),
                len * sizeof(uint16_t));
      }
    }
    break;
  }

  return 0;
}

static int set_map_buf(const agile_modbus_slave_util_map_t *map, int index,
                       int len, void *buf, int bufsz) {
  modbus_data_type_e modbus_data_type = map->data_type;
  int start_addr = map->start_addr;

  size_t size = 0;

  switch (modbus_data_type) {
  case MODBUS_UINT16:
    size = 2;
    break;
  case MODBUS_FLOAT:
    size = 4;
    break;
  case MODBUS_BIT:
    size = 1;
    break;
  }
    uint8_t *dst = (uint8_t *)map->data;
    int offset = index - start_addr;

    memcpy(dst + offset * size, buf, len * size);
  //memcpy(map->data + index, buf, len * size);

  return 0;
}

station_info_u16_u station_info_u16;
station_info_f32_u station_info_f32;

agile_modbus_slave_util_map_t station_input_regs_map[] = {
    // 电池堆数据 UINT16 RO
    {
        .start_addr = 0x00,
        .end_addr = 0x11,
        .data = station_info_u16.reg,
        .data_len = sizeof(station_info_u16.reg),
        .data_type = MODBUS_UINT16,
    },
    // 电池堆数据 FLOAT RO
    {
        .start_addr = 0x12,
        .end_addr = 0x45,
        .data = station_info_f32.reg,
        .data_len = sizeof(station_info_f32.reg),
        .data_type = MODBUS_FLOAT,
    }};

station_charge_ctrl_u station_charge_ctrl;

agile_modbus_slave_util_map_t station_regs_map[] = {
    // 充放电控制
    {
        .start_addr = 0x01,
        .end_addr = 0x01,
        .data = station_charge_ctrl.reg,
        .data_len = sizeof(station_charge_ctrl.reg),
        .data_type = MODBUS_UINT16,
    }};

agile_modbus_slave_util_t station_info = {NULL,
                                          0,
                                          NULL,
                                          0,
                                          station_regs_map,
                                          sizeof(station_regs_map) /
                                              sizeof(station_regs_map[0]),
                                          station_input_regs_map,
                                          sizeof(station_input_regs_map) /
                                              sizeof(station_input_regs_map[0]),
                                          get_map_buf,
                                          set_map_buf,
                                          NULL,
                                          NULL,
                                          NULL};

cluster_info_u16_u cluster_info_u16[cluster_num];
cluster_info_f32_u cluster_info_f32[cluster_num];
modbus_float_u cell_vol[cluster_num][TOTOL_BAT_num];
modbus_float_u cell_temp[cluster_num][TOTOL_BAT_num];
modbus_float_u cell_soc[cluster_num][TOTOL_BAT_num];
modbus_float_u cell_resistance[cluster_num][TOTOL_BAT_num];
modbus_float_u cell_soh[cluster_num][TOTOL_BAT_num];

agile_modbus_slave_util_map_t cluster_input_regs[cluster_num][7];

void cluster_input_regs_init(void) {
  for (int i = 0; i < cluster_num; i++) {
    cluster_input_regs[i][0].start_addr = 0x01;
    cluster_input_regs[i][0].end_addr = 0x11;
    cluster_input_regs[i][0].data = cluster_info_u16[i].reg;
    cluster_input_regs[i][0].data_len = sizeof(cluster_info_u16[i].reg);
    cluster_input_regs[i][0].data_type = MODBUS_UINT16;

    cluster_input_regs[i][1].start_addr = 0x12;
    cluster_input_regs[i][1].end_addr = 0x55;
    cluster_input_regs[i][1].data = cluster_info_f32[i].reg;
    cluster_input_regs[i][1].data_len = sizeof(cluster_info_f32[i].reg);
    cluster_input_regs[i][1].data_type = MODBUS_FLOAT;

    // debug only
    // for (int j = 0, f = 0; j < TOTOL_BAT_num; j++, f++) {
    //   cell_vol[i][j].f32 = f;
    // }

    cluster_input_regs[i][2].start_addr = 0xC9;
    cluster_input_regs[i][2].end_addr = 0xC9 + TOTOL_BAT_num * 2 - 1;
    cluster_input_regs[i][2].data = cell_vol[i];
    cluster_input_regs[i][2].data_len = sizeof(cell_vol[i]);
    cluster_input_regs[i][2].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][3].start_addr = 0x579;
    cluster_input_regs[i][3].end_addr = 0x579 + TOTOL_BAT_num * 2 - 1;
    cluster_input_regs[i][3].data = cell_temp[i];
    cluster_input_regs[i][3].data_len = sizeof(cell_temp[i]);
    cluster_input_regs[i][3].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][4].start_addr = 0xA29;
    cluster_input_regs[i][4].end_addr = 0xA29 + TOTOL_BAT_num * 2 - 1;
    cluster_input_regs[i][4].data = cell_soc[i];
    cluster_input_regs[i][4].data_len = sizeof(cell_soc[i]);
    cluster_input_regs[i][4].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][5].start_addr = 0xED9;
    cluster_input_regs[i][5].end_addr = 0xED9 + TOTOL_BAT_num * 2 - 1;
    cluster_input_regs[i][5].data = cell_resistance[i];
    cluster_input_regs[i][5].data_len = sizeof(cell_resistance[i]);
    cluster_input_regs[i][5].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][6].start_addr = 0x1389;
    cluster_input_regs[i][6].end_addr = 0x1389 + TOTOL_BAT_num * 2 - 1;
    cluster_input_regs[i][6].data = cell_soh[i];
    cluster_input_regs[i][6].data_len = sizeof(cell_soh[i]);
    cluster_input_regs[i][6].data_type = MODBUS_FLOAT;
  }
}

cluster_warning_u cluster_warning[cluster_num];
uint8_t cell_charge_balance_status[cluster_num][TOTOL_BAT_num];
uint8_t cell_discharge_balance_status[cluster_num][TOTOL_BAT_num];

agile_modbus_slave_util_map_t cluster_input_bit_regs[cluster_num][3];

void cluster_input_bit_regs_init(void) {
  for (int i = 0; i < cluster_num; i++) {
    cluster_input_bit_regs[i][0].start_addr = 0x01;
    cluster_input_bit_regs[i][0].end_addr = 0x41;
    cluster_input_bit_regs[i][0].data = cluster_warning[i].reg;
    cluster_input_bit_regs[i][0].data_len = sizeof(cluster_warning[i].reg);
    cluster_input_bit_regs[i][0].data_type = MODBUS_BIT;

    // debug only
    // for (int j = 0; j < TOTOL_BAT_num; j++)
    // {
    //   cell_charge_balance_status[i][j] = j % 2;
    // }

    cluster_input_bit_regs[i][1].start_addr = 0x65;
    cluster_input_bit_regs[i][1].end_addr = 0x65 + TOTOL_BAT_num - 1;
    cluster_input_bit_regs[i][1].data = cell_charge_balance_status[i];
    cluster_input_bit_regs[i][1].data_len =
        sizeof(cell_charge_balance_status[i]);
    cluster_input_bit_regs[i][1].data_type = MODBUS_BIT;

    cluster_input_bit_regs[i][2].start_addr = 0x2BD;
    cluster_input_bit_regs[i][2].end_addr = 0x2BD + TOTOL_BAT_num - 1;
    cluster_input_bit_regs[i][2].data = cell_discharge_balance_status[i];
    cluster_input_bit_regs[i][2].data_len =
        sizeof(cell_discharge_balance_status[i]);
    cluster_input_bit_regs[i][2].data_type = MODBUS_BIT;
  }
}

agile_modbus_slave_util_t cluster_info[cluster_num];

void cluster_info_init(void) {
  cluster_input_regs_init();
  cluster_input_bit_regs_init();

  for (int i = 0; i < cluster_num; i++) {
    cluster_info[i].tab_bits = NULL;
    cluster_info[i].nb_bits = 0;
    cluster_info[i].tab_input_bits = cluster_input_bit_regs[i];
    cluster_info[i].nb_input_bits = sizeof(cluster_input_bit_regs[i]) /
                                    sizeof(cluster_input_bit_regs[i][0]);
    cluster_info[i].tab_registers = NULL;
    cluster_info[i].nb_registers = 0;
    cluster_info[i].tab_input_registers = cluster_input_regs[i];
    cluster_info[i].nb_input_registers =
        sizeof(cluster_input_regs[i]) / sizeof(cluster_input_regs[i][0]);
    cluster_info[i].get = get_map_buf;
    cluster_info[i].set = set_map_buf;
    cluster_info[i].addr_check = NULL;
    cluster_info[i].special_function = NULL;
    cluster_info[i].done = NULL;
  }
}

bsmu_modbus_data_t bsmu_modbus_data[cluster_num + 1];

void bsmu_modbus_data_init(void) {
  cluster_info_init();

  bsmu_modbus_data[0].slave_addr = 1;
  bsmu_modbus_data[0].slave_util = &station_info;

  for (int i = 0; i < cluster_num; i++) {
    bsmu_modbus_data[i + 1].slave_addr = i + 2;
    bsmu_modbus_data[i + 1].slave_util = &cluster_info[i];
  }

  // set protocol version
  station_info_u16.data.protocol_version = 1;
}
