#include <gui/modbus_threshold_setting_screen/Modbus_threshold_settingView.hpp>
#include <touchgfx/Utils.hpp>

#ifndef SIMULATOR
extern "C"{
  #include "data_persistence.h"

  extern modbus_threshold_flash_t modbus_threshold_flash;
};
#endif

#ifndef SIMULATOR
float simple_atof(const touchgfx::Unicode::UnicodeChar* str) {
    float result = 0.0f;
    float factor = 1.0f;
    bool decimal_point_encountered = false;
    float decimal_factor = 0.1f;

    // Handle optional sign
    if (*str == '-') {
        factor = -1.0f;
        str++;
    } else if (*str == '+') {
        str++;
    }

    while (*str) {
        if (*str >= '0' && *str <= '9') {
            if (!decimal_point_encountered) {
                result = result * 10.0f + (*str - '0');
            } else {
                result = result + (*str - '0') * decimal_factor;
                decimal_factor *= 0.1f;
            }
        } else if (*str == '.') {
            decimal_point_encountered = true;
        } else {
            break; // Invalid character, stop parsing
        }
        str++;
    }

    return result * factor;
}
#endif

Modbus_threshold_settingView::Modbus_threshold_settingView()
{

}

void Modbus_threshold_settingView::setupScreen()
{
    Modbus_threshold_settingViewBase::setupScreen();

#ifndef SIMULATOR
    Unicode::snprintfFloats(clu_heigh_volt_warn_btnBuffer, CLU_HEIGH_VOLT_WARN_BTN_SIZE, "%.2f", &modbus_threshold_flash.clu_high_volt_warn);
    Unicode::snprintfFloats(clu_heigh_volt_alarm_btnBuffer, CLU_HEIGH_VOLT_ALARM_BTN_SIZE, "%.2f", &modbus_threshold_flash.clu_high_volt_alarm);
    Unicode::snprintfFloats(clu_heigh_volt_protect_btnBuffer, CLU_HEIGH_VOLT_PROTECT_BTN_SIZE, "%.2f", &modbus_threshold_flash.clu_high_volt_protect);
    Unicode::snprintfFloats(clu_low_volt_warn_btn_1Buffer, CLU_LOW_VOLT_WARN_BTN_1_SIZE, "%.2f", &modbus_threshold_flash.clu_low_volt_warn);
    Unicode::snprintfFloats(clu_low_volt_alarm_btn_1Buffer, CLU_LOW_VOLT_ALARM_BTN_1_SIZE, "%.2f", &modbus_threshold_flash.clu_low_volt_alarm);
    Unicode::snprintfFloats(clu_low_volt_protect_btn_1Buffer, CLU_LOW_VOLT_PROTECT_BTN_1_SIZE, "%.2f", &modbus_threshold_flash.clu_low_volt_protect);
    Unicode::snprintfFloats(clu_heigh_charge_cur_warn_btn_2Buffer, CLU_HEIGH_CHARGE_CUR_WARN_BTN_2_SIZE, "%.2f", &modbus_threshold_flash.clu_high_charge_cur_warn);
    Unicode::snprintfFloats(clu_heigh_charge_cur_alarm_btn_2Buffer, CLU_HEIGH_CHARGE_CUR_ALARM_BTN_2_SIZE, "%.2f", &modbus_threshold_flash.clu_high_charge_cur_alarm);
    Unicode::snprintfFloats(clu_heigh_charge_cur_protect_btn_2Buffer, CLU_HEIGH_CHARGE_CUR_PROTECT_BTN_2_SIZE, "%.2f", &modbus_threshold_flash.clu_high_charge_cur_protect);
    Unicode::snprintfFloats(clu_heigh_discharge_cur_warn_btn_3Buffer, CLU_HEIGH_DISCHARGE_CUR_WARN_BTN_3_SIZE, "%.2f", &modbus_threshold_flash.clu_high_discharge_cur_warn);
    Unicode::snprintfFloats(clu_heigh_discharge_cur_alarm_btn_3Buffer, CLU_HEIGH_DISCHARGE_CUR_ALARM_BTN_3_SIZE, "%.2f", &modbus_threshold_flash.clu_high_discharge_cur_alarm);
    Unicode::snprintfFloats(clu_heigh_discharge_cur_protect_btn_3Buffer, CLU_HEIGH_DISCHARGE_CUR_PROTECT_BTN_3_SIZE, "%.2f", &modbus_threshold_flash.clu_high_discharge_cur_protect);
    Unicode::snprintfFloats(cell_heigh_volt_warn_btn_4Buffer, CELL_HEIGH_VOLT_WARN_BTN_4_SIZE, "%.2f", &modbus_threshold_flash.cell_high_volt_warn);
    Unicode::snprintfFloats(cell_heigh_volt_alarm_btn_4Buffer, CELL_HEIGH_VOLT_ALARM_BTN_4_SIZE, "%.2f", &modbus_threshold_flash.cell_high_volt_alarm);
    Unicode::snprintfFloats(cell_heigh_volt_protect_btn_4Buffer, CELL_HEIGH_VOLT_PROTECT_BTN_4_SIZE, "%.2f", &modbus_threshold_flash.cell_high_volt_protect);
    Unicode::snprintfFloats(cell_low_volt_warn_btn_5Buffer, CELL_LOW_VOLT_WARN_BTN_5_SIZE, "%.2f", &modbus_threshold_flash.cell_low_volt_warn);
    Unicode::snprintfFloats(cell_low_volt_alarm_btn_5Buffer, CELL_LOW_VOLT_ALARM_BTN_5_SIZE, "%.2f", &modbus_threshold_flash.cell_low_volt_alarm);
    Unicode::snprintfFloats(cell_low_volt_protect_btn_5Buffer, CELL_LOW_VOLT_PROTECT_BTN_5_SIZE, "%.2f", &modbus_threshold_flash.cell_low_volt_protect);
    Unicode::snprintfFloats(clu_heigh_charge_temp_warn_btn_6Buffer, CLU_HEIGH_CHARGE_TEMP_WARN_BTN_6_SIZE, "%.2f", &modbus_threshold_flash.clu_high_charge_temp_warn);
    Unicode::snprintfFloats(clu_heigh_charge_temp_alarm_btn_6Buffer, CLU_HEIGH_CHARGE_TEMP_ALARM_BTN_6_SIZE, "%.2f", &modbus_threshold_flash.clu_high_charge_temp_alarm);
    Unicode::snprintfFloats(clu_heigh_charge_temp_protect_btn_6Buffer, CLU_HEIGH_CHARGE_TEMP_PROTECT_BTN_6_SIZE, "%.2f", &modbus_threshold_flash.clu_high_charge_temp_protect);
    Unicode::snprintfFloats(clu_low_charge_temp_warn_btn_7Buffer, CLU_LOW_CHARGE_TEMP_WARN_BTN_7_SIZE, "%.2f", &modbus_threshold_flash.clu_low_charge_temp_warn);
    Unicode::snprintfFloats(clu_low_charge_temp_alarm_btn_7Buffer, CLU_LOW_CHARGE_TEMP_ALARM_BTN_7_SIZE, "%.2f", &modbus_threshold_flash.clu_low_charge_temp_alarm);
    Unicode::snprintfFloats(clu_low_charge_temp_protect_btn_7Buffer, CLU_LOW_CHARGE_TEMP_PROTECT_BTN_7_SIZE, "%.2f", &modbus_threshold_flash.clu_low_charge_temp_protect);
    Unicode::snprintfFloats(clu_heigh_discharge_temp_warn_btn_8Buffer, CLU_HEIGH_DISCHARGE_TEMP_WARN_BTN_8_SIZE, "%.2f", &modbus_threshold_flash.clu_high_discharge_temp_warn);
    Unicode::snprintfFloats(clu_heigh_discharge_temp_alarm_btn_8Buffer, CLU_HEIGH_DISCHARGE_TEMP_ALARM_BTN_8_SIZE, "%.2f", &modbus_threshold_flash.clu_high_discharge_temp_alarm);
    Unicode::snprintfFloats(clu_heigh_discharge_temp_protect_btn_8Buffer, CLU_HEIGH_DISCHARGE_TEMP_PROTECT_BTN_8_SIZE, "%.2f", &modbus_threshold_flash.clu_high_discharge_temp_protect);
    Unicode::snprintfFloats(clu_low_discharge_temp_warn_btn_9Buffer, CLU_LOW_DISCHARGE_TEMP_WARN_BTN_9_SIZE, "%.2f", &modbus_threshold_flash.clu_low_discharge_temp_warn);
    Unicode::snprintfFloats(clu_low_discharge_temp_alarm_btn_9Buffer, CLU_LOW_DISCHARGE_TEMP_ALARM_BTN_9_SIZE, "%.2f", &modbus_threshold_flash.clu_low_discharge_temp_alarm);
    Unicode::snprintfFloats(clu_low_discharge_temp_protect_btn_9Buffer, CLU_LOW_DISCHARGE_TEMP_PROTECT_BTN_9_SIZE, "%.2f", &modbus_threshold_flash.clu_low_discharge_temp_protect);
    Unicode::snprintfFloats(cell_heigh_temp_warn_btn_10Buffer, CELL_HEIGH_TEMP_WARN_BTN_10_SIZE, "%.2f", &modbus_threshold_flash.cell_high_temp_warn);
    Unicode::snprintfFloats(cell_heigh_temp_alarm_btn_10Buffer, CELL_HEIGH_TEMP_ALARM_BTN_10_SIZE, "%.2f", &modbus_threshold_flash.cell_high_temp_alarm);
    Unicode::snprintfFloats(cell_heigh_temp_protect_btn_10Buffer, CELL_HEIGH_TEMP_PROTECT_BTN_10_SIZE, "%.2f", &modbus_threshold_flash.cell_high_temp_protect);
    Unicode::snprintfFloats(cell_low_temp_warn_btn_11Buffer, CELL_LOW_TEMP_WARN_BTN_11_SIZE, "%.2f", &modbus_threshold_flash.cell_low_temp_warn);
    Unicode::snprintfFloats(cell_low_temp_alarm_btn_11Buffer, CELL_LOW_TEMP_ALARM_BTN_11_SIZE, "%.2f", &modbus_threshold_flash.cell_low_temp_alarm);
    Unicode::snprintfFloats(cell_low_temp_protect_btn_11Buffer, CELL_LOW_TEMP_PROTECT_BTN_11_SIZE, "%.2f", &modbus_threshold_flash.cell_low_temp_protect);
    Unicode::snprintfFloats(clu_heigh_soc_warn_btn_12Buffer, CLU_HEIGH_SOC_WARN_BTN_12_SIZE, "%.2f", &modbus_threshold_flash.clu_high_soc_warn);
    Unicode::snprintfFloats(clu_heigh_soc_alarm_btn_12Buffer, CLU_HEIGH_SOC_ALARM_BTN_12_SIZE, "%.2f", &modbus_threshold_flash.clu_high_soc_alarm);
    Unicode::snprintfFloats(clu_heigh_soc_protect_btn_12Buffer, CLU_HEIGH_SOC_PROTECT_BTN_12_SIZE, "%.2f", &modbus_threshold_flash.clu_high_soc_protect);
    Unicode::snprintfFloats(clu_low_soc_warn_btn_13Buffer, CLU_LOW_SOC_WARN_BTN_13_SIZE, "%.2f", &modbus_threshold_flash.clu_low_soc_warn);
    Unicode::snprintfFloats(clu_low_soc_alarm_btn_13Buffer, CLU_LOW_SOC_ALARM_BTN_13_SIZE, "%.2f", &modbus_threshold_flash.clu_low_soc_alarm);
    Unicode::snprintfFloats(clu_low_soc_protect_btn_13Buffer, CLU_LOW_SOC_PROTECT_BTN_13_SIZE, "%.2f", &modbus_threshold_flash.clu_low_soc_protect);
    Unicode::snprintfFloats(clu_insul_res_p_low_warn_btn_14Buffer, CLU_INSUL_RES_P_LOW_WARN_BTN_14_SIZE, "%.2f", &modbus_threshold_flash.clu_insul_res_p_low_warn);
    Unicode::snprintfFloats(clu_insul_res_p_low_alarm_btn_14Buffer, CLU_INSUL_RES_P_LOW_ALARM_BTN_14_SIZE, "%.2f", &modbus_threshold_flash.clu_insul_res_p_low_alarm);
    Unicode::snprintfFloats(clu_insul_res_p_low_protect_btn_14Buffer, CLU_INSUL_RES_P_LOW_PROTECT_BTN_14_SIZE, "%.2f", &modbus_threshold_flash.clu_insul_res_p_low_protect);
    Unicode::snprintfFloats(clu_insul_res_n_low_warn_btn_15Buffer, CLU_INSUL_RES_N_LOW_WARN_BTN_15_SIZE, "%.2f", &modbus_threshold_flash.clu_insul_res_n_low_warn);
    Unicode::snprintfFloats(clu_insul_res_n_low_alarm_btn_15Buffer, CLU_INSUL_RES_N_LOW_ALARM_BTN_15_SIZE, "%.2f", &modbus_threshold_flash.clu_insul_res_n_low_alarm);
    Unicode::snprintfFloats(clu_insul_res_n_low_protect_btn_15Buffer, CLU_INSUL_RES_N_LOW_PROTECT_BTN_15_SIZE, "%.2f", &modbus_threshold_flash.clu_insul_res_n_low_protect);
#endif
}

