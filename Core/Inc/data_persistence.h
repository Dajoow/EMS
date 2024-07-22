#ifndef __DATA_PERSISTENCE_H__
#define __DATA_PERSISTENCE_H__

typedef struct 
{
  float charging_sum;
  float discharging_sum;
} cluster_flash_t;

typedef struct 
{
  // WARN < ALARM < PROTECT
float clu_high_volt_warn;                   
float clu_high_volt_alarm;                  
float clu_high_volt_protect;               
float clu_low_volt_warn;                   
float clu_low_volt_alarm;                  
float clu_low_volt_protect;                
float clu_high_charge_cur_warn;            
float clu_high_charge_cur_alarm;           
float clu_high_charge_cur_protect;         
float clu_high_discharge_cur_warn;         
float clu_high_discharge_cur_alarm;        
float clu_high_discharge_cur_protect;      
float cell_high_volt_warn;                 
float cell_high_volt_alarm;                
float cell_high_volt_protect;              
float cell_low_volt_warn;                  
float cell_low_volt_alarm;                 
float cell_low_volt_protect;               
float clu_high_charge_temp_warn;           
float clu_high_charge_temp_alarm;          
float clu_high_charge_temp_protect;        
float clu_low_charge_temp_warn;            
float clu_low_charge_temp_alarm;
float clu_low_charge_temp_protect;
float clu_high_discharge_temp_warn;        
float clu_high_discharge_temp_alarm;       
float clu_high_discharge_temp_protect;     
float clu_low_discharge_temp_warn;         
float clu_low_discharge_temp_alarm;
float clu_low_discharge_temp_protect;
float cell_high_temp_warn;                 
float cell_high_temp_alarm;                
float cell_high_temp_protect;              
float cell_low_temp_warn;                  
float cell_low_temp_alarm;
float cell_low_temp_protect;
// float env_high_temp_warn;                  
// float env_high_temp_alarm;                 
// float env_high_temp_protect;               
// float env_low_temp_warn;
// float env_low_temp_alarm;
// float env_low_temp_protect;
// float pwrline_high_temp_warn;              
// float pwrline_high_temp_alarm;             
// float pwrline_high_temp_protect;           
float clu_high_soc_warn;                   
float clu_high_soc_alarm;                  
float clu_high_soc_protect;                
float clu_low_soc_warn;
float clu_low_soc_alarm;
float clu_low_soc_protect;
float clu_insul_res_p_low_warn;
float clu_insul_res_p_low_alarm;
float clu_insul_res_p_low_protect;
float clu_insul_res_n_low_warn;
float clu_insul_res_n_low_alarm;
float clu_insul_res_n_low_protect;
} modbus_threshold_flash_t;

void modbus_threshold_save(const modbus_threshold_flash_t * obj);
void flash_save_init (void);

#endif
