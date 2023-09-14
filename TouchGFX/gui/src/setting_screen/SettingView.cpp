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
#include "BitmapDatabase.hpp"
#include <touchgfx/Unicode.hpp>
#include <touchgfx/widgets/Image.hpp>
#include <images/BitmapDatabase.hpp>

#ifndef SIMULATOR
extern "C"{
#include "at24cxx.h"
#include "string.h"
#include "stm32h7xx_hal.h"
#include "core_cm7.h"	

void SoftReset(void)
{
    __set_FAULTMASK(1); //关闭所有中断
    NVIC_SystemReset();
}
};
#endif

#endif


SettingView::SettingView() {

}

void SettingView::setupScreen() 
{
   /* SettingViewBase::setupScreen();*/

    //按键数组的初始化
   
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
    IP_setting_1_butAry[0] = &IP_ADD_1_1; //对应本地服务器IP设置的4个按键
    IP_setting_1_butAry[1] = &IP_ADD_1_2;
    IP_setting_1_butAry[2] = &IP_ADD_1_3;
    IP_setting_1_butAry[3] = &IP_ADD_1_4;
    IP_setting_1_butAry[4] = &PORT; //端口

    //IP_setting_2这个界面的有关按键
    IP_setting_2_butAry[0] = &IP_ADD_2_1; //对应4G模块IP设置的4个按键
    IP_setting_2_butAry[1] = &IP_ADD_2_2;
    IP_setting_2_butAry[2] = &IP_ADD_2_3;
    IP_setting_2_butAry[3] = &IP_ADD_2_4;
    IP_setting_2_butAry[4] = &PORT_1;//端口

    //zu_set界面中的按键
    zu_set_butAry[0] = &zu_setbutton;//对应簇装机数的按键

    t_set_butAry[0] = &t_setbutton;//对应采集周期的按键

    //还要SettingBuff和EEPROM中一致（初值）
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

    SettingBuff_temp.IP_ADD_1[0] = 0;
    SettingBuff_temp.IP_ADD_1[1] = 0;
    SettingBuff_temp.IP_ADD_1[2] = 0;
    SettingBuff_temp.IP_ADD_1[3] = 0;
    SettingBuff_temp.port = 0;

    SettingBuff_temp.IP_ADD_2[0] = 0;
    SettingBuff_temp.IP_ADD_2[1] = 0;
    SettingBuff_temp.IP_ADD_2[2] = 0;
    SettingBuff_temp.IP_ADD_2[3] = 0;
    SettingBuff_temp.port_1 = 0;

    SettingBuff_temp.cu_num = 20;
    SettingBuff_temp.poll_T = 5;



//不在模拟器中,在实际中要将EEPROM中的数据拷贝过来
#ifndef SIMULATOR
    memcpy(&SettingBuff, &bsmuSetting, sizeof(EEPROM_BSMU_tem));
#endif

    //装机的簇数量和轮询周期参数显示初始化
    /*poll_T.setValue(SettingBuff.poll_T);*/
    Unicode::snprintf(poll_tBuffer, POLL_T_SIZE, "%d", SettingBuff.poll_T); //轮询中x10去掉
    poll_t.invalidate();
    
    /*cu_NUM.setValue(SettingBuff.cu_num);*/
    Unicode::snprintf(cu_numBuffer, CU_NUM_SIZE, "%d",  SettingBuff.cu_num );
    cu_num.invalidate();

    //初始化本机IP相关参数显示
    //Unicode::snprintf(Buffer[0], 10, "%03d", SettingBuff.IP_ADD[0]);     //赋值给临时变量，用作显示更新
    //IP_ADD_1.setWildcardTextBuffer(Buffer[0]);
    //IP_ADD_1.invalidate();
    //Unicode::snprintf(Buffer[1], 10, "%03d", SettingBuff.IP_ADD[1]);
    //IP_ADD_2.setWildcardTextBuffer(Buffer[1]);
    //IP_ADD_2.invalidate();
    //Unicode::snprintf(Buffer[2], 10, "%03d", SettingBuff.IP_ADD[2]);
    //IP_ADD_3.setWildcardTextBuffer(Buffer[2]);
    //IP_ADD_3.invalidate();
    //Unicode::snprintf(Buffer[3], 10, "%03d", SettingBuff.IP_ADD[3]);
    //IP_ADD_4.setWildcardTextBuffer(Buffer[3]);
    //IP_ADD_4.invalidate();

    //Unicode::snprintf(Buffer[4], 10, "%03d", SettingBuff.GATEWAY[0]);
    //GATEWAY_1.setWildcardTextBuffer(Buffer[4]);
    //GATEWAY_1.invalidate();
    //Unicode::snprintf(Buffer[5], 10, "%03d", SettingBuff.GATEWAY[1]);
    //GATEWAY_2.setWildcardTextBuffer(Buffer[5]);
    //GATEWAY_2.invalidate();
    //Unicode::snprintf(Buffer[6], 10, "%03d", SettingBuff.GATEWAY[2]);
    //GATEWAY_3.setWildcardTextBuffer(Buffer[6]);
    //GATEWAY_3.invalidate();
    //Unicode::snprintf(Buffer[7], 10, "%03d", SettingBuff.GATEWAY[3]);
    //GATEWAY_4.setWildcardTextBuffer(Buffer[7]);
    //GATEWAY_4.invalidate();

    //Unicode::snprintf(Buffer[8], 10, "%03d", SettingBuff.NETMASK[0]);
    //NETMASK_1.setWildcardTextBuffer(Buffer[8]);
    //NETMASK_1.invalidate();
    //Unicode::snprintf(Buffer[9], 10, "%03d", SettingBuff.NETMASK[1]);
    //NETMASK_2.setWildcardTextBuffer(Buffer[9]);
    //NETMASK_2.invalidate();
    //Unicode::snprintf(Buffer[10], 10, "%03d", SettingBuff.NETMASK[2]);
    //NETMASK_3.setWildcardTextBuffer(Buffer[10]);
    //NETMASK_3.invalidate();
    //Unicode::snprintf(Buffer[11], 10, "%03d", SettingBuff.NETMASK[3]);
    //NETMASK_4.setWildcardTextBuffer(Buffer[11]);
    //NETMASK_4.invalidate();

    //初始化本地服务器IP相关参数显示
    Unicode::snprintf(Buffer1[0], 10, "%03d", SettingBuff_temp.IP_ADD_1[0]);
    IP_ADD_1_1.setWildcardTextBuffer(Buffer1[0]);
    IP_ADD_1_1.invalidate();
    Unicode::snprintf(Buffer1[1], 10, "%03d", SettingBuff_temp.IP_ADD_1[1]);
    IP_ADD_1_2.setWildcardTextBuffer(Buffer1[1]);
    IP_ADD_1_2.invalidate();
    Unicode::snprintf(Buffer1[2], 10, "%03d", SettingBuff_temp.IP_ADD_1[2]);
    IP_ADD_1_3.setWildcardTextBuffer(Buffer1[2]);
    IP_ADD_1_3.invalidate();
    Unicode::snprintf(Buffer1[3], 10, "%03d", SettingBuff_temp.IP_ADD_1[3]);
    IP_ADD_1_4.setWildcardTextBuffer(Buffer1[3]);
    IP_ADD_1_4.invalidate();
    Unicode::snprintf(Buffer1[4], 10, "%d", SettingBuff_temp.port);
    PORT.setWildcardTextBuffer(Buffer1[4]);
    PORT.invalidate();

    //初始化4G模块IP相关参数显示
    Unicode::snprintf(Buffer2[0], 10, "%03d", SettingBuff_temp.IP_ADD_2[0]);
    IP_ADD_2_1.setWildcardTextBuffer(Buffer2[0]);
    IP_ADD_2_1.invalidate();
    Unicode::snprintf(Buffer2[1], 10, "%03d", SettingBuff_temp.IP_ADD_2[1]);
    IP_ADD_2_2.setWildcardTextBuffer(Buffer2[1]);
    IP_ADD_2_2.invalidate();
    Unicode::snprintf(Buffer2[2], 10, "%03d", SettingBuff_temp.IP_ADD_2[2]);
    IP_ADD_2_3.setWildcardTextBuffer(Buffer2[2]);
    IP_ADD_2_3.invalidate();
    Unicode::snprintf(Buffer2[3], 10, "%03d", SettingBuff_temp.IP_ADD_2[3]);
    IP_ADD_2_4.setWildcardTextBuffer(Buffer2[3]);
    IP_ADD_2_4.invalidate();
    Unicode::snprintf(Buffer2[4], 10, "%d", SettingBuff_temp.port_1);
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
} 


