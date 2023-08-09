#include "modbus_data.h"
#include "modbus_slave.h"
#include "string.h"

static int get_map_buf(const agile_modbus_slave_util_map_t *map, void *buf,
                       int bufsz) {
  modbus_data_type_e modbus_data_type = map->data_type;
  int len = bufsz;

  if (bufsz > map->data_len) {
    len = map->data_len;
  }

  switch (modbus_data_type) {
  case MODBUS_FLOAT: {
    uint16_t *ptr = (uint16_t *)buf;
    modbus_float_u *data = map->data;

    for (int i = 0; i < len / sizeof(float); i++) {
      // Note: Modbus transfers data in big-endian, which needed to be
      // swapped to fit the little-endian ISA.
      ptr[i * 2] = data[i].u16[1];
      ptr[i * 2 + 1] = data[i].u16[0];
    }
    break;
  }
  default:
    memcpy(buf, map->data, len);
    break;
  }

  return 0;
}

static int set_map_buf(const agile_modbus_slave_util_map_t *map, int index,
                       int len, void *buf, int bufsz) {
  modbus_data_type_e modbus_data_type = map->data_type;
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

  memcpy(map->data, buf, len * size);

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

agile_modbus_slave_util_t station_info = {NULL,
                                          0,
                                          NULL,
                                          0,
                                          NULL,
                                          0,
                                          station_input_regs_map,
                                          sizeof(station_input_regs_map) /
                                              sizeof(station_input_regs_map[0]),
                                          get_map_buf,
                                          set_map_buf,
                                          NULL,
                                          NULL,
                                          NULL};

cluster_info_u16_u cluster_info_u16[20];
cluster_info_f32_u cluster_info_f32[20];
modbus_float_u cell_vol[20][360];
modbus_float_u cell_temp[20][360];
modbus_float_u cell_soc[20][360];
modbus_float_u cell_resistance[20][360];
modbus_float_u cell_soh[20][360];

agile_modbus_slave_util_map_t cluster_input_regs[20][7];

void cluster_input_regs_init(void) {
  for (int i = 0; i < 20; i++) {
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

    cluster_input_regs[i][2].start_addr = 0xC9;
    cluster_input_regs[i][2].end_addr = 0x398;
    cluster_input_regs[i][2].data = cell_vol[i];
    cluster_input_regs[i][2].data_len = sizeof(cell_vol[i]);
    cluster_input_regs[i][2].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][3].start_addr = 0x579;
    cluster_input_regs[i][3].end_addr = 0x848;
    cluster_input_regs[i][3].data = cell_temp[i];
    cluster_input_regs[i][3].data_len = sizeof(cell_temp[i]);
    cluster_input_regs[i][3].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][4].start_addr = 0xA29;
    cluster_input_regs[i][4].end_addr = 0xCF8;
    cluster_input_regs[i][4].data = cell_soc[i];
    cluster_input_regs[i][4].data_len = sizeof(cell_soc[i]);
    cluster_input_regs[i][4].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][5].start_addr = 0xED9;
    cluster_input_regs[i][5].end_addr = 0x11A8;
    cluster_input_regs[i][5].data = cell_resistance[i];
    cluster_input_regs[i][5].data_len = sizeof(cell_resistance[i]);
    cluster_input_regs[i][5].data_type = MODBUS_FLOAT;

    cluster_input_regs[i][6].start_addr = 0x1389;
    cluster_input_regs[i][6].end_addr = 0x1658;
    cluster_input_regs[i][6].data = cell_soh[i];
    cluster_input_regs[i][6].data_len = sizeof(cell_soh[i]);
    cluster_input_regs[i][6].data_type = MODBUS_FLOAT;
  }
}

cluster_warning_u cluster_warning[20];
uint8_t cell_charge_balance_status[20][360];
uint8_t cell_discharge_balance_status[20][360];

agile_modbus_slave_util_map_t cluster_input_bit_regs[20][3];

void cluster_input_bit_regs_init(void) {
  for (int i = 0; i < 20; i++) {
    cluster_input_bit_regs[i][0].start_addr = 0x01;
    cluster_input_bit_regs[i][0].end_addr = 0x41;
    cluster_input_bit_regs[i][0].data = cluster_warning[i].reg;
    cluster_input_bit_regs[i][0].data_len = sizeof(cluster_warning[i].reg);
    cluster_input_bit_regs[i][0].data_type = MODBUS_BIT;

    cluster_input_bit_regs[i][1].start_addr = 0x65;
    cluster_input_bit_regs[i][1].end_addr = 0x1CC;
    cluster_input_bit_regs[i][1].data = cell_charge_balance_status[i];
    cluster_input_bit_regs[i][1].data_len =
        sizeof(cell_charge_balance_status[i]);
    cluster_input_bit_regs[i][1].data_type = MODBUS_BIT;

    cluster_input_bit_regs[i][2].start_addr = 0x2BD;
    cluster_input_bit_regs[i][2].end_addr = 0x424;
    cluster_input_bit_regs[i][2].data = cell_discharge_balance_status[i];
    cluster_input_bit_regs[i][2].data_len =
        sizeof(cell_discharge_balance_status[i]);
    cluster_input_bit_regs[i][2].data_type = MODBUS_BIT;
  }
}

agile_modbus_slave_util_t cluster_info[20];

void cluster_info_init(void) {
  cluster_input_regs_init();
  cluster_input_bit_regs_init();

  for (int i = 0; i < 20; i++) {
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

bsmu_modbus_data_t bsmu_modbus_data[21];

void bsmu_modbus_data_init(void) {
  cluster_info_init();

  bsmu_modbus_data[0].slave_addr = 1;
  bsmu_modbus_data[0].slave_util = &station_info;

  for (int i = 0; i < 20; i++) {
    bsmu_modbus_data[i + 1].slave_addr = i + 2;
    bsmu_modbus_data[i + 1].slave_util = &cluster_info[i];
  }
}