void Modbus_threshold_settingView::tearDownScreen()
{
    Modbus_threshold_settingViewBase::tearDownScreen();
}

void Modbus_threshold_settingView::clu_heigh_vol_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_volt_warn_btnBuffer, CLU_HEIGH_VOLT_WARN_BTN_SIZE, &clu_heigh_volt_warn_btn);
}

void Modbus_threshold_settingView::clu_heigh_vol_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_volt_alarm_btnBuffer, CLU_HEIGH_VOLT_ALARM_BTN_SIZE, &clu_heigh_volt_alarm_btn);
}

void Modbus_threshold_settingView::clu_heigh_vol_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_volt_protect_btnBuffer, CLU_HEIGH_VOLT_PROTECT_BTN_SIZE, &clu_heigh_volt_protect_btn);
}

void Modbus_threshold_settingView::clu_low_vol_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_volt_warn_btn_1Buffer, CLU_LOW_VOLT_WARN_BTN_1_SIZE, &clu_low_volt_warn_btn_1);
}

void Modbus_threshold_settingView::clu_low_vol_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_volt_alarm_btn_1Buffer, CLU_LOW_VOLT_ALARM_BTN_1_SIZE, &clu_low_volt_alarm_btn_1);
}

void Modbus_threshold_settingView::clu_low_vol_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_volt_protect_btn_1Buffer, CLU_LOW_VOLT_PROTECT_BTN_1_SIZE, &clu_low_volt_protect_btn_1);
}

