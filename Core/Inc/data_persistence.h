#ifndef __DATA_PERSISTENCE_H__
#define __DATA_PERSISTENCE_H__

typedef struct 
{
  float charging_sum;
  float discharging_sum;
} cluster_flash_t;

void flash_save_init (void);

#endif
