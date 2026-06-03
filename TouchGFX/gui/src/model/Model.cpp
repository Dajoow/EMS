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
extern BCMU_Mail_t BCMU[cluster_num];
extern osSemaphoreId ViewUpdateSemHandle;
};
#endif

ViewToModelData viewToModelDataTemp;

Model::Model () : modelListener (0) {}

void
Model::tick ()
{
#ifndef SIMULATOR
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