void Modbus_threshold_settingView::clu_heigh_charge_cur_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_charge_cur_warn_btn_2Buffer, CLU_HEIGH_CHARGE_CUR_WARN_BTN_2_SIZE, &clu_heigh_charge_cur_warn_btn_2);
}

void Modbus_threshold_settingView::clu_heigh_charge_cur_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_charge_cur_alarm_btn_2Buffer, CLU_HEIGH_CHARGE_CUR_ALARM_BTN_2_SIZE, &clu_heigh_charge_cur_alarm_btn_2);
}

void Modbus_threshold_settingView::clu_heigh_charge_cur_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_charge_cur_protect_btn_2Buffer, CLU_HEIGH_CHARGE_CUR_PROTECT_BTN_2_SIZE, &clu_heigh_charge_cur_protect_btn_2);
}

void Modbus_threshold_settingView::clu_heigh_discharge_cur_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_discharge_cur_warn_btn_3Buffer, CLU_HEIGH_DISCHARGE_CUR_WARN_BTN_3_SIZE, &clu_heigh_discharge_cur_warn_btn_3);
}

void Modbus_threshold_settingView::clu_heigh_discharge_cur_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_discharge_cur_alarm_btn_3Buffer, CLU_HEIGH_DISCHARGE_CUR_ALARM_BTN_3_SIZE, &clu_heigh_discharge_cur_alarm_btn_3);
}

