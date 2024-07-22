#include "data_persistence.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "easyflash.h"
#include "station_ctl.h"


osThreadId flash_save_task_handle;

cluster_flash_t cluster_flash[cluster_num];
modbus_threshold_flash_t modbus_threshold_flash;

void modbus_threshold_save(const modbus_threshold_flash_t * obj){
    ef_set_env_blob("modbus_threshold_flash", obj, sizeof(modbus_threshold_flash_t));
}

void flash_save(void const * args){
    while(1){
        osDelay(30 * 60 * 1000); // 30min

        ef_set_env_blob("cluster_falsh", cluster_flash, sizeof(cluster_flash));
    }
}

void flash_save_init (void){
    size_t len;
    
    easyflash_init();

    ef_get_env_blob("cluster_falsh", cluster_flash, sizeof(cluster_flash), &len);
    if (len == 0){
        ef_set_env_blob("cluster_falsh", cluster_flash, sizeof(cluster_flash));
    }

    ef_get_env_blob("modbus_threshold_flash", &modbus_threshold_flash, sizeof(modbus_threshold_flash), &len);
    if (len == 0){
        ef_set_env_blob("modbus_threshold_flash", &modbus_threshold_flash, sizeof(modbus_threshold_flash));
    }

    osThreadDef(flash_save_task,flash_save , osPriorityNormal, 0, 256);
    flash_save_task_handle = osThreadCreate(osThread(flash_save_task), NULL);
}
