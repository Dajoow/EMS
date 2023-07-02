/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2023-03-20 15:02:49
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-06-09 20:49:38
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\touchgfx\gui\src\setting_screen\settingview.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include <gui/setting_screen/SettingView.hpp>

#ifndef SIMULATOR
extern "C"{
#include "at24cxx.h"
#include "string.h"
#include "stm32h7xx_hal.h"
#include "core_cm7.h"	

void SoftReset(void)
{
    __set_FAULTMASK(1); //关闭所有中断
}
};
#endif

SettingView::SettingView():CAN_kbpsAnimateToCallback(this, &SettingView::CAN_kbpsAnimateToHandler), RS485_kbpsAnimateToCallback(this, &SettingView::RS485_kbpsAnimateToHandler)
{
	//滚轮的添加回调函数
	CAN_kbps.setAnimateToCallback(CAN_kbpsAnimateToCallback);
	RS485_kbps.setAnimateToCallback(RS485_kbpsAnimateToCallback);
}

void SettingView::CAN_kbpsAnimateToHandler(int16_t itemSelected)
{
	#ifndef SIMULATOR
	if(itemSelected == 0)
        SettingBuff.canBps = 0;
    else if (itemSelected == 1)
        SettingBuff.canBps = 1;
    else if (itemSelected == 2)
        SettingBuff.canBps = 2;
    else if (itemSelected == 3)
        SettingBuff.canBps = 3;
    else if (itemSelected == 4)
        SettingBuff.canBps = 4;
	#endif
}

void SettingView::RS485_kbpsAnimateToHandler(int16_t itemSelected)
{
	#ifndef SIMULATOR
	if(itemSelected == 0)
        SettingBuff.RS485Bps = 0;
    else if (itemSelected == 1)
        SettingBuff.RS485Bps = 1;
    else if (itemSelected == 2)
        SettingBuff.RS485Bps = 2;
    else if (itemSelected == 3)
        SettingBuff.RS485Bps = 3;
    else if (itemSelected == 4)
        SettingBuff.RS485Bps = 4;
	else if (itemSelected == 5)
        SettingBuff.RS485Bps = 5;
	#endif
}