void Modbus_threshold_settingView::clu_heigh_discharge_cur_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_discharge_cur_protect_btn_3Buffer, CLU_HEIGH_DISCHARGE_CUR_PROTECT_BTN_3_SIZE, &clu_heigh_discharge_cur_protect_btn_3);
}

void Modbus_threshold_settingView::cell_heigh_volt_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_heigh_volt_warn_btn_4Buffer, CELL_HEIGH_VOLT_WARN_BTN_4_SIZE, &cell_heigh_volt_warn_btn_4);
}

void Modbus_threshold_settingView::cell_heigh_volt_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_heigh_volt_alarm_btn_4Buffer, CELL_HEIGH_VOLT_ALARM_BTN_4_SIZE, &cell_heigh_volt_alarm_btn_4);
}

void Modbus_threshold_settingView::cell_heigh_volt_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_heigh_volt_protect_btn_4Buffer, CELL_HEIGH_VOLT_PROTECT_BTN_4_SIZE, &cell_heigh_volt_protect_btn_4);
}

void Modbus_threshold_settingView::cell_low_volt_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_low_volt_warn_btn_5Buffer, CELL_LOW_VOLT_WARN_BTN_5_SIZE, &cell_low_volt_warn_btn_5);
}

void Modbus_threshold_settingView::cell_low_volt_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_low_volt_alarm_btn_5Buffer, CELL_LOW_VOLT_ALARM_BTN_5_SIZE, &cell_low_volt_alarm_btn_5);
}