void SettingView::tearDownScreen()
{
    
}



//mSettingView每刷新10次
void SettingView::SettingViewTick()
{
    //本机配置界面中
    //if(IP_setting.isVisible() == true){   //在弹窗界面打开时，定期刷新数据
    //    //如果虚拟键盘曾经按下ok
    //    if(keyboard1.refreshFlag){
    //        keyboard1.refreshFlag = 0;
    //        Unicode::snprintf(Buffer[0], 10, "%03d", SettingBuff.IP_ADD[0]);
    //        IP_ADD_1.setWildcardTextBuffer(Buffer[0]);
    //        IP_ADD_1.invalidate();
    //        Unicode::snprintf(Buffer[1], 10, "%03d", SettingBuff.IP_ADD[1]);
    //        IP_ADD_2.setWildcardTextBuffer(Buffer[1]);
    //        IP_ADD_2.invalidate();
    //        Unicode::snprintf(Buffer[2], 10, "%03d", SettingBuff.IP_ADD[2]);
    //        IP_ADD_3.setWildcardTextBuffer(Buffer[2]);
    //        IP_ADD_3.invalidate();
    //        Unicode::snprintf(Buffer[3], 10, "%03d", SettingBuff.IP_ADD[3]);
    //        IP_ADD_4.setWildcardTextBuffer(Buffer[3]);
    //        IP_ADD_4.invalidate();

    //        Unicode::snprintf(Buffer[4], 10, "%03d", SettingBuff.GATEWAY[0]);
    //        GATEWAY_1.setWildcardTextBuffer(Buffer[4]);
    //        GATEWAY_1.invalidate();
    //        Unicode::snprintf(Buffer[5], 10, "%03d", SettingBuff.GATEWAY[1]);
    //        GATEWAY_2.setWildcardTextBuffer(Buffer[5]);
    //        GATEWAY_2.invalidate();
    //        Unicode::snprintf(Buffer[6], 10, "%03d", SettingBuff.GATEWAY[2]);
    //        GATEWAY_3.setWildcardTextBuffer(Buffer[6]);
    //        GATEWAY_3.invalidate();
    //        Unicode::snprintf(Buffer[7], 10, "%03d", SettingBuff.GATEWAY[3]);
    //        GATEWAY_4.setWildcardTextBuffer(Buffer[7]);
    //        GATEWAY_4.invalidate();

    //        Unicode::snprintf(Buffer[8], 10, "%03d", SettingBuff.NETMASK[0]);
    //        NETMASK_1.setWildcardTextBuffer(Buffer[8]);
    //        NETMASK_1.invalidate();
    //        Unicode::snprintf(Buffer[9], 10, "%03d", SettingBuff.NETMASK[1]);
    //        NETMASK_2.setWildcardTextBuffer(Buffer[9]);
    //        NETMASK_2.invalidate();
    //        Unicode::snprintf(Buffer[10], 10, "%03d", SettingBuff.NETMASK[2]);
    //        NETMASK_3.setWildcardTextBuffer(Buffer[10]);
    //        NETMASK_3.invalidate();
    //        Unicode::snprintf(Buffer[11], 10, "%03d", SettingBuff.NETMASK[3]);
    //        NETMASK_4.setWildcardTextBuffer(Buffer[11]);
    //        NETMASK_4.invalidate();
    //    }
    //}
    //本地服务器配置界面中
    if(IP_setting_1.isVisible() == true){
        //如果虚拟键盘曾经按下ok
        if(keyboard1.refreshFlag){
            keyboard1.refreshFlag = 0;
            Unicode::snprintf(Buffer1[0], 10, "%03d", SettingBuff_temp.IP_ADD_1[0]);
            IP_ADD_1_1.setWildcardTextBuffer(Buffer1[0]);
            IP_ADD_1_1.invalidate();
            Unicode::snprintf(Buffer1[1], 10, "%03d", SettingBuff_temp.IP_ADD_1[1]);
            IP_ADD_1_2.setWildcardTextBuffer(Buffer1[1]);
            IP_ADD_1_2.invalidate();
            Unicode::snprintf(Buffer1[2], 10, "%03d", SettingBuff_temp.IP_ADD_1[2]);
            IP_ADD_1_3.setWildcardTextBuffer(Buffer1[2]);
            IP_ADD_1_3.invalidate();
            Unicode::snprintf(Buffer1[3], 10, "%03d", SettingBuff_temp.IP_ADD_1[3]);
            IP_ADD_1_4.setWildcardTextBuffer(Buffer1[3]);
            IP_ADD_1_4.invalidate();
            Unicode::snprintf(Buffer1[4], 10, "%d", SettingBuff_temp.port);
            PORT.setWildcardTextBuffer(Buffer1[4]);
            PORT.invalidate();
        }
    }
    //4G模块配置界面中
    if(IP_setting_2.isVisible() == true){
        //如果虚拟键盘曾经按下ok
        if(keyboard1.refreshFlag){
            keyboard1.refreshFlag = 0;
            Unicode::snprintf(Buffer2[0], 10, "%03d", SettingBuff_temp.IP_ADD_2[0]);
            IP_ADD_2_1.setWildcardTextBuffer(Buffer2[0]);
            IP_ADD_2_1.invalidate();
            Unicode::snprintf(Buffer2[1], 10, "%03d", SettingBuff_temp.IP_ADD_2[1]);
            IP_ADD_2_2.setWildcardTextBuffer(Buffer2[1]);
            IP_ADD_2_2.invalidate();
            Unicode::snprintf(Buffer2[2], 10, "%03d", SettingBuff_temp.IP_ADD_2[2]);
            IP_ADD_2_3.setWildcardTextBuffer(Buffer2[2]);
            IP_ADD_2_3.invalidate();
            Unicode::snprintf(Buffer2[3], 10, "%03d", SettingBuff_temp.IP_ADD_2[3]);
            IP_ADD_2_4.setWildcardTextBuffer(Buffer2[3]);
            IP_ADD_2_4.invalidate();
            Unicode::snprintf(Buffer2[4], 10, "%d", SettingBuff_temp.port_1);
            PORT_1.setWildcardTextBuffer(Buffer2[4]);
            PORT_1.invalidate();
        }
     }  
    
    //装机组数配置
    if (zu_set.isVisible() == true)
     {
        if(keyboard1.refreshFlag){
            keyboard1.refreshFlag = 0;
            Unicode::snprintf(zu_Buffer[0], 10, "%02d", SettingBuff_temp.cu_num);
            zu_setbutton.setWildcardTextBuffer(zu_Buffer[0]);
            zu_setbutton.invalidate();
        }
     }


    //采集周期配置
    if (t_set.isVisible() == true)
    {
        if (keyboard1.refreshFlag) {
            keyboard1.refreshFlag = 0;
            Unicode::snprintf(t_Buffer[0], 10, "%d", SettingBuff_temp.poll_T);
            t_setbutton.setWildcardTextBuffer(t_Buffer[0]);
            t_setbutton.invalidate();
        }
    }
}