void SettingView::setupScreen()
{
    SettingViewBase::setupScreen();
	    //IP_setting这个界面的有关按键
    IP_setting_butAry[0] = &IP_ADD_1;
    IP_setting_butAry[1] = &IP_ADD_2;
    IP_setting_butAry[2] = &IP_ADD_3;
    IP_setting_butAry[3] = &IP_ADD_4;

    IP_setting_butAry[4] = &GATEWAY_1;
    IP_setting_butAry[5] = &GATEWAY_2;
    IP_setting_butAry[6] = &GATEWAY_3;
    IP_setting_butAry[7] = &GATEWAY_4;

    IP_setting_butAry[8] = &NETMASK_1;
    IP_setting_butAry[9] = &NETMASK_2;
    IP_setting_butAry[10] = &NETMASK_3;
    IP_setting_butAry[11] = &NETMASK_4;

    //IP_setting_1这个界面的有关按键
    IP_setting_1_butAry[0] = &IP_ADD_1_1;
    IP_setting_1_butAry[1] = &IP_ADD_1_2;
    IP_setting_1_butAry[2] = &IP_ADD_1_3;
    IP_setting_1_butAry[3] = &IP_ADD_1_4;
    IP_setting_1_butAry[4] = &PORT;

    //IP_setting_2这个界面的有关按键
    IP_setting_2_butAry[0] = &IP_ADD_2_1;
    IP_setting_2_butAry[1] = &IP_ADD_2_2;
    IP_setting_2_butAry[2] = &IP_ADD_2_3;
    IP_setting_2_butAry[3] = &IP_ADD_2_4;
    IP_setting_2_butAry[4] = &PORT_1;

    //还要SettingBuff和EPProm中一致（初值）
    SettingBuff.IP_ADD[0] = 0;
    SettingBuff.IP_ADD[1] = 0;
    SettingBuff.IP_ADD[2] = 0;
    SettingBuff.IP_ADD[3] = 0;

    SettingBuff.GATEWAY[0] = 0;
    SettingBuff.GATEWAY[1] = 0;
    SettingBuff.GATEWAY[2] = 0;
    SettingBuff.GATEWAY[3] = 0;

    SettingBuff.NETMASK[0] = 0;
    SettingBuff.NETMASK[1] = 0;
    SettingBuff.NETMASK[2] = 0;
    SettingBuff.NETMASK[3] = 0;

    SettingBuff.IP_ADD_1[0] = 0;
    SettingBuff.IP_ADD_1[1] = 0;
    SettingBuff.IP_ADD_1[2] = 0;
    SettingBuff.IP_ADD_1[3] = 0;
    SettingBuff.port = 0;

    SettingBuff.IP_ADD_2[0] = 0;
    SettingBuff.IP_ADD_2[1] = 0;
    SettingBuff.IP_ADD_2[2] = 0;
    SettingBuff.IP_ADD_2[3] = 0;
    SettingBuff.port_1 = 0;

    SettingBuff.cu_num = 20;
    SettingBuff.poll_T = 5;

//不在模拟器中,在实际中要将EEPRON中的数据拷贝过来
#ifndef SIMULATOR
    memcpy(&SettingBuff, &bsmuSetting, sizeof(EEPROM_BSMU_tem));
#endif
    //装机的簇数量和轮询周期参数显示初始化
    poll_T.setValue(SettingBuff.poll_T);
    Unicode::snprintf(poll_tBuffer, POLL_T_SIZE, "%d", SettingBuff.poll_T*10);
    poll_t.invalidate();
    
    cu_NUM.setValue(SettingBuff.cu_num);
    Unicode::snprintf(cu_numBuffer, CU_NUM_SIZE, "%d",  SettingBuff.cu_num );
    cu_num.invalidate();

    //初始化本机IP相关参数显示
    Unicode::snprintf(Buffer[0], 10, "%03d", SettingBuff.IP_ADD[0]);
    IP_ADD_1.setWildcardTextBuffer(Buffer[0]);
    IP_ADD_1.invalidate();
    Unicode::snprintf(Buffer[1], 10, "%03d", SettingBuff.IP_ADD[1]);
    IP_ADD_2.setWildcardTextBuffer(Buffer[1]);
    IP_ADD_2.invalidate();
    Unicode::snprintf(Buffer[2], 10, "%03d", SettingBuff.IP_ADD[2]);
    IP_ADD_3.setWildcardTextBuffer(Buffer[2]);
    IP_ADD_3.invalidate();
    Unicode::snprintf(Buffer[3], 10, "%03d", SettingBuff.IP_ADD[3]);
    IP_ADD_4.setWildcardTextBuffer(Buffer[3]);
    IP_ADD_4.invalidate();

    Unicode::snprintf(Buffer[4], 10, "%03d", SettingBuff.GATEWAY[0]);
    GATEWAY_1.setWildcardTextBuffer(Buffer[4]);
    GATEWAY_1.invalidate();
    Unicode::snprintf(Buffer[5], 10, "%03d", SettingBuff.GATEWAY[1]);
    GATEWAY_2.setWildcardTextBuffer(Buffer[5]);
    GATEWAY_2.invalidate();
    Unicode::snprintf(Buffer[6], 10, "%03d", SettingBuff.GATEWAY[2]);
    GATEWAY_3.setWildcardTextBuffer(Buffer[6]);
    GATEWAY_3.invalidate();
    Unicode::snprintf(Buffer[7], 10, "%03d", SettingBuff.GATEWAY[3]);
    GATEWAY_4.setWildcardTextBuffer(Buffer[7]);
    GATEWAY_4.invalidate();

    Unicode::snprintf(Buffer[8], 10, "%03d", SettingBuff.NETMASK[0]);
    NETMASK_1.setWildcardTextBuffer(Buffer[8]);
    NETMASK_1.invalidate();
    Unicode::snprintf(Buffer[9], 10, "%03d", SettingBuff.NETMASK[1]);
    NETMASK_2.setWildcardTextBuffer(Buffer[9]);
    NETMASK_2.invalidate();
    Unicode::snprintf(Buffer[10], 10, "%03d", SettingBuff.NETMASK[2]);
    NETMASK_3.setWildcardTextBuffer(Buffer[10]);
    NETMASK_3.invalidate();
    Unicode::snprintf(Buffer[11], 10, "%03d", SettingBuff.NETMASK[3]);
    NETMASK_4.setWildcardTextBuffer(Buffer[11]);
    NETMASK_4.invalidate();

    //初始化本地服务器IP相关参数显示
    Unicode::snprintf(Buffer1[0], 10, "%03d", SettingBuff.IP_ADD_1[0]);
    IP_ADD_1_1.setWildcardTextBuffer(Buffer1[0]);
    IP_ADD_1_1.invalidate();
    Unicode::snprintf(Buffer1[1], 10, "%03d", SettingBuff.IP_ADD_1[1]);
    IP_ADD_1_2.setWildcardTextBuffer(Buffer1[1]);
    IP_ADD_1_2.invalidate();
    Unicode::snprintf(Buffer1[2], 10, "%03d", SettingBuff.IP_ADD_1[2]);
    IP_ADD_1_3.setWildcardTextBuffer(Buffer1[2]);
    IP_ADD_1_3.invalidate();
    Unicode::snprintf(Buffer1[3], 10, "%03d", SettingBuff.IP_ADD_1[3]);
    IP_ADD_1_4.setWildcardTextBuffer(Buffer1[3]);
    IP_ADD_1_4.invalidate();
    Unicode::snprintf(Buffer1[4], 10, "%d", SettingBuff.port);
    PORT.setWildcardTextBuffer(Buffer1[4]);
    PORT.invalidate();

    //初始化4G模块IP相关参数显示
    Unicode::snprintf(Buffer2[0], 10, "%03d", SettingBuff.IP_ADD_2[0]);
    IP_ADD_2_1.setWildcardTextBuffer(Buffer2[0]);
    IP_ADD_2_1.invalidate();
    Unicode::snprintf(Buffer2[1], 10, "%03d", SettingBuff.IP_ADD_2[1]);
    IP_ADD_2_2.setWildcardTextBuffer(Buffer2[1]);
    IP_ADD_2_2.invalidate();
    Unicode::snprintf(Buffer2[2], 10, "%03d", SettingBuff.IP_ADD_2[2]);
    IP_ADD_2_3.setWildcardTextBuffer(Buffer2[2]);
    IP_ADD_2_3.invalidate();
    Unicode::snprintf(Buffer2[3], 10, "%03d", SettingBuff.IP_ADD_2[3]);
    IP_ADD_2_4.setWildcardTextBuffer(Buffer2[3]);
    IP_ADD_2_4.invalidate();
    Unicode::snprintf(Buffer2[4], 10, "%d", SettingBuff.port_1);
    PORT_1.setWildcardTextBuffer(Buffer2[4]);
    PORT_1.invalidate();

    //配置界面中显示内容初始化
    Unicode::snprintf(ip_text1Buffer1, IP_TEXT1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD[0]);
    Unicode::snprintf(ip_text1Buffer2, IP_TEXT1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD[1]);
    ip_text1.invalidate();
    Unicode::snprintf(ip_text2Buffer1, IP_TEXT2BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD[2]);
    Unicode::snprintf(ip_text2Buffer2, IP_TEXT2BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD[3]);
    ip_text2.invalidate();

    Unicode::snprintf(gateway_text1Buffer1, GATEWAY_TEXT1BUFFER1_SIZE, "%03d", SettingBuff.GATEWAY[0]);
    Unicode::snprintf(gateway_text1Buffer2, GATEWAY_TEXT1BUFFER2_SIZE, "%03d", SettingBuff.GATEWAY[1]);
    gateway_text1.invalidate();
    Unicode::snprintf(gateway_text2Buffer1, GATEWAY_TEXT2BUFFER1_SIZE, "%03d", SettingBuff.GATEWAY[2]);
    Unicode::snprintf(gateway_text2Buffer2, GATEWAY_TEXT2BUFFER2_SIZE, "%03d", SettingBuff.GATEWAY[3]);
    gateway_text2.invalidate();

    Unicode::snprintf(networkText1Buffer1, NETWORKTEXT1BUFFER1_SIZE, "%03d", SettingBuff.NETMASK[0]);
    Unicode::snprintf(networkText1Buffer2, NETWORKTEXT1BUFFER2_SIZE, "%03d", SettingBuff.NETMASK[1]);
    networkText1.invalidate();
    Unicode::snprintf(networkText2Buffer1, NETWORKTEXT2BUFFER1_SIZE, "%03d", SettingBuff.NETMASK[2]);
    Unicode::snprintf(networkText2Buffer2, NETWORKTEXT2BUFFER2_SIZE, "%03d", SettingBuff.NETMASK[3]);
    networkText2.invalidate();

    Unicode::snprintf(ip_text1_1Buffer1, IP_TEXT1_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_1[0]);
    Unicode::snprintf(ip_text1_1Buffer2, IP_TEXT1_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_1[1]);
    ip_text1_1.invalidate();
    Unicode::snprintf(ip_text2_1Buffer1, IP_TEXT2_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_1[2]);
    Unicode::snprintf(ip_text2_1Buffer2, IP_TEXT2_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_1[3]);
    ip_text2_1.invalidate();
    Unicode::snprintf(port_textBuffer, PORT_TEXT_SIZE, "%d", SettingBuff.port);
    port_text.invalidate();

    Unicode::snprintf(ip_1_text1_1Buffer1, IP_1_TEXT1_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_2[0]);
    Unicode::snprintf(ip_1_text1_1Buffer2, IP_1_TEXT1_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_2[1]);
    ip_1_text1_1.invalidate();
    Unicode::snprintf(ip_1_text2_1Buffer1, IP_1_TEXT2_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_2[2]);
    Unicode::snprintf(ip_1_text2_1Buffer2, IP_1_TEXT2_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_2[3]);
    ip_1_text1_1.invalidate();
    Unicode::snprintf(port_1_textBuffer, PORT_1_TEXT_SIZE, "%d", SettingBuff.port_1);
    port_1_text.invalidate();
	
	//滚轮状态
	CAN_kbps.animateToItem(SettingBuff.canBps);
	RS485_kbps.animateToItem(SettingBuff.RS485Bps);
}

