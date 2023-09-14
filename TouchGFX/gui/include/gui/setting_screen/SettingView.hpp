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
	uint16_t poll_T;      //这里的单位是10ms,使用的时候记得乘以10
	uint8_t IP_ADD[4];	 //本机网络配置
	uint8_t GATEWAY[4];	
	uint8_t NETMASK[4];	
	uint8_t IP_ADD_1[4];	 //本地服务器
	uint16_t port;
    uint8_t IP_ADD_2[4];	 //4G
	uint16_t port_1;
	uint8_t canBps;			//CAN波特率
	uint8_t RS485Bps;			//485波特率
	uint8_t checksum;
    uint8_t Scroll1;
    uint8_t local_flag;     //开、关本地网络
    uint8_t yunduan_flag;   //开、关云端服务器
    //.c的头文件修改相应
}EEPROM_BSMU_tem;


class SettingView : public SettingViewBase
{
public:
    SettingView();
    virtual ~SettingView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void canbutton_clicked();
    virtual void RS485button_clicked();

    virtual void SettingViewTick();

    //virtual void SetCuNum(int value);
    //virtual void SetPoll_T(int value);

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

   /* virtual void IP_ADD_1_fun();
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
    virtual void NETMASK_4_fun();*/

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

    virtual void SetCan1(int value);
    virtual void SetCan2(int value);
    virtual void SetRs485(int value);

    void SetButState(WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >** p, uint8_t num, bool state);


    virtual void setcanbaudrate125_125();//设置波特率，并存储到EEPROM中
    virtual void setcanbaudrate250_250();
    virtual void setcanbaudrate500_500();
    virtual void setcanbaudrate500_1000();
    virtual void setcanbaudrate500_2000();

    virtual void setRS485baudrate_4800();
    virtual void setRS485baudrate_9600();
    virtual void setRS485baudrate_19200();
    virtual void setRS485baudrate_38400();
    virtual void setRS485baudrate_57600();
    virtual void setRS485baudrate_115200();

    virtual void poll_t_set();
    virtual void zhuangji_set();

    virtual void zu_set_save();
    virtual void zu_set_close();


    virtual void t_set_save();
    virtual void t_set_close();
    virtual void t_setbutton_fun();

    virtual void localnet_state(); //toggle组件开关，对应本地网络连接开关
    virtual void yunduannet_state();//toggle组件开关，对应云端网络连接开关

    virtual void local_ip_set();
    virtual void local_port_set();
    virtual void ip_4g_set();
    virtual void port_4g_set();



protected:
    EEPROM_BSMU_tem SettingBuff;
    EEPROM_BSMU_tem SettingBuff_temp;//该临时结构体用于临时保存数据，根据保存或取消指令来判断是否对数据进行保存
    //按键数组，存储每个弹窗里所有的按键数量
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* IP_setting_butAry[12];
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* IP_setting_1_butAry[5];
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* IP_setting_2_butAry[5];
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* zu_set_butAry[1]; 
    WildcardTextButtonStyle< touchgfx::BoxWithBorderButtonStyle< touchgfx::ToggleButtonTrigger >  >* t_set_butAry[1];

    Unicode::UnicodeChar Buffer[12][10]; //临时变量
    Unicode::UnicodeChar Buffer1[5][10]; //临时变量
    Unicode::UnicodeChar Buffer2[5][10]; //临时变量

    Unicode::UnicodeChar zu_Buffer[1][10]; //临时变量
    Unicode::UnicodeChar t_Buffer[1][10]; //临时变量


	// Callback which is executed when a item in the scroll whell is selected to as selected style.
    // The parameter itemSelected is the selected item.
 
	//Callback<SettingView, int16_t> RS485_kbpsAnimateToCallback;
    //void RS485_kbpsAnimateToHandler(int16_t itemSelected);

    //Callback<SettingView, const ToggleButton&, const ClickEvent&> networkSwitchClickedCallback;

 



  
};

#endif // SETTINGVIEW_HPP
