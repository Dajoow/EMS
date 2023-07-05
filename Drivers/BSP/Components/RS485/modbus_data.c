#include "modbus_data.h"
#include "modbus_slave.h"
#include "string.h"

static int
get_map_buf (const agile_modbus_slave_util_map_t *map, void *buf, int bufsz)
{
  modbus_data_type_e modbus_data_type = map->data_type;

  switch (modbus_data_type)
    {
    case MODBUS_FLOAT:
      {
        uint16_t *ptr = (uint16_t *)buf;
        modbus_float_u *data = map->data;

        for (int i = 0; i < map->data_len / sizeof (float); i++)
          {
            // Note: Modbus transfers data in big-endian, which needed to be
            // swapped to fit the little-endian ISA.
            ptr[i * 2] = data[i].u16[1];
            ptr[i * 2 + 1] = data[i].u16[0];
          }
        break;
      }
    default:
      memcpy (buf, map->data, map->data_len);
      break;
    }

  return 0;
}

static int
set_map_buf (const agile_modbus_slave_util_map_t *map, int index, int len, void *buf,
             int bufsz)
{
  modbus_data_type_e modbus_data_type = map->data_type;
  size_t size = 0;

  switch (modbus_data_type)
    {
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

  memcpy (map->data, buf, len * size);

  return 0;
}

uint16_t cellstack_info_u16[20] = { 1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                    11, 12, 13, 14, 15, 16, 17, 18, 19, 20 };
modbus_float_u cellstack_info_f32[26] = {
  { .f32 = 1. }, { .f32 = 2. }, { .f32 = 3. }, { .f32 = 4. }, { .f32 = 26. }
};
uint16_t system_version[18];

agile_modbus_slave_util_map_t system_input_regs_map[] = {
  // 电池堆数据 UINT16 RO
  {
      .start_addr = 0x01,
      .end_addr = 0x14,
      .data = cellstack_info_u16,
      .data_len = sizeof (cellstack_info_u16),
      .data_type = MODBUS_UINT16,
  },
  // 电池堆数据 FLOAT RO
  {
      .start_addr = 0x33,
      .end_addr = 0x66,
      .data = cellstack_info_f32,
      .data_len = sizeof (cellstack_info_f32),
      .data_type = MODBUS_FLOAT,
  },
  // 版本信息 UINT16 RO
  {
      .start_addr = 0x1f41,
      .end_addr = 0x1f52,
      .data = system_version,
      .data_len = sizeof (system_version),
      .data_type = MODBUS_UINT16,
  }
};

uint8_t system_status[11]={0,0,1,0};

agile_modbus_slave_util_map_t system_input_bits_map[] = {
  // 系统告警 RO
  {
      .start_addr = 0x01,
      .end_addr = 0x0b,
      .data = system_status,
      .data_len = sizeof (system_status),
      .data_type = MODBUS_BIT,
  }
};

uint16_t system_config[11];

agile_modbus_slave_util_map_t system_regs_map[] = {
  // 电池堆信息设置 UINT16 RW
  {
      .start_addr = 0x01,
      .end_addr = 0x11,
      .data = system_config,
      .data_len = sizeof (system_config),
      .data_type = MODBUS_UINT16,
  }
};

agile_modbus_slave_util_t system_info
    = { NULL,
        0,
        system_input_bits_map,
        sizeof (system_input_bits_map) / sizeof (system_input_bits_map[0]),
        system_regs_map,
        sizeof (system_regs_map) / sizeof (system_regs_map[0]),
        system_input_regs_map,
        sizeof (system_input_regs_map) / sizeof (system_input_regs_map[0]),
        get_map_buf,
        set_map_buf,
        NULL,
        NULL,
        NULL };

agile_modbus_slave_util_t supervision_info = {

};

agile_modbus_slave_util_t cell_info[20] = { {

} };

bsmu_modbus_data_t bsmu_modbus_data[] = {
  {
      .slave_addr = 1,
      .slave_util = &system_info,
  },
  {
      .slave_addr = 101,
      .slave_util = &supervision_info,
  },
  {
      .slave_addr = 2,
      .slave_util = &cell_info[0],
  },
  {
      .slave_addr = 3,
      .slave_util = &cell_info[1],
  },
  {
      .slave_addr = 4,
      .slave_util = &cell_info[2],
  },
  {
      .slave_addr = 5,
      .slave_util = &cell_info[3],
  },
  {
      .slave_addr = 6,
      .slave_util = &cell_info[4],
  },
  {
      .slave_addr = 7,
      .slave_util = &cell_info[5],
  },
  {
      .slave_addr = 8,
      .slave_util = &cell_info[6],
  },
  {
      .slave_addr = 9,
      .slave_util = &cell_info[7],
  },
  {
      .slave_addr = 10,
      .slave_util = &cell_info[8],
  },
  {
      .slave_addr = 11,
      .slave_util = &cell_info[9],
  },
  {
      .slave_addr = 12,
      .slave_util = &cell_info[10],
  },
  {
      .slave_addr = 13,
      .slave_util = &cell_info[11],
  },
  {
      .slave_addr = 14,
      .slave_util = &cell_info[12],
  },
  {
      .slave_addr = 15,
      .slave_util = &cell_info[13],
  },
  {
      .slave_addr = 16,
      .slave_util = &cell_info[14],
  },
  {
      .slave_addr = 17,
      .slave_util = &cell_info[15],
  },
  {
      .slave_addr = 18,
      .slave_util = &cell_info[16],
  },
  {
      .slave_addr = 19,
      .slave_util = &cell_info[17],
  },
  {
      .slave_addr = 20,
      .slave_util = &cell_info[18],
  },
  {
      .slave_addr = 21,
      .slave_util = &cell_info[19],
  },
};