void SettingView::tearDownScreen()
{
    SettingViewBase::tearDownScreen();
}

//mSettingView每刷新10次
void SettingView::SettingViewTick()
{
    //本机配置界面中
    if(IP_setting.isVisible() == true){
        //如果虚拟键盘曾经按下ok
        if(keyboard1.refreshFlag){
            keyboard1.refreshFlag = 0;
            Unicode::snprintf(Buffer[0], 10, "%03d", SettingBuff.IP_ADD[0]);
            IP_ADD_1.setWildcardTextBuffer(Buffer[0]);
            IP_ADD_1.invalidate();
            Unicode::snprintf(Buffer[1], 10, "%03d", SettingBuff.IP_ADD[1]);
            IP_ADD_2.setWildcardTextBuffer(Buffer[1]);
            IP_ADD_2.invalidate();
            Unicode::snprintf(Buffer[2], 10, "%03d", SettingBuff.IP_ADD[2]);
            IP_ADD_3.setWildcardTextBuffer(Buffer[2]);
            IP_ADD_3.invalidate();
            Unicode::snprintf(Buffer[3], 10, "%03d", SettingBuff.IP_ADD[3]);
            IP_ADD_4.setWildcardTextBuffer(Buffer[3]);
            IP_ADD_4.invalidate();

            Unicode::snprintf(Buffer[4], 10, "%03d", SettingBuff.GATEWAY[0]);
            GATEWAY_1.setWildcardTextBuffer(Buffer[4]);
            GATEWAY_1.invalidate();
            Unicode::snprintf(Buffer[5], 10, "%03d", SettingBuff.GATEWAY[1]);
            GATEWAY_2.setWildcardTextBuffer(Buffer[5]);
            GATEWAY_2.invalidate();
            Unicode::snprintf(Buffer[6], 10, "%03d", SettingBuff.GATEWAY[2]);
            GATEWAY_3.setWildcardTextBuffer(Buffer[6]);
            GATEWAY_3.invalidate();
            Unicode::snprintf(Buffer[7], 10, "%03d", SettingBuff.GATEWAY[3]);
            GATEWAY_4.setWildcardTextBuffer(Buffer[7]);
            GATEWAY_4.invalidate();

            Unicode::snprintf(Buffer[8], 10, "%03d", SettingBuff.NETMASK[0]);
            NETMASK_1.setWildcardTextBuffer(Buffer[8]);
            NETMASK_1.invalidate();
            Unicode::snprintf(Buffer[9], 10, "%03d", SettingBuff.NETMASK[1]);
            NETMASK_2.setWildcardTextBuffer(Buffer[9]);
            NETMASK_2.invalidate();
            Unicode::snprintf(Buffer[10], 10, "%03d", SettingBuff.NETMASK[2]);
            NETMASK_3.setWildcardTextBuffer(Buffer[10]);
            NETMASK_3.invalidate();
            Unicode::snprintf(Buffer[11], 10, "%03d", SettingBuff.NETMASK[3]);
            NETMASK_4.setWildcardTextBuffer(Buffer[11]);
            NETMASK_4.invalidate();
        }
    }
    //本地服务器配置界面中
    if(IP_setting_1.isVisible() == true){
        //如果虚拟键盘曾经按下ok
        if(keyboard1.refreshFlag){
            keyboard1.refreshFlag = 0;
            Unicode::snprintf(Buffer1[0], 10, "%03d", SettingBuff.IP_ADD_1[0]);
            IP_ADD_1_1.setWildcardTextBuffer(Buffer1[0]);
            IP_ADD_1_1.invalidate();
            Unicode::snprintf(Buffer1[1], 10, "%03d", SettingBuff.IP_ADD_1[1]);
            IP_ADD_1_2.setWildcardTextBuffer(Buffer1[1]);
            IP_ADD_1_2.invalidate();
            Unicode::snprintf(Buffer1[2], 10, "%03d", SettingBuff.IP_ADD_1[2]);
            IP_ADD_1_3.setWildcardTextBuffer(Buffer1[2]);
            IP_ADD_1_3.invalidate();
            Unicode::snprintf(Buffer1[3], 10, "%03d", SettingBuff.IP_ADD_1[3]);
            IP_ADD_1_4.setWildcardTextBuffer(Buffer1[3]);
            IP_ADD_1_4.invalidate();
            Unicode::snprintf(Buffer1[4], 10, "%d", SettingBuff.port);
            PORT.setWildcardTextBuffer(Buffer1[4]);
            PORT.invalidate();
        }
    }
    //4G模块配置界面中
    if(IP_setting_2.isVisible() == true){
        //如果虚拟键盘曾经按下ok
        if(keyboard1.refreshFlag){
            keyboard1.refreshFlag = 0;
            Unicode::snprintf(Buffer2[0], 10, "%03d", SettingBuff.IP_ADD_2[0]);
            IP_ADD_2_1.setWildcardTextBuffer(Buffer2[0]);
            IP_ADD_2_1.invalidate();
            Unicode::snprintf(Buffer2[1], 10, "%03d", SettingBuff.IP_ADD_2[1]);
            IP_ADD_2_2.setWildcardTextBuffer(Buffer2[1]);
            IP_ADD_2_2.invalidate();
            Unicode::snprintf(Buffer2[2], 10, "%03d", SettingBuff.IP_ADD_2[2]);
            IP_ADD_2_3.setWildcardTextBuffer(Buffer2[2]);
            IP_ADD_2_3.invalidate();
            Unicode::snprintf(Buffer2[3], 10, "%03d", SettingBuff.IP_ADD_2[3]);
            IP_ADD_2_4.setWildcardTextBuffer(Buffer2[3]);
            IP_ADD_2_4.invalidate();
            Unicode::snprintf(Buffer2[4], 10, "%d", SettingBuff.port_1);
            PORT_1.setWildcardTextBuffer(Buffer2[4]);
            PORT_1.invalidate();
        }
    }
}

