/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2023-03-20 15:02:49
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-24 21:49:08
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\TouchGFX\gui\include\gui\setting_screen\SettingView.hpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef SETTINGVIEW_HPP
#define SETTINGVIEW_HPP

#include <gui_generated/setting_screen/SettingViewBase.hpp>
#include <gui/setting_screen/SettingPresenter.hpp>

//这个结构体要和EEPROM中的结构体EEPROM_BSMU保持一致
typedef struct
{
	uint8_t cu_num;
	uint8_t poll_T;      //这里的单位是10ms,使用的时候记得乘以10
	uint8_t IP_ADD[4];	 //本机网络配置
	uint8_t GATEWAY[4];	
	uint8_t NETMASK[4];	
	uint8_t IP_ADD_1[4];	 //本地服务器
	uint16_t port;
    uint8_t IP_ADD_2[4];	 //4G
	uint16_t port_1;
}EEPROM_BSMU_tem;


class SettingView : public SettingViewBase
{
public:
    SettingView();
    virtual ~SettingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void CAN_kbpsUpdateItem(CustomContainer3& item, int16_t itemIndex);
    virtual void CAN_kbpsUpdateCenterItem(CustomContainer4& item, int16_t itemIndex); 

    virtual void RS485_kbpsUpdateItem(CustomContainer1& item, int16_t itemIndex);
    virtual void RS485_kbpsUpdateCenterItem(CustomContainer2& item, int16_t itemIndex);


    virtual void SettingViewTick();

    virtual void SetCuNum(int value);
    virtual void SetPoll_T(int value);
    virtual void IP_setting_fun();
    virtual void IP_setting_1_fun();
    virtual void IP_setting_2_fun();
    virtual void CloseSetting_fun();
    virtual void CloseSetting_1_fun();
    virtual void CloseSetting_2_fun();
    virtual void SaveFun(void);
    virtual void Save_1_Fun(void);
    virtual void Save_2_Fun(void);
    virtual void save_all_fun(void);

    virtual void IP_ADD_1_fun();
    virtual void IP_ADD_2_fun();
    virtual void IP_ADD_3_fun();
    virtual void IP_ADD_4_fun();

    virtual void GATEWAY_1_fun();
    virtual void GATEWAY_2_fun();
    virtual void GATEWAY_3_fun();
    virtual void GATEWAY_4_fun();

    virtual void NETMASK_1_fun();
    virtual void NETMASK_2_fun();
    virtual void NETMASK_3_fun();
    virtual void NETMASK_4_fun();

    virtual void IP_ADD_1_1_fun();
    virtual void IP_ADD_1_2_fun();
    virtual void IP_ADD_1_3_fun();
    virtual void IP_ADD_1_4_fun();
    virtual void PORT_fun();

    virtual void IP_ADD_2_1_fun();
    virtual void IP_ADD_2_2_fun();
    virtual void IP_ADD_2_3_fun();
    virtual void IP_ADD_2_4_fun();
    virtual void PORT_1_fun();

    void SetButState(WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >** p, uint8_t num, bool state);

    
protected:
    EEPROM_BSMU_tem SettingBuff;
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* IP_setting_butAry[12];
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* IP_setting_1_butAry[5];
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* IP_setting_2_butAry[5];
    Unicode::UnicodeChar Buffer[12][10]; //临时变量
    Unicode::UnicodeChar Buffer1[5][10]; //临时变量
    Unicode::UnicodeChar Buffer2[5][10]; //临时变量
};

#endif // SETTINGVIEW_HPP