void Modbus_threshold_settingView::cell_low_volt_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_low_volt_protect_btn_5Buffer, CELL_LOW_VOLT_PROTECT_BTN_5_SIZE, &cell_low_volt_protect_btn_5);
}

void Modbus_threshold_settingView::clu_heigh_charge_temp_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_charge_temp_warn_btn_6Buffer, CLU_HEIGH_CHARGE_TEMP_WARN_BTN_6_SIZE, &clu_heigh_charge_temp_warn_btn_6);
}

void Modbus_threshold_settingView::clu_heigh_charge_temp_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_charge_temp_alarm_btn_6Buffer, CLU_HEIGH_CHARGE_TEMP_ALARM_BTN_6_SIZE, &clu_heigh_charge_temp_alarm_btn_6);
}

void Modbus_threshold_settingView::clu_heigh_charge_temp_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_charge_temp_protect_btn_6Buffer, CLU_HEIGH_CHARGE_TEMP_PROTECT_BTN_6_SIZE, &clu_heigh_charge_temp_protect_btn_6);
}

void Modbus_threshold_settingView::clu_low_charge_temp_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_charge_temp_warn_btn_7Buffer, CLU_LOW_CHARGE_TEMP_WARN_BTN_7_SIZE, &clu_low_charge_temp_warn_btn_7);
}

void Modbus_threshold_settingView::clu_low_charge_temp_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_charge_temp_alarm_btn_7Buffer, CLU_LOW_CHARGE_TEMP_ALARM_BTN_7_SIZE, &clu_low_charge_temp_alarm_btn_7);
}

void Modbus_threshold_settingView::clu_low_charge_temp_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_charge_temp_protect_btn_7Buffer, CLU_LOW_CHARGE_TEMP_PROTECT_BTN_7_SIZE, &clu_low_charge_temp_protect_btn_7);
}

void Modbus_threshold_settingView::clu_heigh_discharge_temp_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_discharge_temp_warn_btn_8Buffer, CLU_HEIGH_DISCHARGE_TEMP_WARN_BTN_8_SIZE, &clu_heigh_discharge_temp_warn_btn_8);
}

void Modbus_threshold_settingView::clu_heigh_discharge_temp_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_discharge_temp_alarm_btn_8Buffer, CLU_HEIGH_DISCHARGE_TEMP_ALARM_BTN_8_SIZE, &clu_heigh_discharge_temp_alarm_btn_8);
}

void Modbus_threshold_settingView::clu_heigh_discharge_temp_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_discharge_temp_protect_btn_8Buffer, CLU_HEIGH_DISCHARGE_TEMP_PROTECT_BTN_8_SIZE, &clu_heigh_discharge_temp_protect_btn_8);
}

