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
