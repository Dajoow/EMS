/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2022-10-28 10:39:08
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-12 19:23:35
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\touchgfx\gui\src\model\model.cpp
 * @Description:
 *
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved.
 */
#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include <touchgfx/Utils.hpp>

#include <gui/common/GuiTransTypes.hpp>

#ifndef SIMULATOR
extern "C" {
#include "CAN_Control.h"
#include "station_ctl.h"
#include "string.h"
#include "cmsis_os.h"
#include "task.h"
#include "lcd.h"
#include "modbus.h"
extern BCMU_Mail_t BCMU[cluster_num];
extern osSemaphoreId ViewUpdateSemHandle;
};
#endif

ViewToModelData viewToModelDataTemp;

Model::Model () : modelListener (0), deviceStatusTick (0) {}

void
Model::tick ()
{
#ifndef SIMULATOR
    if (++deviceStatusTick >= 30U) {
        acdc_modbus_data_t acdcData;
        dcdc_modbus_data_t dcdcData;
        dcdc_modbus_write_status_t writeStatus;
        DeviceStatusData ui;
        uint32_t now = HAL_GetTick();
        uint8_t index;

        deviceStatusTick = 0U;
        modbus_copy_gui_snapshot(&acdcData, &dcdcData, &writeStatus);

        ui.acdcOnline = acdcData.online;
        ui.acdcAgeMs = (acdcData.last_update_tick == 0U) ? 0U : (now - acdcData.last_update_tick);
        ui.acdcDcVoltage = acdcData.dc_bus_voltage_v;
        ui.acdcDcCurrent = acdcData.dc_load_current_a;
        ui.acdcAcVoltage = acdcData.ac_voltage_v;
        ui.acdcAcCurrent = acdcData.ac_current_a;
        ui.acdcFrequency = acdcData.ac_frequency_hz;
        ui.acdcRectifierCount = acdcData.rectifier_count;
        ui.acdcRectifierPower = acdcData.rectifier_total_power_w;
        for (index = 0U; index < 12U; index++) {
            if (acdcData.ac_alarm[index] != 0U) {
                ui.acdcAlarm = 1U;
                break;
            }
        }

        ui.dcdcAgeMs = (dcdcData.last_update_tick == 0U) ? 0U : (now - dcdcData.last_update_tick);
        if ((dcdcData.online == 0U) || (dcdcData.last_update_tick == 0U)) {
            ui.dcdcState = DEVICE_UI_OFFLINE;
        } else if (ui.dcdcAgeMs > DCDC_MODBUS_DATA_FRESHNESS_MS) {
            ui.dcdcState = DEVICE_UI_STALE;
        } else if (dcdcData.fault_raw != 0U) {
            ui.dcdcState = DEVICE_UI_FAULT;
        } else if (dcdcData.work_state == DCDC_WORK_STATE_CV_CURRENT_LIMIT) {
            ui.dcdcState = DEVICE_UI_RUNNING;
        } else if (dcdcData.work_state == DCDC_WORK_STATE_STOP_OR_FAULT) {
            ui.dcdcState = DEVICE_UI_STOPPED;
        } else {
            ui.dcdcState = DEVICE_UI_UNKNOWN;
        }
        ui.dcdcFaultRaw = dcdcData.fault_raw;
        ui.dcdcBVoltage = dcdcData.b_voltage_v;
        ui.dcdcBCurrent = dcdcData.b_current_a;
        ui.dcdcBPower = dcdcData.b_power_w;
        ui.dcdcPVoltage = dcdcData.p_voltage_v;
        ui.dcdcPCurrent = dcdcData.p_current_a;
        ui.dcdcPPower = dcdcData.p_power_w;
        ui.dcdcMaxTemperature = dcdcData.max_temperature_c;
        ui.writeState = writeStatus.state;
        ui.writeAttempts = writeStatus.attempt_count;
        ui.readbackConfirmed = writeStatus.readback_confirmed;
        ui.writeError = writeStatus.last_error;
        ui.writeSequence = writeStatus.sequence;
        modelListener->NotifyDeviceStatus(ui);
    }

    modelToViewData.frameRateCount++; // 再CPUtask线程中1s清零
    // 获取信号量但不阻塞
    // 更新数据原因1.CAN采集完成传过来信号量 或者 2. VIEW通知要更新
    if (ViewUpdateSemHandle != NULL
        && (xSemaphoreTake (ViewUpdateSemHandle, 0) == pdTRUE || viewToModelDataTemp.reflashFlag == true)) {
        if (viewToModelDataTemp.reflashFlag == true) viewToModelDataTemp.reflashFlag = false;
        // 电站数据VIEW更新
        modelToViewData.station_VOL     = Client_Sd_Station.station_VOL;
        modelToViewData.station_CUR     = Client_Sd_Station.station_CUR;
        modelToViewData.station_SOC     = Client_Sd_Station.station_SOC;
        modelToViewData.station_SOH     = Client_Sd_Station.station_SOH;
        modelToViewData.charge_power    = Client_Sd_Station.charge_power;
        modelToViewData.discharge_power = Client_Sd_Station.discharge_power;
        modelToViewData.station_state   = Client_Sd_Station.station_state;
        // 20簇状态数据VIEW更新
        for (uint8_t i = 0; i < cluster_num; i++) {
            modelToViewData.BCMU_state[i] = BCMU[i].OnlineOrOffline; // 离线状态到时候要加这里
        }
        // 某簇数据VIEW更新
        uint8_t bcmu_sel = viewToModelDataTemp.BCMU_SEL;
        uint8_t bmu_sel  = viewToModelDataTemp.BMU_SEL;
        if (bcmu_sel < 1 || bcmu_sel > cluster_num) bcmu_sel = 1;
        if (bmu_sel < 1 || bmu_sel > GRP_num) bmu_sel = 1;
        modelToViewData.cluster_VOL      = Client_Sd[bcmu_sel - 1].cluster_VOL;
        modelToViewData.cluster_CUR      = Client_Sd[bcmu_sel - 1].cluster_CUR;
        modelToViewData.cluster_SOC      = Client_Sd[bcmu_sel - 1].cluster_SOC;
        modelToViewData.cluster_SOH      = Client_Sd[bcmu_sel - 1].cluster_SOH;
        modelToViewData.cluster_res      = Client_Sd[bcmu_sel - 1].cluster_res;
        modelToViewData.insulation_res_p = Client_Sd[bcmu_sel - 1].insulation_res_p;
        modelToViewData.insulation_res_n = Client_Sd[bcmu_sel - 1].insulation_res_n;
        // 某组数据VIEW更新
        for (uint8_t i = 0; i < GRP_BAT_num; i++) {
            modelToViewData.BAT_VOL[i] = Client_Sd[bcmu_sel - 1].BAT_VOL[(bmu_sel - 1) * GRP_BAT_num + i];
            modelToViewData.BAT_TMP[i] = Client_Sd[bcmu_sel - 1].BAT_TMP[(bmu_sel - 1) * GRP_BAT_num + i];
            // todo
            // modelToViewData.BAT_FAULT[i] =
            // Client_Sd[viewToModelDataTemp.BCMU_SEL-1].BAT_FAULT[(viewToModelDataTemp.BMU_SEL-1)*GRP_BAT_num + i];
            modelToViewData.BAT_SOC[i] = Client_Sd[bcmu_sel - 1].BAT_SOC[(bmu_sel - 1) * GRP_BAT_num + i];
        }
        // 将硬件数据传递给VIEW
        modelListener->NotifyViewMsg (modelToViewData);
    }

#else
    if (++deviceStatusTick >= 30U) {
        DeviceStatusData demo;
        deviceStatusTick = 0U;
        demo.acdcOnline = 0U;
        demo.acdcAgeMs = 0U;
        demo.dcdcState = DEVICE_UI_UNKNOWN;
        demo.dcdcAgeMs = 0U;
        modelListener->NotifyDeviceStatus(demo);
    }
#endif
}

int Model::requestDcdcSafeStop()
{
#ifndef SIMULATOR
    return dcdc_modbus_set_run_async(0U);
#else
    return -3;
#endif
}
// 接受来自VIEW的数据并处理
void
Model::ViewtoModelDat (const ViewToModelData &Data)
{
// 拷贝来自VIEW的参数
#ifndef SIMULATOR
    memcpy (&viewToModelDataTemp, &Data, sizeof (ViewToModelData));
#endif
    touchgfx_printf ("bcmu: %d\n", Data.BCMU_SEL);
    touchgfx_printf ("bmu:  %d\n", Data.BMU_SEL);
    touchgfx_printf ("\n");
}

// void Model::err_inf(const err_info& err)
//{
//
// }