void Modbus_threshold_settingView::clu_low_discharge_temp_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_discharge_temp_warn_btn_9Buffer, CLU_LOW_DISCHARGE_TEMP_WARN_BTN_9_SIZE, &clu_low_discharge_temp_warn_btn_9);
}

void Modbus_threshold_settingView::clu_low_discharge_temp_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_discharge_temp_alarm_btn_9Buffer, CLU_LOW_DISCHARGE_TEMP_ALARM_BTN_9_SIZE, &clu_low_discharge_temp_alarm_btn_9);
}

void Modbus_threshold_settingView::clu_low_discharge_temp_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_discharge_temp_protect_btn_9Buffer, CLU_LOW_DISCHARGE_TEMP_PROTECT_BTN_9_SIZE, &clu_low_discharge_temp_protect_btn_9);
}

void Modbus_threshold_settingView::cell_heigh_temp_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_heigh_temp_warn_btn_10Buffer, CELL_HEIGH_TEMP_WARN_BTN_10_SIZE, &cell_heigh_temp_warn_btn_10);
}

void Modbus_threshold_settingView::cell_heigh_temp_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_heigh_temp_alarm_btn_10Buffer, CELL_HEIGH_TEMP_ALARM_BTN_10_SIZE, &cell_heigh_temp_alarm_btn_10);
}

void Modbus_threshold_settingView::cell_heigh_temp_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_heigh_temp_protect_btn_10Buffer, CELL_HEIGH_TEMP_PROTECT_BTN_10_SIZE, &cell_heigh_temp_protect_btn_10);
}

void Modbus_threshold_settingView::cell_low_temp_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_low_temp_warn_btn_11Buffer, CELL_LOW_TEMP_WARN_BTN_11_SIZE, &cell_low_temp_warn_btn_11);
}

void Modbus_threshold_settingView::cell_low_temp_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_low_temp_alarm_btn_11Buffer, CELL_LOW_TEMP_ALARM_BTN_11_SIZE, &cell_low_temp_alarm_btn_11);
}

void Modbus_threshold_settingView::cell_low_temp_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(cell_low_temp_protect_btn_11Buffer, CELL_LOW_TEMP_PROTECT_BTN_11_SIZE, &cell_low_temp_protect_btn_11);
}

void Modbus_threshold_settingView::clu_heigh_soc_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_soc_warn_btn_12Buffer, CLU_HEIGH_SOC_WARN_BTN_12_SIZE, &clu_heigh_soc_warn_btn_12);
}

void Modbus_threshold_settingView::clu_heigh_soc_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_soc_alarm_btn_12Buffer, CLU_HEIGH_SOC_ALARM_BTN_12_SIZE, &clu_heigh_soc_alarm_btn_12);
}

void Modbus_threshold_settingView::clu_heigh_soc_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_heigh_soc_protect_btn_12Buffer, CLU_HEIGH_SOC_PROTECT_BTN_12_SIZE, &clu_heigh_soc_protect_btn_12);
}

void Modbus_threshold_settingView::clu_low_soc_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_soc_warn_btn_13Buffer, CLU_LOW_SOC_WARN_BTN_13_SIZE, &clu_low_soc_warn_btn_13);
}

void Modbus_threshold_settingView::clu_low_soc_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_soc_alarm_btn_13Buffer, CLU_LOW_SOC_ALARM_BTN_13_SIZE, &clu_low_soc_alarm_btn_13);
}

void Modbus_threshold_settingView::clu_low_soc_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_low_soc_protect_btn_13Buffer, CLU_LOW_SOC_PROTECT_BTN_13_SIZE, &clu_low_soc_protect_btn_13);
}

void Modbus_threshold_settingView::clu_insul_res_p_low_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_insul_res_p_low_warn_btn_14Buffer, CLU_INSUL_RES_P_LOW_WARN_BTN_14_SIZE, &clu_insul_res_p_low_warn_btn_14);
}

void Modbus_threshold_settingView::clu_insul_res_p_low_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_insul_res_p_low_alarm_btn_14Buffer, CLU_INSUL_RES_P_LOW_ALARM_BTN_14_SIZE, &clu_insul_res_p_low_alarm_btn_14);
}

