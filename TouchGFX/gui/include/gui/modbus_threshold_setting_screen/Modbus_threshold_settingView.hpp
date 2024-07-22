#ifndef MODBUS_THRESHOLD_SETTINGVIEW_HPP
#define MODBUS_THRESHOLD_SETTINGVIEW_HPP

#include <gui_generated/modbus_threshold_setting_screen/Modbus_threshold_settingViewBase.hpp>
#include <gui/modbus_threshold_setting_screen/Modbus_threshold_settingPresenter.hpp>

class Modbus_threshold_settingView : public Modbus_threshold_settingViewBase
{
public:
    Modbus_threshold_settingView();
    virtual ~Modbus_threshold_settingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void clu_heigh_vol_warn_cb();
    virtual void clu_heigh_vol_alarm_cb();
    virtual void clu_heigh_vol_protect_cb();
    virtual void clu_low_vol_warn_cb();
    virtual void clu_low_vol_alarm_cb();
    virtual void clu_low_vol_protect_cb();
    virtual void clu_heigh_charge_cur_warn_cb();
    virtual void clu_heigh_charge_cur_alarm_cb();
    virtual void clu_heigh_charge_cur_protect_cb();
    virtual void clu_heigh_discharge_cur_warn_cb();
    virtual void clu_heigh_discharge_cur_alarm_cb();
    virtual void clu_heigh_discharge_cur_protect_cb();
    virtual void cell_heigh_volt_warn_cb();
    virtual void cell_heigh_volt_alarm_cb();
    virtual void cell_heigh_volt_protect_cb();
    virtual void cell_low_volt_warn_cb();
    virtual void cell_low_volt_alarm_cb();
    virtual void cell_low_volt_protect_cb();
    virtual void clu_heigh_charge_temp_warn_cb();
    virtual void clu_heigh_charge_temp_alarm_cb();
    virtual void clu_heigh_charge_temp_protect_cb();
    virtual void clu_low_charge_temp_warn_cb();
    virtual void clu_low_charge_temp_alarm_cb();
    virtual void clu_low_charge_temp_protect_cb();
    virtual void clu_heigh_discharge_temp_warn_cb();
    virtual void clu_heigh_discharge_temp_alarm_cb();
    virtual void clu_heigh_discharge_temp_protect_cb();
    virtual void clu_low_discharge_temp_warn_cb();
    virtual void clu_low_discharge_temp_alarm_cb();
    virtual void clu_low_discharge_temp_protect_cb();
    virtual void cell_heigh_temp_warn_cb();
    virtual void cell_heigh_temp_alarm_cb();
    virtual void cell_heigh_temp_protect_cb();
    virtual void cell_low_temp_warn_cb();
    virtual void cell_low_temp_alarm_cb();
    virtual void cell_low_temp_protect_cb();
    virtual void clu_heigh_soc_warn_cb();
    virtual void clu_heigh_soc_alarm_cb();
    virtual void clu_heigh_soc_protect_cb();
    virtual void clu_low_soc_warn_cb();
    virtual void clu_low_soc_alarm_cb();
    virtual void clu_low_soc_protect_cb();
    virtual void clu_insul_res_p_low_warn_cb();
    virtual void clu_insul_res_p_low_alarm_cb();
    virtual void clu_insul_res_p_low_protect_cb();
    virtual void clu_insul_res_n_low_warn_cb();
    virtual void clu_insul_res_n_low_alarm_cb();
    virtual void clu_insul_res_n_low_protect_cb();

    virtual void modbus_threshold_save_cb();

  protected:
};

#endif // MODBUS_THRESHOLD_SETTINGVIEW_HPP
