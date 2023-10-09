#ifndef MAINSCREENVIEW_HPP
#define MAINSCREENVIEW_HPP

#include <gui_generated/mainscreen_screen/MainScreenViewBase.hpp>
#include <gui/mainscreen_screen/MainScreenPresenter.hpp>
#include <gui/common/GuiTransTypes.hpp>
#include <gui_generated/setting_screen/SettingViewBase.hpp>
#include <gui/setting_screen/SettingView.hpp>

//BCMU按键相关参数
#define online 1
#define offline 0

class BMUMenuCallback_t : public GenericCallback<const SlideMenu&>
{
public:
  BMUMenuCallback_t(MainScreenView& view) : view_(view) {}

  virtual void execute(const SlideMenu& menu);

  virtual bool isValid() const
  {
      return true;
  }

private:
  MainScreenView& view_;
 
};
//class local_ip_data :public SettingView {
//public:
//        static int a[] = { 0 };
//        a[0] = SettingBuff.IP_ADD_1[0];
//        a[1] = SettingBuff.IP_ADD_1[1];
//        a[2] = SettingBuff.IP_ADD_1[2];
//        a[3] = SettingBuff.IP_ADD_1[3];
//};

class MainScreenView : public MainScreenViewBase
{
public:
    MainScreenView();
    virtual ~MainScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void BCMU1_clicked();
    virtual void BCMU2_clicked();
    virtual void BCMU3_clicked();
    virtual void BCMU4_clicked();
    virtual void BCMU5_clicked();
    virtual void BCMU6_clicked();
    virtual void BCMU7_clicked();
    virtual void BCMU8_clicked();
    virtual void BCMU9_clicked();
    virtual void BCMU10_clicked();
    virtual void BCMU11_clicked();
    virtual void BCMU12_clicked();
    virtual void BCMU13_clicked();
    virtual void BCMU14_clicked();
    virtual void BCMU15_clicked();
    virtual void BCMU16_clicked();
    virtual void BCMU17_clicked();
    virtual void BCMU18_clicked();
    virtual void BCMU19_clicked();
    virtual void BCMU20_clicked();

    virtual void BMU1_clicked();
    virtual void BMU2_clicked();
    virtual void BMU3_clicked();
    virtual void BMU4_clicked();
    virtual void BMU5_clicked();
    virtual void BMU6_clicked();
    virtual void BMU7_clicked();
    virtual void BMU8_clicked();
    virtual void BMU9_clicked();
    virtual void BMU10_clicked();
    virtual void BMU11_clicked();
    virtual void BMU12_clicked();
    virtual void BMU13_clicked();
    virtual void BMU14_clicked();
    virtual void BMU15_clicked();
    virtual void BMU16_clicked();
    virtual void BMU17_clicked();
    virtual void BMU18_clicked();
    virtual void BMU19_clicked();
    virtual void BMU20_clicked();
    virtual void BMU21_clicked();
    virtual void BMU22_clicked();
    virtual void BMU23_clicked();
    virtual void BMU24_clicked();
    virtual void BMU25_clicked();
    virtual void BMU26_clicked();
    virtual void BMU27_clicked();
    virtual void BMU28_clicked();
    virtual void BMU29_clicked();
    virtual void BMU30_clicked();

    virtual void show_shouye();
    virtual void show_batteryshowarea();

    bool show_batteryshowarea_State();
    virtual void show_batteryshowarea_on();
    virtual void show_batteryshowarea_off();

#ifndef SIMULATOR
//model更改
    void NotifyViewMsg(ModelToViewData modelToViewData);
#endif

protected:
    ViewToModelData viewToModelData;
	int counter;
    int counter2;
    BMUMenuCallback_t BMUMenuCallback;

    char local_ip_buff[17];
    char local_server_ip_buff[16];
    char cloud_server_ip_buff[16];

    ButtonWithLabel* BCMU[20];
    ButtonWithLabel* BMU[30];
    //Unicode::UnicodeChar chinese_buf[10];
 /*   SettingView& dskjsk;*/
};

#endif // MAINSCREENVIEW_HPP