void Modbus_threshold_settingView::clu_insul_res_p_low_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_insul_res_p_low_protect_btn_14Buffer, CLU_INSUL_RES_P_LOW_PROTECT_BTN_14_SIZE, &clu_insul_res_p_low_protect_btn_14);
}

void Modbus_threshold_settingView::clu_insul_res_n_low_warn_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_insul_res_n_low_warn_btn_15Buffer, CLU_INSUL_RES_N_LOW_WARN_BTN_15_SIZE, &clu_insul_res_n_low_warn_btn_15);
}

void Modbus_threshold_settingView::clu_insul_res_n_low_alarm_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_insul_res_n_low_alarm_btn_15Buffer, CLU_INSUL_RES_N_LOW_ALARM_BTN_15_SIZE, &clu_insul_res_n_low_alarm_btn_15);
}

void Modbus_threshold_settingView::clu_insul_res_n_low_protect_cb(){
  keyboard_modbus1.initialize();
  keyboard_modbus1.setVisible(true);
  keyboard_modbus1.invalidate();

  keyboard_modbus1.SetBufPar(clu_insul_res_n_low_protect_btn_15Buffer, CLU_INSUL_RES_N_LOW_PROTECT_BTN_15_SIZE, &clu_insul_res_n_low_protect_btn_15);
}