//SettingView()还需要更新下初始值
//更新簇数量
//void SettingView::SetCuNum(int value)
//{
//#ifndef SIMULATOR
//    SettingBuff.cu_num = value;
//    bsmuSetting.cu_num = value;
//#endif
//    Unicode::snprintf(cu_numBuffer, CU_NUM_SIZE, "%d", value);
//    cu_num.invalidate();
//}
//
////更新轮询周期
//void SettingView::SetPoll_T(int value)
//{
//#ifndef SIMULATOR
//    SettingBuff.poll_T = value;
//    bsmuSetting.poll_T = value;
//#endif
//    Unicode::snprintf(poll_tBuffer, POLL_T_SIZE, "%d", value*10);
//    poll_t.invalidate();
//}



//IP_setting


void SettingView::IP_setting_fun()//IP_setting按键按下
{
    IP_setting.setVisible(true);//打开IP设置弹窗
    IP_setting.invalidate();

    keyboard1.initialize(); //打开键盘输入，并初始化
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

//CloseSetting
void SettingView::CloseSetting_fun() //关闭弹窗
{
    IP_setting.setVisible(false);
    IP_setting.invalidate();
    keyboard1.setVisible(false);
    keyboard1.invalidate();
    SetButState(IP_setting_butAry, 5, false);
}

//IP_setting保存按钮
void SettingView::SaveFun(void) //当Save_but按键按下，将结构体中的数据存入EEPROM 用于ip_setting弹窗
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


void SettingView::save_all_fun(void) //点击save_all后，将所有数据进行保存
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
void SettingView::IP_setting_1_fun()//IP_setting_1按键按下
{
    IP_setting_1.setVisible(true);//打开本地服务器设置弹窗
    IP_setting_1.invalidate();
    
    keyboard1.initialize(); //打开键盘输入，并初始化
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

//CloseSetting_1
void SettingView::CloseSetting_1_fun()//CloseSetting_1按键按下
{
    SettingBuff_temp.IP_ADD_1[0] = SettingBuff.IP_ADD_1[0];
    SettingBuff_temp.IP_ADD_1[1] = SettingBuff.IP_ADD_1[1];
    SettingBuff_temp.IP_ADD_1[2] = SettingBuff.IP_ADD_1[2];
    SettingBuff_temp.IP_ADD_1[3] = SettingBuff.IP_ADD_1[3];
    SettingBuff_temp.port = SettingBuff.port;

    Unicode::snprintf(Buffer1[0], 10, "%03d", SettingBuff_temp.IP_ADD_1[0]);
    IP_ADD_1_1.setWildcardTextBuffer(Buffer1[0]);
    IP_ADD_1_1.invalidate();
    Unicode::snprintf(Buffer1[1], 10, "%03d", SettingBuff_temp.IP_ADD_1[1]);
    IP_ADD_1_2.setWildcardTextBuffer(Buffer1[1]);
    IP_ADD_1_2.invalidate();
    Unicode::snprintf(Buffer1[2], 10, "%03d", SettingBuff_temp.IP_ADD_1[2]);
    IP_ADD_1_3.setWildcardTextBuffer(Buffer1[2]);
    IP_ADD_1_3.invalidate();
    Unicode::snprintf(Buffer1[3], 10, "%03d", SettingBuff_temp.IP_ADD_1[3]);
    IP_ADD_1_4.setWildcardTextBuffer(Buffer1[3]);
    IP_ADD_1_4.invalidate();
    Unicode::snprintf(Buffer1[4], 10, "%d", SettingBuff_temp.port);
    PORT.setWildcardTextBuffer(Buffer1[4]);
    PORT.invalidate();

    IP_setting_1.setVisible(false);//关闭本地服务器设置弹窗
    IP_setting_1.invalidate();

    keyboard1.setVisible(false);//关闭键盘输入
    keyboard1.invalidate();

}

//IP_setting_1保存按钮
void SettingView::Save_1_Fun(void)
{
    //本地服务器更新显示
    SettingBuff.IP_ADD_1[0] = SettingBuff_temp.IP_ADD_1[0];
    SettingBuff.IP_ADD_1[1] = SettingBuff_temp.IP_ADD_1[1];
    SettingBuff.IP_ADD_1[2] = SettingBuff_temp.IP_ADD_1[2];
    SettingBuff.IP_ADD_1[3] = SettingBuff_temp.IP_ADD_1[3];
    SettingBuff.port = SettingBuff_temp.port;

    Unicode::snprintf(ip_text1_1Buffer1, IP_TEXT1_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_1[0]);
    Unicode::snprintf(ip_text1_1Buffer2, IP_TEXT1_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_1[1]);
    ip_text1_1.invalidate();
    Unicode::snprintf(ip_text2_1Buffer1, IP_TEXT2_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_1[2]);
    Unicode::snprintf(ip_text2_1Buffer2, IP_TEXT2_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_1[3]);
    ip_text2_1.invalidate();
    Unicode::snprintf(port_textBuffer, PORT_TEXT_SIZE, "%d", SettingBuff.port);
    port_text.invalidate();

    IP_setting_1.setVisible(false);//关闭本地服务器设置弹窗
    IP_setting_1.invalidate();

    keyboard1.setVisible(false);//关闭键盘输入
    keyboard1.invalidate();
}


//IP_setting_2
void SettingView::IP_setting_2_fun()//IP_setting_2按键按下
{
    IP_setting_2.setVisible(true);//打开4G模块设计弹窗
    IP_setting_2.invalidate();
    
    keyboard1.initialize();//打开键盘输入，并初始化
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

//CloseSetting_2
void SettingView::CloseSetting_2_fun()//CloseSetting_2按键按下
{
    SettingBuff_temp.IP_ADD_2[0] = SettingBuff.IP_ADD_2[0];
    SettingBuff_temp.IP_ADD_2[1] = SettingBuff.IP_ADD_2[1];
    SettingBuff_temp.IP_ADD_2[2] = SettingBuff.IP_ADD_2[2];
    SettingBuff_temp.IP_ADD_2[3] = SettingBuff.IP_ADD_2[3];
    SettingBuff_temp.port_1 = SettingBuff.port_1;

    Unicode::snprintf(Buffer2[0], 10, "%03d", SettingBuff_temp.IP_ADD_2[0]);
    IP_ADD_2_1.setWildcardTextBuffer(Buffer2[0]);
    IP_ADD_2_1.invalidate();
    Unicode::snprintf(Buffer2[1], 10, "%03d", SettingBuff_temp.IP_ADD_2[1]);
    IP_ADD_2_2.setWildcardTextBuffer(Buffer2[1]);
    IP_ADD_2_2.invalidate();
    Unicode::snprintf(Buffer2[2], 10, "%03d", SettingBuff_temp.IP_ADD_2[2]);
    IP_ADD_2_3.setWildcardTextBuffer(Buffer2[2]);
    IP_ADD_2_3.invalidate();
    Unicode::snprintf(Buffer2[3], 10, "%03d", SettingBuff_temp.IP_ADD_2[3]);
    IP_ADD_2_4.setWildcardTextBuffer(Buffer2[3]);
    IP_ADD_2_4.invalidate();
    Unicode::snprintf(Buffer2[4], 10, "%d", SettingBuff_temp.port_1);
    PORT_1.setWildcardTextBuffer(Buffer2[4]);
    PORT_1.invalidate();


    IP_setting_2.setVisible(false);//关闭弹窗
    IP_setting_2.invalidate();

    keyboard1.setVisible(false);
    keyboard1.invalidate();

}

//IP_setting_2保存按钮
void SettingView::Save_2_Fun(void)
{
    SettingBuff.IP_ADD_2[0] = SettingBuff_temp.IP_ADD_2[0];
    SettingBuff.IP_ADD_2[1] = SettingBuff_temp.IP_ADD_2[1];
    SettingBuff.IP_ADD_2[2] = SettingBuff_temp.IP_ADD_2[2];
    SettingBuff.IP_ADD_2[3] = SettingBuff_temp.IP_ADD_2[3];
    SettingBuff.port_1 = SettingBuff_temp.port_1;

    //4G模块更新显示，仅用于数据的更新显示
    Unicode::snprintf(ip_1_text1_1Buffer1, IP_1_TEXT1_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_2[0]);
    Unicode::snprintf(ip_1_text1_1Buffer2, IP_1_TEXT1_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_2[1]);
    ip_1_text1_1.invalidate();

    Unicode::snprintf(ip_1_text2_1Buffer1, IP_1_TEXT2_1BUFFER1_SIZE, "%03d", SettingBuff.IP_ADD_2[2]);
    Unicode::snprintf(ip_1_text2_1Buffer2, IP_1_TEXT2_1BUFFER2_SIZE, "%03d", SettingBuff.IP_ADD_2[3]);
    ip_1_text2_1.invalidate();

    Unicode::snprintf(port_1_textBuffer, PORT_1_TEXT_SIZE, "%d", SettingBuff.port_1);
    port_1_text.invalidate();

    IP_setting_2.setVisible(false);//关闭本地服务器设置弹窗
    IP_setting_2.invalidate();

    keyboard1.setVisible(false);//关闭键盘输入
    keyboard1.invalidate();

}

//true为按下状态，false为释放状态
void SettingView::SetButState(WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >** p, uint8_t num, bool state)
{
    for(uint8_t i = 0; i < num; i++){
        p[i]->setPressed(state);
        p[i]->invalidate();
    }
}

//void SettingView::IP_ADD_1_fun()
//{
//    if(IP_ADD_1.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        IP_ADD_1.setPressed(true);
//        IP_ADD_1.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.IP_ADD[0], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::IP_ADD_2_fun()
//{
//    if(IP_ADD_2.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        IP_ADD_2.setPressed(true);
//        IP_ADD_2.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.IP_ADD[1], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::IP_ADD_3_fun()
//{
//    if(IP_ADD_3.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        IP_ADD_3.setPressed(true);
//        IP_ADD_3.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.IP_ADD[2], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::IP_ADD_4_fun()
//{
//    if(IP_ADD_4.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        IP_ADD_4.setPressed(true);
//        IP_ADD_4.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.IP_ADD[3], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::GATEWAY_1_fun()
//{
//    if(GATEWAY_1.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        GATEWAY_1.setPressed(true);
//        GATEWAY_1.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.GATEWAY[0], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::GATEWAY_2_fun()
//{
//    if(GATEWAY_2.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        GATEWAY_2.setPressed(true);
//        GATEWAY_2.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.GATEWAY[1], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::GATEWAY_3_fun()
//{
//    if(GATEWAY_3.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        GATEWAY_3.setPressed(true);
//        GATEWAY_3.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.GATEWAY[2], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::GATEWAY_4_fun()
//{
//    if(GATEWAY_4.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        GATEWAY_4.setPressed(true);
//        GATEWAY_4.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.GATEWAY[3], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::NETMASK_1_fun()
//{
//    if(NETMASK_1.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        NETMASK_1.setPressed(true);
//        NETMASK_1.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.NETMASK[0], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::NETMASK_2_fun()
//{
//    if(NETMASK_2.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        NETMASK_2.setPressed(true);
//        NETMASK_2.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.NETMASK[1], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::NETMASK_3_fun()
//{
//    if(NETMASK_3.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        NETMASK_3.setPressed(true);
//        NETMASK_3.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.NETMASK[2], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}
//
//void SettingView::NETMASK_4_fun()
//{
//    if(NETMASK_4.getPressed()){
//        SetButState(IP_setting_butAry, 12, false);
//        NETMASK_4.setPressed(true);
//        NETMASK_4.invalidate();
//        keyboard1.initialize();
//        keyboard1.SetBufPar(&SettingBuff.NETMASK[3], 1);
//    }
//    else
//        keyboard1.SetBufPar(NULL, 1);
//}


void SettingView::IP_ADD_1_1_fun() //点击本地服务器设置弹窗第一个ip设置按键
{
    if(IP_ADD_1_1.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false); //先令所有的按键失效
        IP_ADD_1_1.setPressed(true);//选中当前按下的按键
        IP_ADD_1_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_1[0], 1); //将键盘的输入存入临时结构体变量中
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_1_2_fun()//第二个按键
{
    if(IP_ADD_1_2.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        IP_ADD_1_2.setPressed(true);
        IP_ADD_1_2.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_1[1], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_1_3_fun()//第三个按键
{
    if(IP_ADD_1_3.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        IP_ADD_1_3.setPressed(true);
        IP_ADD_1_3.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_1[2], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_1_4_fun()//第四个按键
{
    if(IP_ADD_1_4.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        IP_ADD_1_4.setPressed(true);
        IP_ADD_1_4.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_1[3], 1);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::PORT_fun()//第五个按键
{
    if(PORT.getPressed()){
        SetButState(IP_setting_1_butAry, 5, false);
        PORT.setPressed(true);
        PORT.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff_temp.port, 2);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::IP_ADD_2_1_fun()//4G模块第一个按键
{
    if(IP_ADD_2_1.getPressed()){
        SetButState(IP_setting_2_butAry, 5, false);
        IP_ADD_2_1.setPressed(true);
        IP_ADD_2_1.invalidate();
        keyboard1.initialize();
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_2[0], 1);
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
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_2[1], 1);
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
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_2[2], 1);
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
        keyboard1.SetBufPar(&SettingBuff_temp.IP_ADD_2[3], 1);
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
        keyboard1.SetBufPar(&SettingBuff_temp.port_1, 2);
    }
    else
        keyboard1.SetBufPar(NULL, 1);
}

void SettingView::SetCan1(int val1)
{
    Unicode::snprintf(canbaudrate1Buffer, CANBAUDRATE1_SIZE, "%d", val1);
    canbaudrate1.invalidate();
}

void SettingView::SetCan2(int val1)
{
    Unicode::snprintf(canbaudrate2Buffer, CANBAUDRATE2_SIZE, "%d", val1);
    canbaudrate2.invalidate();
}

void SettingView::SetRs485(int val1) 
{
    Unicode::snprintf(rs485textBuffer, RS485TEXT_SIZE,"%d", val1);
    rs485text.invalidate();
}

//can波特率设置
void SettingView::canbutton_clicked()  //单击打开can波特率选择列表，双击关闭
{
    static int canflag = 0;
    if (!canflag) {
        can_display.setVisible(true);

        can_imageup.setVisible(true);
        can_imagedown.setVisible(false);
    }
    else {
        can_display.setVisible(false);

        can_imageup.setVisible(false);
        can_imagedown.setVisible(true);
    }
    can_display.invalidate();      
    canflag = ~canflag;
}

void SettingView::setcanbaudrate125_125() //设置波特率，如果不在模拟器中，就赋值给结构体中的变量，存入rom中
{
#ifndef SIMULATOR
    SettingBuff.canBps = 4; //因为不涉及保存、取消等按键，所以这里不用临时变量
#endif
    SetCan1(125);
    SetCan2(125);
    can_display.setVisible(false);
    can_imageup.setVisible(false);
    can_imagedown.setVisible(true);

    can_display.invalidate();
}

void SettingView::setcanbaudrate250_250()
{
#ifndef SIMULATOR
    SettingBuff.canBps = 3;
#endif
    SetCan1(250);
    SetCan2(250);
    can_display.setVisible(false);
    can_imageup.setVisible(false);
    can_imagedown.setVisible(true);

    can_display.invalidate();

}
void SettingView::setcanbaudrate500_500()
{
#ifndef SIMULATOR
    SettingBuff.canBps = 2;
#endif
    SetCan1(500);
    SetCan2(500);
    can_display.setVisible(false);
    can_imageup.setVisible(false);
    can_imagedown.setVisible(true);

    can_display.invalidate();
}
void SettingView::setcanbaudrate500_1000()
{
#ifndef SIMULATOR
    SettingBuff.canBps = 1;
#endif
    SetCan1(500);
    SetCan2(1000);
    can_display.setVisible(false);
    can_imageup.setVisible(false);
    can_imagedown.setVisible(true);

    can_display.invalidate();
}
void SettingView::setcanbaudrate500_2000()
{
#ifndef SIMULATOR
    SettingBuff.canBps = 0;
#endif
    SetCan1(500);
    SetCan2(2000);
    can_display.setVisible(false);
    can_imageup.setVisible(false);
    can_imagedown.setVisible(true);

    can_display.invalidate();
}

//RS485设置，同can
void SettingView::RS485button_clicked()
{
    static int RSflag = 0;
    if (!RSflag) {
        rs485_display.setVisible(true);
        rs485_imageup.setVisible(true);
        rs485_imagedown.setVisible(false);
    }
    else {
        rs485_display.setVisible(false);
        rs485_imageup.setVisible(false);
        rs485_imagedown.setVisible(true);
    }
    rs485_display.invalidate();
    RSflag = ~RSflag;
}

void SettingView::setRS485baudrate_4800()
{
#ifndef SIMULATOR
    SettingBuff.RS485Bps = 5;
#endif
    SetRs485(4800);
    rs485_display.setVisible(false);
    rs485_imageup.setVisible(false);
    rs485_imagedown.setVisible(true);

    rs485_display.invalidate();
}

void SettingView::setRS485baudrate_9600()
{
#ifndef SIMULATOR
    SettingBuff.RS485Bps = 4;
#endif
    SetRs485(9600);
    rs485_display.setVisible(false);
    rs485_imageup.setVisible(false);
    rs485_imagedown.setVisible(true);

    rs485_display.invalidate();
}

void SettingView::setRS485baudrate_19200()
{
#ifndef SIMULATOR
    SettingBuff.RS485Bps = 3;
#endif
    SetRs485(19200);
    rs485_display.setVisible(false);
    rs485_imageup.setVisible(false);
    rs485_imagedown.setVisible(true);

    rs485_display.invalidate();
}

void SettingView::setRS485baudrate_38400()
{
#ifndef SIMULATOR
    SettingBuff.RS485Bps = 2;
#endif
    SetRs485(38400);
    rs485_display.setVisible(false);
    rs485_imageup.setVisible(false);
    rs485_imagedown.setVisible(true);

    rs485_display.invalidate();
}

void SettingView::setRS485baudrate_57600()
{
#ifndef SIMULATOR
    SettingBuff.RS485Bps = 1;
#endif
    SetRs485(57600);
    rs485_display.setVisible(false);
    rs485_imageup.setVisible(false);
    rs485_imagedown.setVisible(true);

    rs485_display.invalidate();
}

void SettingView::setRS485baudrate_115200()
{
#ifndef SIMULATOR
    SettingBuff.RS485Bps = 0;
#endif
    SetRs485(115200);
    rs485_display.setVisible(false);
    rs485_imageup.setVisible(false);
    rs485_imagedown.setVisible(true);

    rs485_display.invalidate();
}


//簇装机数
void SettingView::poll_t_set()//点击zhuangji_numButton
{
    t_set.setVisible(true);//打开设置弹窗
    t_set.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(true);//打开键盘，并初始化
    keyboard1.invalidate();
    keyboard1.SetBufPar(&SettingBuff_temp.poll_T, 2); 
}

//当簇装机数被点击后，打开设置界面的弹窗，默认设置弹窗里的参数是被选中的修改状态，以便按键直接输入数据
void SettingView::zhuangji_set() 
{
    zu_set.setVisible(true); //打开设置弹窗
    zu_set.invalidate();

    keyboard1.initialize(); 
    keyboard1.setVisible(true);//打开键盘
    keyboard1.invalidate();
    keyboard1.SetBufPar(&SettingBuff_temp.cu_num, 1); //将设置的值存储到临时变量中
}

void SettingView::zu_set_save()//点击保存，将临时的值赋值给最终存入EEPROM的变量
{
    //如果临时变量装机组数值的上限超过了20，就取20
    if (SettingBuff_temp.cu_num >= 20) SettingBuff_temp.cu_num = 20;
    SettingBuff.cu_num = SettingBuff_temp.cu_num; //完成赋值操作

    Unicode::snprintf(cu_numBuffer, CU_NUM_SIZE, "%d", SettingBuff.cu_num); //因为显示的文本格为字符串的形式，进行拷贝和类型转换
    cu_num.invalidate();

    zu_set.setVisible(false);
    zu_set.invalidate();

    keyboard1.initialize();//初始化
    keyboard1.setVisible(false);
    keyboard1.invalidate();
}


void SettingView::zu_set_close()//如果取消的话，将原来的SettingBuff.cu_num再覆盖掉SettingBuff_temp.cu_num,防止出现设置界面的设置值为上一次取消时的值
{
    SettingBuff_temp.cu_num = SettingBuff.cu_num;
    Unicode::snprintf(zu_Buffer[0], 10, "%02d", SettingBuff.cu_num);
    zu_setbutton.setWildcardTextBuffer(zu_Buffer[0]);
    zu_setbutton.invalidate();//取消按键，对temp的值进行覆盖，并更新显示

    zu_set.setVisible(false);
    zu_set.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(false);
    keyboard1.invalidate();

}

void SettingView::t_set_save()
{
    if (SettingBuff.poll_T >= 1000) SettingBuff_temp.poll_T = 1000;
    SettingBuff.poll_T = SettingBuff_temp.poll_T; //完成赋值操作

    Unicode::snprintf(poll_tBuffer, 10, "%d", SettingBuff.poll_T);
    poll_t.invalidate();

    t_set.setVisible(false);
    t_set.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(false);
    keyboard1.invalidate();
}

void SettingView::t_set_close() 
{
    SettingBuff_temp.poll_T = SettingBuff.poll_T;
    Unicode::snprintf(t_Buffer[0], 10, "%02d", SettingBuff_temp.poll_T);
    t_setbutton.setWildcardTextBuffer(t_Buffer[0]);
    t_setbutton.invalidate();//取消按键，对temp的值进行覆盖，并更新显示

    t_set.setVisible(false);
    t_set.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(false);
    keyboard1.invalidate();

 /*   SetButState(t_set_butAry, 1, false);*/
}

void SettingView::t_setbutton_fun() 
{
    //if (t_setbutton.getPressed()) {
    //    SetButState(t_set_butAry, 1, false);
    //    t_setbutton.setPressed(true);
    //    t_setbutton.invalidate();

    //    keyboard1.initialize();
    //    keyboard1.SetBufPar(&SettingBuff.poll_T, 2);
    //}
    //else
    //    keyboard1.SetBufPar(NULL, 2);
}


void SettingView::localnet_state()
{
#ifndef SIMULATOR
    if (local.getState() == true)
    {
        SettingBuff.local_flag = 1;
    }
    else
    {
        SettingBuff.local_flag = 0;
    }
#endif      
}

void SettingView::yunduannet_state()
{
#ifndef SIMULATOR
    if (yunduan.getState() == true)
    {
        SettingBuff.yunduan_flag = 1;
    }
    else
    {
        SettingBuff.yunduan_flag = 0;
    }
#endif    
}

void SettingView::local_ip_set()
{
    IP_setting_1.setVisible(true);
    IP_setting_1.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

void SettingView::local_port_set() 
{
    IP_setting_1.setVisible(true);
    IP_setting_1.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}


void SettingView::ip_4g_set() 
{
    IP_setting_2.setVisible(true);
    IP_setting_2.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}

void SettingView::port_4g_set()
{
    IP_setting_2.setVisible(true);
    IP_setting_2.invalidate();

    keyboard1.initialize();
    keyboard1.setVisible(true);
    keyboard1.invalidate();
}