//SettingView()还需要更新下初始值
//更新簇数量
void SettingView::SetCuNum(int value)
{
#ifndef SIMULATOR
    SettingBuff.cu_num = value;
    bsmuSetting.cu_num = value;
#endif
    Unicode::snprintf(cu_numBuffer, CU_NUM_SIZE, "%d", value);
    cu_num.invalidate();
}

//更新轮询周期
void SettingView::SetPoll_T(int value)
{
#ifndef SIMULATOR
    SettingBuff.poll_T = value;
    bsmuSetting.poll_T = value;
#endif
    Unicode::snprintf(poll_tBuffer, POLL_T_SIZE, "%d", value*10);
    poll_t.invalidate();
}

//IP_setting
//IP_setting按键按下，调出两个窗口
void SettingView::IP_setting_fun()
{
    IP_setting.setVisible(true);
    IP_setting.invalidate();
    keyboard1.initialize();
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

//CloseSetting
//CloseSetting按键按下，调出两个窗口
void SettingView::CloseSetting_fun()
{
    IP_setting.setVisible(false);
    IP_setting.invalidate();
    keyboard1.setVisible(false);
    keyboard1.invalidate();
    SetButState(IP_setting_butAry, 5, false);
}

//IP_setting保存按钮
void SettingView::SaveFun(void)
{
    /*往AT24C02写入*/
#ifndef SIMULATOR
    /*在下面更新过
    eeprom_bsmu_WR.cu_num
    eeprom_bsmu_WR.poll_T
    */
    // eeprom_bsmu_WR.IP_ADD[0] = IP_ADD_1.getSelectedItem();
    memcpy(eerom_data.WriteBuff, &SettingBuff, sizeof(SettingBuff));
    AT24Cxx_SeqWrite(0x00, CAPACITY_SIZE, eerom_data.WriteBuff);
#endif
    //本机IP更新显示
    Unicode::snprintf(ip_text1Buffer1, IP_TEXT1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD[0]);
    Unicode::snprintf(ip_text1Buffer2, IP_TEXT1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD[1]);
    ip_text1.invalidate();
    Unicode::snprintf(ip_text2Buffer1, IP_TEXT2BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD[2]);
    Unicode::snprintf(ip_text2Buffer2, IP_TEXT2BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD[3]);
    ip_text2.invalidate();

    Unicode::snprintf(gateway_text1Buffer1, GATEWAY_TEXT1BUFFER1_SIZE, "%03d", SettingBuff.GATEWAY[0]);
    Unicode::snprintf(gateway_text1Buffer2, GATEWAY_TEXT1BUFFER2_SIZE, "%03d", SettingBuff.GATEWAY[1]);
    gateway_text1.invalidate();
    Unicode::snprintf(gateway_text2Buffer1, GATEWAY_TEXT2BUFFER1_SIZE, "%03d", SettingBuff.GATEWAY[2]);
    Unicode::snprintf(gateway_text2Buffer2, GATEWAY_TEXT2BUFFER2_SIZE, "%03d", SettingBuff.GATEWAY[3]);
    gateway_text2.invalidate();

    Unicode::snprintf(networkText1Buffer1, NETWORKTEXT1BUFFER1_SIZE, "%03d", SettingBuff.NETMASK[0]);
    Unicode::snprintf(networkText1Buffer2, NETWORKTEXT1BUFFER2_SIZE, "%03d", SettingBuff.NETMASK[1]);
    networkText1.invalidate();
    Unicode::snprintf(networkText2Buffer1, NETWORKTEXT2BUFFER1_SIZE, "%03d", SettingBuff.NETMASK[2]);
    Unicode::snprintf(networkText2Buffer2, NETWORKTEXT2BUFFER2_SIZE, "%03d", SettingBuff.NETMASK[3]);
    networkText2.invalidate();
}


void SettingView::save_all_fun(void)
{
    /*往AT24C02写入*/
#ifndef SIMULATOR
    /*在下面更新过
    eeprom_bsmu_WR.cu_num
    eeprom_bsmu_WR.poll_T
    */
	AT24Cxx_SeqRead(0x00, CAPACITY_SIZE, eerom_data.ReadBuff);
	
    memcpy(eerom_data.WriteBuff, &SettingBuff, sizeof(SettingBuff));
	while(memcmp(eerom_data.WriteBuff, eerom_data.ReadBuff, CAPACITY_SIZE) != 0){
		AT24Cxx_SeqWrite(0x00, CAPACITY_SIZE, eerom_data.WriteBuff);
		AT24Cxx_SeqRead(0x00, CAPACITY_SIZE, eerom_data.ReadBuff);
	}
		
    SoftReset();
#endif
}

//IP_setting_1
//IP_setting_1按键按下，调出两个窗口
void SettingView::IP_setting_1_fun()
{
    IP_setting_1.setVisible(true);
    IP_setting_1.invalidate();
    
    keyboard1.initialize();
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

//CloseSetting_1
//CloseSetting_1按键按下，调出两个窗口
void SettingView::CloseSetting_1_fun()
{
    IP_setting_1.setVisible(false);
    IP_setting_1.invalidate();
    keyboard1.setVisible(false);
    keyboard1.invalidate();
    SetButState(IP_setting_1_butAry, 5, false);
}

//IP_setting_1保存按钮
void SettingView::Save_1_Fun(void)
{
    /*往AT24C02写入*/
#ifndef SIMULATOR
    /*在下面更新过
    eeprom_bsmu_WR.cu_num
    eeprom_bsmu_WR.poll_T
    */
//    memcpy(eerom_data.WriteBuff, &SettingBuff, sizeof(SettingBuff));
//    AT24Cxx_SeqWrite(0x00, CAPACITY_SIZE, eerom_data.WriteBuff);
#endif
    //本本地服务器更新显示
    Unicode::snprintf(ip_text1_1Buffer1, IP_TEXT1_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_1[0]);
    Unicode::snprintf(ip_text1_1Buffer2, IP_TEXT1_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_1[1]);
    ip_text1_1.invalidate();
    Unicode::snprintf(ip_text2_1Buffer1, IP_TEXT2_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_1[2]);
    Unicode::snprintf(ip_text2_1Buffer2, IP_TEXT2_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_1[3]);
    ip_text2_1.invalidate();
    Unicode::snprintf(port_textBuffer, PORT_TEXT_SIZE, "%d", SettingBuff.port);
    port_text.invalidate();
}

//IP_setting_2
//IP_setting_2按键按下，调出两个窗口
void SettingView::IP_setting_2_fun()
{
    IP_setting_2.setVisible(true);
    IP_setting_2.invalidate();
    
    keyboard1.initialize();
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

//CloseSetting_2
//CloseSetting_2按键按下，调出两个窗口
void SettingView::CloseSetting_2_fun()
{
    IP_setting_2.setVisible(false);
    IP_setting_2.invalidate();
    keyboard1.setVisible(false);
    keyboard1.invalidate();
    SetButState(IP_setting_2_butAry, 5, false);
}

//IP_setting_2保存按钮
void SettingView::Save_2_Fun(void)
{
    /*往AT24C02写入*/
#ifndef SIMULATOR
    /*在下面更新过
    eeprom_bsmu_WR.cu_num
    eeprom_bsmu_WR.poll_T
    */
    // eeprom_bsmu_WR.IP_ADD[0] = IP_ADD_1.getSelectedItem();
//    memcpy(eerom_data.WriteBuff, &SettingBuff, sizeof(SettingBuff));
//    AT24Cxx_SeqWrite(0x00, CAPACITY_SIZE, eerom_data.WriteBuff);
#endif
    //4G模块更新显示
    Unicode::snprintf(ip_1_text1_1Buffer1, IP_1_TEXT1_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_2[0]);
    Unicode::snprintf(ip_1_text1_1Buffer2, IP_1_TEXT1_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_2[1]);
    ip_1_text1_1.invalidate();
    Unicode::snprintf(ip_1_text2_1Buffer1, IP_1_TEXT2_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_2[2]);
    Unicode::snprintf(ip_1_text2_1Buffer2, IP_1_TEXT2_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_2[3]);
    ip_1_text1_1.invalidate();
    Unicode::snprintf(port_1_textBuffer, PORT_1_TEXT_SIZE, "%d", SettingBuff.port_1);
    port_1_text.invalidate();
}


//CAN_kbps
//更新滚轮显示
void SettingView::CAN_kbpsUpdateItem(CustomContainer3& item, int16_t itemIndex)
{
    if(itemIndex == 0)
        item.SetText(500, 2000);
    else if (itemIndex == 1)
        item.SetText(500, 1000);
    else if (itemIndex == 2)
        item.SetText(500, 500);
    else if (itemIndex == 3)
        item.SetText(250, 250);
    else if (itemIndex == 4)
        item.SetText(125, 125);    
}

void SettingView::CAN_kbpsUpdateCenterItem(CustomContainer4& item, int16_t itemIndex)
{
    if(itemIndex == 0)
        item.SetText(500, 2000);
    else if (itemIndex == 1)
        item.SetText(500, 1000);
    else if (itemIndex == 2)
        item.SetText(500, 500);
    else if (itemIndex == 3)
        item.SetText(250, 250);
    else if (itemIndex == 4)
        item.SetText(125, 125);    
}

//RS485_kbps
//更新滚轮显示
void SettingView::RS485_kbpsUpdateItem(CustomContainer1& item, int16_t itemIndex)
{
    if(itemIndex == 0)
        item.SetText(115200);
    else if (itemIndex == 1)
        item.SetText(57600);
    else if (itemIndex == 2)
        item.SetText(38400);
    else if (itemIndex == 3)
        item.SetText(19200);
    else if (itemIndex == 4)
        item.SetText(9600);    
    else if (itemIndex == 5)
        item.SetText(4800);    
}

void SettingView::RS485_kbpsUpdateCenterItem(CustomContainer2& item, int16_t itemIndex)
{
    if(itemIndex == 0)
        item.SetText(115200);
    else if (itemIndex == 1)
        item.SetText(57600);
    else if (itemIndex == 2)
        item.SetText(38400);
    else if (itemIndex == 3)
        item.SetText(19200);
    else if (itemIndex == 4)
        item.SetText(9600);    
    else if (itemIndex == 5)
        item.SetText(4800);    
}

void SettingView::SetButState(WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >** p, uint8_t num, bool state)
{
    for(uint8_t i = 0; i < num; i++){
        p[i]->setPressed(state);
        p[i]->invalidate();
    }
}

void SettingView::IP_ADD_1_fun()
{
    if(IP_ADD_1.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        IP_ADD_1.setPressed(true);
        IP_ADD_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD[0], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_2_fun()
{
    if(IP_ADD_2.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        IP_ADD_2.setPressed(true);
        IP_ADD_2.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD[1], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_3_fun()
{
    if(IP_ADD_3.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        IP_ADD_3.setPressed(true);
        IP_ADD_3.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD[2], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_4_fun()
{
    if(IP_ADD_4.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        IP_ADD_4.setPressed(true);
        IP_ADD_4.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD[3], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::GATEWAY_1_fun()
{
    if(GATEWAY_1.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        GATEWAY_1.setPressed(true);
        GATEWAY_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.GATEWAY[0], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::GATEWAY_2_fun()
{
    if(GATEWAY_2.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        GATEWAY_2.setPressed(true);
        GATEWAY_2.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.GATEWAY[1], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::GATEWAY_3_fun()
{
    if(GATEWAY_3.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        GATEWAY_3.setPressed(true);
        GATEWAY_3.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.GATEWAY[2], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::GATEWAY_4_fun()
{
    if(GATEWAY_4.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        GATEWAY_4.setPressed(true);
        GATEWAY_4.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.GATEWAY[3], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::NETMASK_1_fun()
{
    if(NETMASK_1.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        NETMASK_1.setPressed(true);
        NETMASK_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.NETMASK[0], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::NETMASK_2_fun()
{
    if(NETMASK_2.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        NETMASK_2.setPressed(true);
        NETMASK_2.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.NETMASK[1], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::NETMASK_3_fun()
{
    if(NETMASK_3.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        NETMASK_3.setPressed(true);
        NETMASK_3.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.NETMASK[2], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::NETMASK_4_fun()
{
    if(NETMASK_4.getPressed()){
        SetButState(IP_setting_butAry, 12, false);
        NETMASK_4.setPressed(true);
        NETMASK_4.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.NETMASK[3], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}


void SettingView::IP_ADD_1_1_fun()
{
    if(IP_ADD_1_1.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        IP_ADD_1_1.setPressed(true);
        IP_ADD_1_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_1[0], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_1_2_fun()
{
    if(IP_ADD_1_2.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        IP_ADD_1_2.setPressed(true);
        IP_ADD_1_2.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_1[1], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_1_3_fun()
{
    if(IP_ADD_1_3.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        IP_ADD_1_3.setPressed(true);
        IP_ADD_1_3.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_1[2], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_1_4_fun()
{
    if(IP_ADD_1_4.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        IP_ADD_1_4.setPressed(true);
        IP_ADD_1_4.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_1[3], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::PORT_fun()
{
    if(PORT.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        PORT.setPressed(true);
        PORT.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.port, 2);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_2_1_fun()
{
    if(IP_ADD_2_1.getPressed()){
        SetButState(IP_setting_2_butAry, 5, false);
        IP_ADD_2_1.setPressed(true);
        IP_ADD_2_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_2[0], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_2_2_fun()
{
    if(IP_ADD_2_2.getPressed()){
        SetButState(IP_setting_2_butAry, 5, false);
        IP_ADD_2_2.setPressed(true);
        IP_ADD_2_2.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_2[1], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_2_3_fun()
{
    if(IP_ADD_2_3.getPressed()){
        SetButState(IP_setting_2_butAry, 5, false);
        IP_ADD_2_3.setPressed(true);
        IP_ADD_2_3.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_2[2], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_2_4_fun()
{
    if(IP_ADD_2_4.getPressed()){
        SetButState(IP_setting_2_butAry, 5, false);
        IP_ADD_2_4.setPressed(true);
        IP_ADD_2_4.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.IP_ADD_2[3], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::PORT_1_fun()
{
    if(PORT_1.getPressed()){
        SetButState(IP_setting_2_butAry, 5, false);
        PORT_1.setPressed(true);
        PORT_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff.port_1, 2);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}


