/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2022-12-01 19:44:14
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-12 19:11:58
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\touchgfx\gui\src\mainscreen_screen\mainscreenpresenter.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2022-12-01 19:44:14
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-12 16:32:37
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\touchgfx\gui\src\mainscreen_screen\mainscreenpresenter.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>

MainScreenPresenter::MainScreenPresenter(MainScreenView& v)
    : view(v)
{

}

void MainScreenPresenter::activate()
{

}

void MainScreenPresenter::deactivate()
{

}

//TouchGFX仿真时，硬件不向TouchGFX传数据
#ifndef SIMULATOR
void MainScreenPresenter::NotifyViewMsg(ModelToViewData modelToViewData)
{
    view.NotifyViewMsg(modelToViewData);
}
#endif

void MainScreenPresenter::ViewtoModelDat(const ViewToModelData& Data){
    model->ViewtoModelDat(Data);
}