void Modbus_threshold_settingView::modbus_threshold_save_cb(){
#ifndef SIMULATOR
    modbus_threshold_flash.clu_high_volt_warn = simple_atof(clu_heigh_volt_warn_btnBuffer);
    modbus_threshold_flash.clu_high_volt_alarm = simple_atof(clu_heigh_volt_alarm_btnBuffer);
    modbus_threshold_flash.clu_high_volt_protect = simple_atof(clu_heigh_volt_protect_btnBuffer);
    modbus_threshold_flash.clu_low_volt_warn = simple_atof(clu_low_volt_warn_btn_1Buffer);
    modbus_threshold_flash.clu_low_volt_alarm = simple_atof(clu_low_volt_alarm_btn_1Buffer);
    modbus_threshold_flash.clu_low_volt_protect = simple_atof(clu_low_volt_protect_btn_1Buffer);
    modbus_threshold_flash.clu_high_charge_cur_warn = simple_atof(clu_heigh_charge_cur_warn_btn_2Buffer);
    modbus_threshold_flash.clu_high_charge_cur_alarm = simple_atof(clu_heigh_charge_cur_alarm_btn_2Buffer);
    modbus_threshold_flash.clu_high_charge_cur_protect = simple_atof(clu_heigh_charge_cur_protect_btn_2Buffer);
    modbus_threshold_flash.clu_high_discharge_cur_warn = simple_atof(clu_heigh_discharge_cur_warn_btn_3Buffer);
    modbus_threshold_flash.clu_high_discharge_cur_alarm = simple_atof(clu_heigh_discharge_cur_alarm_btn_3Buffer);
    modbus_threshold_flash.clu_high_discharge_cur_protect = simple_atof(clu_heigh_discharge_cur_protect_btn_3Buffer);
    modbus_threshold_flash.cell_high_volt_warn = simple_atof(cell_heigh_volt_warn_btn_4Buffer);
    modbus_threshold_flash.cell_high_volt_alarm = simple_atof(cell_heigh_volt_alarm_btn_4Buffer);
    modbus_threshold_flash.cell_high_volt_protect = simple_atof(cell_heigh_volt_protect_btn_4Buffer);
    modbus_threshold_flash.cell_low_volt_warn = simple_atof(cell_low_volt_warn_btn_5Buffer);
    modbus_threshold_flash.cell_low_volt_alarm = simple_atof(cell_low_volt_alarm_btn_5Buffer);
    modbus_threshold_flash.cell_low_volt_protect = simple_atof(cell_low_volt_protect_btn_5Buffer);
    modbus_threshold_flash.clu_high_charge_temp_warn = simple_atof(clu_heigh_charge_temp_warn_btn_6Buffer);
    modbus_threshold_flash.clu_high_charge_temp_alarm = simple_atof(clu_heigh_charge_temp_alarm_btn_6Buffer);
    modbus_threshold_flash.clu_high_charge_temp_protect = simple_atof(clu_heigh_charge_temp_protect_btn_6Buffer);
    modbus_threshold_flash.clu_low_charge_temp_warn = simple_atof(clu_low_charge_temp_warn_btn_7Buffer);
    modbus_threshold_flash.clu_low_charge_temp_alarm = simple_atof(clu_low_charge_temp_alarm_btn_7Buffer);
    modbus_threshold_flash.clu_low_charge_temp_protect = simple_atof(clu_low_charge_temp_protect_btn_7Buffer);
    modbus_threshold_flash.clu_high_discharge_temp_warn = simple_atof(clu_heigh_discharge_temp_warn_btn_8Buffer);
    modbus_threshold_flash.clu_high_discharge_temp_alarm = simple_atof(clu_heigh_discharge_temp_alarm_btn_8Buffer);
    modbus_threshold_flash.clu_high_discharge_temp_protect = simple_atof(clu_heigh_discharge_temp_protect_btn_8Buffer);
    modbus_threshold_flash.clu_low_discharge_temp_warn = simple_atof(clu_low_discharge_temp_warn_btn_9Buffer);
    modbus_threshold_flash.clu_low_discharge_temp_alarm = simple_atof(clu_low_discharge_temp_alarm_btn_9Buffer);
    modbus_threshold_flash.clu_low_discharge_temp_protect = simple_atof(clu_low_discharge_temp_protect_btn_9Buffer);
    modbus_threshold_flash.cell_high_temp_warn = simple_atof(cell_heigh_temp_warn_btn_10Buffer);
    modbus_threshold_flash.cell_high_temp_alarm = simple_atof(cell_heigh_temp_alarm_btn_10Buffer);
    modbus_threshold_flash.cell_high_temp_protect = simple_atof(cell_heigh_temp_protect_btn_10Buffer);
    modbus_threshold_flash.cell_low_temp_warn = simple_atof(cell_low_temp_warn_btn_11Buffer);
    modbus_threshold_flash.cell_low_temp_alarm = simple_atof(cell_low_temp_alarm_btn_11Buffer);
    modbus_threshold_flash.cell_low_temp_protect = simple_atof(cell_low_temp_protect_btn_11Buffer);
    modbus_threshold_flash.clu_high_soc_warn = simple_atof(clu_heigh_soc_warn_btn_12Buffer);
    modbus_threshold_flash.clu_high_soc_alarm = simple_atof(clu_heigh_soc_alarm_btn_12Buffer);
    modbus_threshold_flash.clu_high_soc_protect = simple_atof(clu_heigh_soc_protect_btn_12Buffer);
    modbus_threshold_flash.clu_low_soc_warn = simple_atof(clu_low_soc_warn_btn_13Buffer);
    modbus_threshold_flash.clu_low_soc_alarm = simple_atof(clu_low_soc_alarm_btn_13Buffer);
    modbus_threshold_flash.clu_low_soc_protect = simple_atof(clu_low_soc_protect_btn_13Buffer);
    modbus_threshold_flash.clu_insul_res_p_low_warn = simple_atof(clu_insul_res_p_low_warn_btn_14Buffer);
    modbus_threshold_flash.clu_insul_res_p_low_alarm = simple_atof(clu_insul_res_p_low_alarm_btn_14Buffer);
    modbus_threshold_flash.clu_insul_res_p_low_protect = simple_atof(clu_insul_res_p_low_protect_btn_14Buffer);
    modbus_threshold_flash.clu_insul_res_n_low_warn = simple_atof(clu_insul_res_n_low_warn_btn_15Buffer);
    modbus_threshold_flash.clu_insul_res_n_low_alarm = simple_atof(clu_insul_res_n_low_alarm_btn_15Buffer);
    modbus_threshold_flash.clu_insul_res_n_low_protect = simple_atof(clu_insul_res_n_low_protect_btn_15Buffer);

    modbus_threshold_save(&modbus_threshold_flash);
#endif
}
