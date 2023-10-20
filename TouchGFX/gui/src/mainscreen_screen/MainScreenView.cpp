#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/Callback.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <gui/setting_screen/SettingView.hpp>
#include <string>
#include <touchgfx/Color.hpp>
#include <stdio.h>


#ifndef SIMULATOR
extern "C" {
#include "time.h"
#include "at24cxx.h"
#include "http_client.h"
#include "station_ctl.h"
#include "CAN_Control.h"
#include "sntp_client.h" 
//    extern error_info_t a[5] = { {0x00,0x00,0x0001},
//{0x00,0x00,0x0001},
//{0x00,0x01,0x0001},
//{0x00,0x02,0x0004},
//{0x00,0x00,0x0001}

//};
		
};

#endif

 struct err_info
  {
      uint8_t error_id_h;
      uint8_t error_id_l;
      uint16_t error_code;
  };
	
	

//struct tm_touchgfx {
//    int tm_sec;         /* 秒，范围从 0 到 59        */
//    int tm_min;         /* 分，范围从 0 到 59        */
//    int tm_hour;        /* 小时，范围从 0 到 23        */
//    int tm_mday;        /* 一月中的第几天，范围从 1 到 31    */
//    int tm_mon;         /* 月，范围从 0 到 11        */
//    int tm_year;        /* 自 1900 年起的年数        */
//    int tm_wday;        /* 一周中的第几天，范围从 0 到 6    */
//    int tm_yday;        /* 一年中的第几天，范围从 0 到 365    */
//    int tm_isdst;       /* 夏令时                */
//};


MainScreenView::MainScreenView()
:BMUMenuCallback(*this)
{
  counter = 0;
  counter2 = 1;

  /*这里还要判断没有接入电池簇和第一簇是哪个*/
  viewToModelData.BCMU_SEL = 1;   //1-20
  viewToModelData.BMU_SEL = 1;    //1-30

  /*先不显示BCMU选择块*/
  BCMU_SEL_BOX.setVisible(false);
  BMU_SEL_BOX.setVisible(false);
  /*setStateChangedCallback() 函数注册回调函数*/
  BMUMenu.setStateChangedCallback(BMUMenuCallback);
}

void BMUMenuCallback_t::execute(const SlideMenu& menu)
{
  touchgfx_printf("BMUMenuCallback\n");
  if(view_.show_batteryshowarea_State())
    view_.show_batteryshowarea_on();
  else
    view_.show_batteryshowarea_off();
}

bool MainScreenView::show_batteryshowarea_State()//电池界面是否显示
{
    if (batteryshowarea.isVisible())
        return true;
    else
        return false;
}

/**
 * @description: 展开BMU菜单
 * @return {*}
 */
void MainScreenView::show_batteryshowarea_on() //电池界面显示
{
    BMU_SEL_BOX.setVisible(true);
    BMU_SEL_BOX.invalidate();

    BCMU_SEL_BOX.setVisible(true);
    BCMU_SEL_BOX.invalidate();
}

/**
 * @description: 收回BMU菜单
 * @return {*}
 */
void MainScreenView::show_batteryshowarea_off() //电池界面不显示
{
    BCMU_SEL_BOX.setVisible(false);
    BCMU_SEL_BOX.invalidate();
    BMU_SEL_BOX.setVisible(false);
    BMU_SEL_BOX.invalidate();

    viewToModelData.BCMU_SEL = 1;   //1-20
    viewToModelData.BMU_SEL = 1;    //1-30
    
 /*   BMU_SEL_BOX.setXY(BMU1.getX()-(BMU_SEL_BOX.getWidth()-BMU1.getWidth())/2, BMU1.getY()-(BMU_SEL_BOX.getHeight()-BMU1.getHeight())/2);
    BMU_BG.invalidate();*/
    //更新选定组编号
    Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
    zu.invalidate();

    presenter->ViewtoModelDat(viewToModelData);
}


void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();
	//通知model更新数据
	viewToModelData.reflashFlag = true;
	presenter->ViewtoModelDat(viewToModelData);


    BCMU[0] = &BCMU1;
    BCMU[1] = &BCMU2;
    BCMU[2] = &BCMU3;
    BCMU[3] = &BCMU4;
    BCMU[4] = &BCMU5;
    BCMU[5] = &BCMU6;
    BCMU[6] = &BCMU7;
    BCMU[7] = &BCMU8;
    BCMU[8] = &BCMU9;
    BCMU[9] = &BCMU10;
    BCMU[10] = &BCMU11;
    BCMU[11] = &BCMU12;
    BCMU[12] = &BCMU13;
    BCMU[13] = &BCMU14;
    BCMU[14] = &BCMU15;
    BCMU[15] = &BCMU16;
    BCMU[16] = &BCMU17;
    BCMU[17] = &BCMU18;
    BCMU[18] = &BCMU19;
    BCMU[19] = &BCMU20;

    BMU[0] = &BMU1;
    BMU[1] = &BMU2;
    BMU[2] = &BMU3;
    BMU[3] = &BMU4;
    BMU[4] = &BMU5;
    BMU[5] = &BMU6;
    BMU[6] = &BMU7;
    BMU[7] = &BMU8;
    BMU[8] = &BMU9;
    BMU[9] = &BMU10;
    BMU[10] = &BMU11;
    BMU[11] = &BMU12;
    BMU[12] = &BMU13;
    BMU[13] = &BMU14;
    BMU[14] = &BMU15;
    BMU[15] = &BMU16;
    BMU[16] = &BMU17;
    BMU[17] = &BMU18;
    BMU[18] = &BMU19;
    BMU[19] = &BMU20;
    BMU[20] = &BMU21;
    BMU[21] = &BMU22;
    BMU[22] = &BMU23;
    BMU[23] = &BMU24;
    BMU[24] = &BMU25;
    BMU[25] = &BMU26;
    BMU[26] = &BMU27;
    BMU[27] = &BMU28;
    BMU[28] = &BMU29;
    BMU[29] = &BMU30;

    tim[0] = &err_time0Buffer[0];
    tim[1] = &err_time1Buffer[0];
    tim[2] = &err_time2Buffer[0];
    tim[3] = &err_time3Buffer[0];
    tim[4] = &err_time4Buffer[0];

    id[0] = &err_id0Buffer[0];
    id[1] = &err_id1Buffer[0];
    id[2] = &err_id2Buffer[0];
    id[3] = &err_id3Buffer[0];
    id[4] = &err_id4Buffer[0];

    inf[0] = &err_inf0;
    inf[1] = &err_inf1;
    inf[2] = &err_inf2;
    inf[3] = &err_inf3;
    inf[4] = &err_inf4;
 
//		err_info  a[]={
//		{0x00,0x01,0b1100000000000000},
//		{0x00,0x00,0x01},
//		{0x00,0x01,0x01},
//		{0x00,0x02,0x01},
//		{0x00,0x01,0x03}
//		};
		
    //uint8_t str[120]= "BMU错误";
    //Unicode::fromUTF8(str, id[0],10);
    ////Unicode::snprintf(id[0], 20, "%s", "BMU错误");  //id
    //inf[0]->setTypedText(touchgfx::TypedText(T_BSMU_ERR0 ));
    //scrollableContainer1.invalidate();
//		int max = 5;
//		int err_counter=0;
////		int BSMU_counter = 0;
////		tm *tm_inf;
////		time_t rawtime;



//  for (int i = 0;i <= max;i++) //遍历所有数组（数组中已存放错误信息，直接按条打印）
//  {
////			rawtime= time (NULL);
////      tm_inf = gmtime(&rawtime);
//      //Unicode::snprintf(tim[i], 20, "%d", i);
//      if (a[i].error_id_h == 0x00) //判断id高8位
//      {
//          if (a[i].error_id_l == 0x00)//判断id低8位
//          {
//            
//          /*    Unicode::UnicodeChar wan_ip_buf[16];
//              Unicode::strncpy(wan_ip_buf, local_ip_buff, 16);
//              Unicode::snprintf(local_ipBuffer, LOCAL_IP_SIZE, "%s", wan_ip_buf);*/

////              Unicode::UnicodeChar id_buf[16];
////              Unicode::strncpy(id_buf, "BSMU错误!", 16);
////              Unicode::snprintf(id[i], 20, "%s", id_buf);  //id

//              // uint16_t error_code get_one_bit_value
//              for (int j = 0;j < 16;j++) //逐位判断错误信息，并输出
//              {
//                  if (get_one_bit_value(a[i].error_code, j + 1) == 1)
//                  {
//                     
//                      inf[err_counter]->setTypedText(touchgfx::TypedText(T_BSMU_ERR0 - j)); 
//											err_counter++;
//                  }
//              }

//          }
//          else if (a[i].error_id_l == 0x01 || a[i].error_id_l == 0x02)//判断id低8位
//          {
//              Unicode::snprintf(id[i], 20, "%s", "BCMU error\0");  //id
//              if (a[i].error_id_l == 0x01) //错误位宽为2
//              {
//                  for (int j = 0;j < 8;j++)
//                  { 
//                      if (get_two_bit_value(a[i].error_code, j  + 1) == 0b00) //无故障
//                      {

//                      }
//                      else if (get_two_bit_value(a[i].error_code, j  + 1) == 0b01)//有故障
//                      {

//                      }
//                      else if (get_two_bit_value(a[i].error_code, j + 1) == 0b10)//预警
//                      {

//                      }
//                      else if (get_two_bit_value(a[i].error_code, j + 1) == 0b11)//预警过，故障发生
//                      {
//                          inf[err_counter]->setTypedText(touchgfx::TypedText(T_BCMU_ERR_2BIT_0 - j));
//						  err_counter++;
//                      }

//                  }
//              }
//              if (a[i].error_id_l == 0x02)//错误位宽为1
//              {
//                  for (int j = 0;j < 16;j++)
//                  {
//                      if (get_one_bit_value(a[i].error_code, j + 1) == 0) //无故障
//                      {

//                      }
//                      else if (get_one_bit_value(a[i].error_code, j + 1) == 1) //无故障
//                      {
//                          inf[err_counter]->setTypedText(touchgfx::TypedText(T_BCMU_ERR0 - j));
//													err_counter++;
//                      }
//                  }
//              }
//          }
//      }
//			else if(a[i].error_id_h >= 0x01 && a[i].error_id_h <= 0x1F) //判断id高8位
//			{
//				if(a[i].error_id_l ==0x00) 
//				{
//					Unicode::snprintf(id[i], 20, "%s", "BMU(组内)板错误!\0");
//					for(int j=0;j<16;j++)
//					{
//							 if (get_one_bit_value(a[i].error_code, j + 1) == 1) 
//							 {
//									inf[err_counter]->setTypedText(touchgfx::TypedText(T_BMU_BOARD_ERR0 - j));
//								  err_counter++;
//							 }
//					}
//				}
//				else if(a[i].error_id_l >=0x01 && a[i].error_id_l <=0x0C)
//				{
//					for(int j=0;j<16;j++)
//					{
//							 if (get_one_bit_value(a[i].error_code, j + 1) == 1) 
//								 {
//										inf[err_counter]->setTypedText(touchgfx::TypedText(T_BMU_BATTERY_ERR0 - j));
//									  err_counter++;
//								 }
//					 }
//				}
//			}
//			else if(a[i].error_id_h == 0x20) //判断id高8位
//			{
//					if(a[i].error_id_l ==0)
//					{
//						Unicode::snprintf(id[i], 20, "%s", "BMU(组内)板错误!\0");
//						for(int j=0;j<16;j++)
//					{
//							 if (get_one_bit_value(a[i].error_code, j + 1) == 1) 
//							 {
//									inf[err_counter]->setTypedText(touchgfx::TypedText(T_BMU_BETWEEN_ERR0 - j));
//								 err_counter++;
//							 }
//					}
//					}
//			}
//       } 
//       scrollableContainer1.invalidate();
//       err_inf.invalidate();
	
    //for (int i = 0;i < 5;i++)
    //{
    //    Unicode::snprintf(tim[i], 10, "%d", i+1);
    //    Unicode::snprintf(id[i], 10, "%d", i+1);
    //    inf[i]->setTypedText(touchgfx::TypedText(T_BMU_BOARD_ERR0 +i));
    //}

    //scrollableContainer1.invalidate();


    //for (int i = 0;i < 30;i++) //显示最近30条错误，格式：打印时间戳 + 错误板号 + 错误类型
    //{
       
  /*  err_num1.setPosition(20, 30, 280, 30);
    err_num1.setColor(touchgfx::Color::getColorFromRGB(1, 255, 255));
    err_num1.setLinespacing(0);
    err_num1.setWildcard(err_num1Buffer);  
    err_num1.setTypedText(touchgfx::TypedText(T_BCMU_ERR3));
    scrollableContainer1.add(err_num1);
           
    Unicode::snprintf(err_num1Buffer, ERR_NUM1_SIZE, "%d", 2);
    err_num1.invalidate();
    scrollableContainer1.invalidate();*/




  
 /*       FrameRateText.setPosition(271, 26, 70, 27);
        FrameRateText.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
        FrameRateText.setLinespacing(0);
        Unicode::snprintf(FrameRateTextBuffer, FRAMERATETEXT_SIZE, "%s", touchgfx::TypedText(T___SINGLEUSE_1G1L).getText());
        FrameRateText.setWildcard(FrameRateTextBuffer);
        FrameRateText.setTypedText(touchgfx::TypedText(T___SINGLEUSE_HULS));
        add(FrameRateText);*/
   /* }*/


  /*  this->bufSize = 4096;
    this->textBuf = (uint8_t*)malloc(this->bufSize);
    if (textBuf != NULL)
    {
        memset(textBuf, 0, this->bufSize);
    }*/


}

void MainScreenView::handleTickEvent()
{

 
    //tickCounter++;
    //if (tickCounter % 60 == 0)
    //{
    //    if (++digitalSeconds >= 60)
    //    {
    //        digitalSeconds = 0;
    //        if (++digitalMinutes >= 60)
    //        {
    //            digitalMinutes = 0;
    //            if (++digitalHours >= 24)
    //            {
    //                digitalHours = 0;
    //            }
    //        }
    //    }
    //}

 /*   tickCounter++;
    uint8_t str[128];
    if (tickCounter % 50 == 0)
    {
        static uint16_t textCount = 0;
        sprintf((char*)str, "你好TouchGFX:count %d\n", textCount++);
        this->TextAreaAddStr(str, sizeof(str));
    }*/
 //   char a[] = {"dsads"};
 //   Unicode::UnicodeChar c[5];
 ///*   char b[4];
 //   for (int i = 0;i < 4;i++)
 //   {
 //       b[i] = a[i];
 //   }*/
 //   Unicode::strncpy(c, a, 5);
 //   Unicode::snprintf(err1Buffer,16,"%s",a);
 //  /* err1.setWideTextAction(WIDE_TEXT_CHARWRAP);*/
 //   err1.invalidate();
#ifndef SIMULATOR
   
#endif
}





void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
}




//按键切换显示内容
void MainScreenView::show_shouye() 
{
    shouye.setVisible(true);
    batteryshowarea.setVisible(false);
    shouye.invalidate();
    batteryshowarea.invalidate();
}

void MainScreenView::show_batteryshowarea()
{
    shouye.setVisible(false);
    batteryshowarea.setVisible(true);
    shouye.invalidate();
    batteryshowarea.invalidate();
}

void MainScreenView::BCMU1_clicked()
{
  viewToModelData.BCMU_SEL = 1;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU1.getX()-(BCMU_SEL_BOX.getWidth()-BCMU1.getWidth())/2, BCMU1.getY()-(BCMU_SEL_BOX.getHeight()-BCMU1.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  //通知model更新数据
  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU2_clicked()
{
  viewToModelData.BCMU_SEL = 2;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU2.getX()-(BCMU_SEL_BOX.getWidth()-BCMU2.getWidth())/2, BCMU2.getY()-(BCMU_SEL_BOX.getHeight()-BCMU2.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();
  
  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU3_clicked()
{
  viewToModelData.BCMU_SEL = 3;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU3.getX()-(BCMU_SEL_BOX.getWidth()-BCMU3.getWidth())/2, BCMU3.getY()-(BCMU_SEL_BOX.getHeight()-BCMU3.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU4_clicked()
{
  viewToModelData.BCMU_SEL = 4;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU4.getX()-(BCMU_SEL_BOX.getWidth()-BCMU4.getWidth())/2, BCMU4.getY()-(BCMU_SEL_BOX.getHeight()-BCMU4.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU5_clicked()
{
  viewToModelData.BCMU_SEL = 5;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU5.getX()-(BCMU_SEL_BOX.getWidth()-BCMU5.getWidth())/2, BCMU5.getY()-(BCMU_SEL_BOX.getHeight()-BCMU5.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU6_clicked()
{
  viewToModelData.BCMU_SEL = 6;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU6.getX()-(BCMU_SEL_BOX.getWidth()-BCMU6.getWidth())/2, BCMU6.getY()-(BCMU_SEL_BOX.getHeight()-BCMU6.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU7_clicked()
{
  viewToModelData.BCMU_SEL = 7;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU7.getX()-(BCMU_SEL_BOX.getWidth()-BCMU7.getWidth())/2, BCMU7.getY()-(BCMU_SEL_BOX.getHeight()-BCMU7.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU8_clicked()
{
  viewToModelData.BCMU_SEL = 8;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU8.getX()-(BCMU_SEL_BOX.getWidth()-BCMU8.getWidth())/2, BCMU8.getY()-(BCMU_SEL_BOX.getHeight()-BCMU8.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU9_clicked()
{
  viewToModelData.BCMU_SEL = 9;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU9.getX()-(BCMU_SEL_BOX.getWidth()-BCMU9.getWidth())/2, BCMU9.getY()-(BCMU_SEL_BOX.getHeight()-BCMU9.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU10_clicked()
{
  viewToModelData.BCMU_SEL = 10;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU10.getX()-(BCMU_SEL_BOX.getWidth()-BCMU10.getWidth())/2, BCMU10.getY()-(BCMU_SEL_BOX.getHeight()-BCMU10.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU11_clicked()
{
  viewToModelData.BCMU_SEL = 11;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU11.getX()-(BCMU_SEL_BOX.getWidth()-BCMU11.getWidth())/2, BCMU11.getY()-(BCMU_SEL_BOX.getHeight()-BCMU11.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU12_clicked()
{
  viewToModelData.BCMU_SEL = 12;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU12.getX()-(BCMU_SEL_BOX.getWidth()-BCMU12.getWidth())/2, BCMU12.getY()-(BCMU_SEL_BOX.getHeight()-BCMU12.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU13_clicked()
{
  viewToModelData.BCMU_SEL = 13;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU13.getX()-(BCMU_SEL_BOX.getWidth()-BCMU13.getWidth())/2, BCMU13.getY()-(BCMU_SEL_BOX.getHeight()-BCMU13.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU14_clicked()
{
  viewToModelData.BCMU_SEL = 14;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU14.getX()-(BCMU_SEL_BOX.getWidth()-BCMU14.getWidth())/2, BCMU14.getY()-(BCMU_SEL_BOX.getHeight()-BCMU14.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU15_clicked()
{
  viewToModelData.BCMU_SEL = 15;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU15.getX()-(BCMU_SEL_BOX.getWidth()-BCMU15.getWidth())/2, BCMU15.getY()-(BCMU_SEL_BOX.getHeight()-BCMU15.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU16_clicked()
{
  viewToModelData.BCMU_SEL = 16;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU16.getX()-(BCMU_SEL_BOX.getWidth()-BCMU16.getWidth())/2, BCMU16.getY()-(BCMU_SEL_BOX.getHeight()-BCMU16.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU17_clicked()
{
  viewToModelData.BCMU_SEL = 17;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU17.getX()-(BCMU_SEL_BOX.getWidth()-BCMU17.getWidth())/2, BCMU17.getY()-(BCMU_SEL_BOX.getHeight()-BCMU17.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU18_clicked()
{
  viewToModelData.BCMU_SEL = 18;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU18.getX()-(BCMU_SEL_BOX.getWidth()-BCMU18.getWidth())/2, BCMU18.getY()-(BCMU_SEL_BOX.getHeight()-BCMU18.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU19_clicked()
{
  viewToModelData.BCMU_SEL = 19;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU19.getX()-(BCMU_SEL_BOX.getWidth()-BCMU19.getWidth())/2, BCMU19.getY()-(BCMU_SEL_BOX.getHeight()-BCMU19.getHeight())/2);
	BCMU_BG.invalidate();	
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BCMU20_clicked()
{
  viewToModelData.BCMU_SEL = 20;
  //更新BCMU选择块位置
  BCMU_SEL_BOX.setVisible(true);
  BCMU_SEL_BOX.setXY(BCMU20.getX()-(BCMU_SEL_BOX.getWidth()-BCMU20.getWidth())/2, BCMU20.getY()-(BCMU_SEL_BOX.getHeight()-BCMU20.getHeight())/2);
	BCMU_BG.invalidate();
  //更新选定簇编号
  Unicode::snprintf(cuBuffer, CU_SIZE, "%d", viewToModelData.BCMU_SEL);
  cu.invalidate();
  /*滑出BMU菜单*/
  //CellStateShow();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU1_clicked()
{
  viewToModelData.BMU_SEL = 1;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU1.getX()-(BMU_SEL_BOX.getWidth()-BMU1.getWidth())/2, BMU1.getY()-(BMU_SEL_BOX.getHeight()-BMU1.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU2_clicked()
{
  viewToModelData.BMU_SEL = 2;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU2.getX()-(BMU_SEL_BOX.getWidth()-BMU2.getWidth())/2, BMU2.getY()-(BMU_SEL_BOX.getHeight()-BMU2.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU3_clicked()
{
  viewToModelData.BMU_SEL = 3;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU3.getX()-(BMU_SEL_BOX.getWidth()-BMU3.getWidth())/2, BMU3.getY()-(BMU_SEL_BOX.getHeight()-BMU3.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU4_clicked()
{
  viewToModelData.BMU_SEL = 4;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU4.getX()-(BMU_SEL_BOX.getWidth()-BMU4.getWidth())/2, BMU4.getY()-(BMU_SEL_BOX.getHeight()-BMU4.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU5_clicked()
{
  viewToModelData.BMU_SEL = 5;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU5.getX()-(BMU_SEL_BOX.getWidth()-BMU5.getWidth())/2, BMU5.getY()-(BMU_SEL_BOX.getHeight()-BMU5.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU6_clicked()
{
  viewToModelData.BMU_SEL = 6;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU6.getX()-(BMU_SEL_BOX.getWidth()-BMU6.getWidth())/2, BMU6.getY()-(BMU_SEL_BOX.getHeight()-BMU6.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU7_clicked()
{
  viewToModelData.BMU_SEL = 7;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU7.getX()-(BMU_SEL_BOX.getWidth()-BMU7.getWidth())/2, BMU7.getY()-(BMU_SEL_BOX.getHeight()-BMU7.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU8_clicked()
{
  viewToModelData.BMU_SEL = 8;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU8.getX()-(BMU_SEL_BOX.getWidth()-BMU8.getWidth())/2, BMU8.getY()-(BMU_SEL_BOX.getHeight()-BMU8.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU9_clicked()
{
  viewToModelData.BMU_SEL = 9;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU9.getX()-(BMU_SEL_BOX.getWidth()-BMU9.getWidth())/2, BMU9.getY()-(BMU_SEL_BOX.getHeight()-BMU9.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU10_clicked()
{
  viewToModelData.BMU_SEL = 10;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU10.getX()-(BMU_SEL_BOX.getWidth()-BMU10.getWidth())/2, BMU10.getY()-(BMU_SEL_BOX.getHeight()-BMU10.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU11_clicked()
{
  viewToModelData.BMU_SEL = 11;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU11.getX()-(BMU_SEL_BOX.getWidth()-BMU11.getWidth())/2, BMU11.getY()-(BMU_SEL_BOX.getHeight()-BMU11.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU12_clicked()
{
  viewToModelData.BMU_SEL = 12;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU12.getX()-(BMU_SEL_BOX.getWidth()-BMU12.getWidth())/2, BMU12.getY()-(BMU_SEL_BOX.getHeight()-BMU12.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU13_clicked()
{
  viewToModelData.BMU_SEL = 13;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU13.getX()-(BMU_SEL_BOX.getWidth()-BMU13.getWidth())/2, BMU13.getY()-(BMU_SEL_BOX.getHeight()-BMU13.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU14_clicked()
{
  viewToModelData.BMU_SEL = 14;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU14.getX()-(BMU_SEL_BOX.getWidth()-BMU14.getWidth())/2, BMU14.getY()-(BMU_SEL_BOX.getHeight()-BMU14.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU15_clicked()
{
  viewToModelData.BMU_SEL = 15;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU15.getX()-(BMU_SEL_BOX.getWidth()-BMU15.getWidth())/2, BMU15.getY()-(BMU_SEL_BOX.getHeight()-BMU15.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU16_clicked()
{
  viewToModelData.BMU_SEL = 16;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU16.getX()-(BMU_SEL_BOX.getWidth()-BMU16.getWidth())/2, BMU16.getY()-(BMU_SEL_BOX.getHeight()-BMU16.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU17_clicked()
{
  viewToModelData.BMU_SEL = 17;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU17.getX()-(BMU_SEL_BOX.getWidth()-BMU17.getWidth())/2, BMU17.getY()-(BMU_SEL_BOX.getHeight()-BMU17.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU18_clicked()
{
  viewToModelData.BMU_SEL = 18;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU18.getX()-(BMU_SEL_BOX.getWidth()-BMU18.getWidth())/2, BMU18.getY()-(BMU_SEL_BOX.getHeight()-BMU18.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU19_clicked()
{
  viewToModelData.BMU_SEL = 19;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU19.getX()-(BMU_SEL_BOX.getWidth()-BMU19.getWidth())/2, BMU19.getY()-(BMU_SEL_BOX.getHeight()-BMU19.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU20_clicked()
{
  viewToModelData.BMU_SEL = 20;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU20.getX()-(BMU_SEL_BOX.getWidth()-BMU20.getWidth())/2, BMU20.getY()-(BMU_SEL_BOX.getHeight()-BMU20.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU21_clicked()
{
  viewToModelData.BMU_SEL = 21;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU21.getX()-(BMU_SEL_BOX.getWidth()-BMU21.getWidth())/2, BMU21.getY()-(BMU_SEL_BOX.getHeight()-BMU21.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU22_clicked()
{
  viewToModelData.BMU_SEL = 22;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU22.getX()-(BMU_SEL_BOX.getWidth()-BMU22.getWidth())/2, BMU22.getY()-(BMU_SEL_BOX.getHeight()-BMU22.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU23_clicked()
{
  viewToModelData.BMU_SEL = 23;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU23.getX()-(BMU_SEL_BOX.getWidth()-BMU23.getWidth())/2, BMU23.getY()-(BMU_SEL_BOX.getHeight()-BMU23.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU24_clicked()
{
  viewToModelData.BMU_SEL = 24;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU24.getX()-(BMU_SEL_BOX.getWidth()-BMU24.getWidth())/2, BMU24.getY()-(BMU_SEL_BOX.getHeight()-BMU24.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU25_clicked()
{
  viewToModelData.BMU_SEL = 25;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU25.getX()-(BMU_SEL_BOX.getWidth()-BMU25.getWidth())/2, BMU25.getY()-(BMU_SEL_BOX.getHeight()-BMU25.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU26_clicked()
{
  viewToModelData.BMU_SEL = 26;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU26.getX()-(BMU_SEL_BOX.getWidth()-BMU26.getWidth())/2, BMU26.getY()-(BMU_SEL_BOX.getHeight()-BMU26.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU27_clicked()
{
  viewToModelData.BMU_SEL = 27;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU27.getX()-(BMU_SEL_BOX.getWidth()-BMU27.getWidth())/2, BMU27.getY()-(BMU_SEL_BOX.getHeight()-BMU27.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU28_clicked()
{
  viewToModelData.BMU_SEL = 28;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU28.getX()-(BMU_SEL_BOX.getWidth()-BMU28.getWidth())/2, BMU28.getY()-(BMU_SEL_BOX.getHeight()-BMU28.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU29_clicked()
{
  viewToModelData.BMU_SEL = 29;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU29.getX()-(BMU_SEL_BOX.getWidth()-BMU29.getWidth())/2, BMU29.getY()-(BMU_SEL_BOX.getHeight()-BMU29.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::BMU30_clicked()
{
  viewToModelData.BMU_SEL = 30;
  //更新BCMU选择块位置
  
  BMU_SEL_BOX.setXY(BMU30.getX()-(BMU_SEL_BOX.getWidth()-BMU30.getWidth())/2, BMU30.getY()-(BMU_SEL_BOX.getHeight()-BMU30.getHeight())/2);
	BMU_BG.invalidate();
  //更新选定组编号
  Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
  zu.invalidate();

  viewToModelData.reflashFlag = true;
  presenter->ViewtoModelDat(viewToModelData);
}

 uint8_t MainScreenView::get_one_bit_value(uint16_t src, uint8_t bit_num) //bit_num 1~16
{
    return (uint8_t)((src >> (bit_num - 1)) & 1);
}

 uint8_t MainScreenView::get_two_bit_value(uint16_t src, uint8_t bit_num)//bit num 1~8
 {

     return (uint8_t)((src >> (bit_num*2 - 2)) & 3);
 }

#ifndef SIMULATOR
//model更改通知UI
void MainScreenView::NotifyViewMsg(ModelToViewData modelToViewData)
{
  //更新LCD帧率
  Unicode::snprintf(FrameRateTextBuffer, FRAMERATETEXT_SIZE, "%d", modelToViewData.frameRate);
  FrameRateText.invalidate();

  //更新CELLSHOW中提示
	Unicode::snprintf(dianchixinxiBuffer1, DIANCHIXINXIBUFFER1_SIZE, "%d", viewToModelData.BCMU_SEL);
    Unicode::snprintf(dianchixinxiBuffer2, DIANCHIXINXIBUFFER2_SIZE, "%d", viewToModelData.BMU_SEL);
	dianchixinxi.invalidate();
  
	//更新所有单体电池电压(原/10000  现在/1000)
	Unicode::snprintfFloat(CellText1Buffer, CELLTEXT1_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[0]/1000);
	CellText1.invalidate();
	
	Unicode::snprintfFloat(CellText2Buffer, CELLTEXT2_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[1]/1000);
	CellText2.invalidate();
	
	Unicode::snprintfFloat(CellText3Buffer, CELLTEXT3_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[2]/1000);
	CellText3.invalidate();

	Unicode::snprintfFloat(CellText4Buffer, CELLTEXT4_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[3]/1000);
	CellText4.invalidate();

	Unicode::snprintfFloat(CellText5Buffer, CELLTEXT5_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[4]/1000);
	CellText5.invalidate();

	Unicode::snprintfFloat(CellText6Buffer, CELLTEXT6_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[5]/1000);
	CellText6.invalidate();

	Unicode::snprintfFloat(CellText7Buffer, CELLTEXT7_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[6]/1000);
	CellText7.invalidate();

	Unicode::snprintfFloat(CellText8Buffer, CELLTEXT8_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[7]/1000);
	CellText8.invalidate();

	Unicode::snprintfFloat(CellText9Buffer, CELLTEXT9_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[8]/1000);
	CellText9.invalidate();

	Unicode::snprintfFloat(CellText10Buffer, CELLTEXT10_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[9]/1000);
	CellText10.invalidate();

	Unicode::snprintfFloat(CellText11Buffer, CELLTEXT11_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[10]/1000);
	CellText11.invalidate();

	Unicode::snprintfFloat(CellText12Buffer, CELLTEXT12_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[11]/1000);
	CellText12.invalidate();
    //显示平均电压（调试用）
    float sum_v = 0;
    for (int i = 0;i < 12;i++)
    {          
        sum_v += (float)modelToViewData.BAT_VOL[i] / 1000;
    }
    float avg_V = sum_v/12;
    Unicode::snprintfFloat(avg_vBuffer, AVG_V_SIZE, "%.3f", avg_V);
    avg_v.invalidate();

  //更新电站信息
  Unicode::snprintf(zongdianliuBuffer, ZONGDIANLIU_SIZE, "%d", modelToViewData.station_CUR);
  zongdianliu.invalidate();

  Unicode::snprintf(zongdianyaBuffer, ZONGDIANYA_SIZE, "%d", modelToViewData.station_VOL);
  zongdianya.invalidate();

  Unicode::snprintf(chongdiangonglvBuffer, CHONGDIANGONGLV_SIZE, "%d", modelToViewData.charge_power);
  chongdiangonglv.invalidate();

  Unicode::snprintf(fangdiangonglvBuffer, FANGDIANGONGLV_SIZE, "%d", modelToViewData.discharge_power);
  fangdiangonglv.invalidate();

  //更新单簇实时数据
  Unicode::snprintf(cubianhaoBuffer, CUBIANHAO_SIZE, "%d", viewToModelData.BCMU_SEL);
  cubianhao.invalidate();

  Unicode::snprintfFloat(cudianyaBuffer, CUDIANYA_SIZE, "%.1f", (float)modelToViewData.cluster_VOL*0.1);
  cudianya.invalidate();

  Unicode::snprintfFloat(cudianliuBuffer, CUDIANLIU_SIZE, "%.1f", (float)modelToViewData.cluster_CUR*0.01);
  cudianliu.invalidate();

  Unicode::snprintf(zhengjueyuanBuffer, ZHENGJUEYUAN_SIZE, "%d",  modelToViewData.insulation_res_n);
  zhengjueyuan.invalidate();

  Unicode::snprintf(fujueyuanBuffer, FUJUEYUAN_SIZE, "%d",  modelToViewData.insulation_res_p);
  fujueyuan.invalidate();


	//更新电池指示图
  SOC1_view.setValue((float)modelToViewData.BAT_SOC[0] / 10);
  SOC2_view.setValue((float)modelToViewData.BAT_SOC[1] / 10);
  SOC3_view.setValue((float)modelToViewData.BAT_SOC[2] / 10);
  SOC4_view.setValue((float)modelToViewData.BAT_SOC[3] / 10);
  SOC5_view.setValue((float)modelToViewData.BAT_SOC[4] / 10);
  SOC6_view.setValue((float)modelToViewData.BAT_SOC[5] / 10);
  SOC7_view.setValue((float)modelToViewData.BAT_SOC[6] / 10);
  SOC8_view.setValue((float)modelToViewData.BAT_SOC[7] / 10);
  SOC9_view.setValue((float)modelToViewData.BAT_SOC[8] / 10);
  SOC10_view.setValue((float)modelToViewData.BAT_SOC[9] / 10);
  SOC11_view.setValue((float)modelToViewData.BAT_SOC[10] / 10);
  SOC12_view.setValue((float)modelToViewData.BAT_SOC[11] / 10);

	//更新所有单体电池SOC
  Unicode::snprintfFloat(CellSOCText1Buffer, CELLSOCTEXT1_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[0]/10);
	CellSOCText1.invalidate();

  Unicode::snprintfFloat(CellSOCText2Buffer, CELLSOCTEXT2_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[1]/10);
	CellSOCText2.invalidate();

  Unicode::snprintfFloat(CellSOCText3Buffer, CELLSOCTEXT3_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[2]/10);
	CellSOCText3.invalidate();

  Unicode::snprintfFloat(CellSOCText4Buffer, CELLSOCTEXT4_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[3]/10);
	CellSOCText4.invalidate();

  Unicode::snprintfFloat(CellSOCText5Buffer, CELLSOCTEXT5_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[4]/10);
	CellSOCText5.invalidate();

  Unicode::snprintfFloat(CellSOCText6Buffer, CELLSOCTEXT6_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[5]/10);
	CellSOCText6.invalidate();

  Unicode::snprintfFloat(CellSOCText7Buffer, CELLSOCTEXT7_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[6]/10);
	CellSOCText7.invalidate();

  Unicode::snprintfFloat(CellSOCText8Buffer, CELLSOCTEXT8_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[7]/10);
	CellSOCText8.invalidate();

  Unicode::snprintfFloat(CellSOCText9Buffer, CELLSOCTEXT9_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[8]/10);
	CellSOCText9.invalidate();

  Unicode::snprintfFloat(CellSOCText10Buffer, CELLSOCTEXT10_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[9]/10);
	CellSOCText10.invalidate();

  Unicode::snprintfFloat(CellSOCText11Buffer, CELLSOCTEXT11_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[10]/10);
	CellSOCText11.invalidate();

  Unicode::snprintfFloat(CellSOCText12Buffer, CELLSOCTEXT12_SIZE, "%.1f", (float)modelToViewData.BAT_SOC[11]/10);
	CellSOCText12.invalidate();

    float sum_soc = 0;
    for (int i = 0;i < 12;i++)
    {       
        sum_soc += modelToViewData.BAT_SOC[i]/10;
    }
    float avg_S = sum_soc / 12.0;
    Unicode::snprintfFloat(avg_socBuffer, AVG_SOC_SIZE, "%.1f", avg_S);
    avg_soc.invalidate();

	//更新电池温度
  Unicode::snprintfFloat(CellTempText1Buffer, CELLTEMPTEXT1_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText1.invalidate();

  Unicode::snprintfFloat(CellTempText2Buffer, CELLTEMPTEXT2_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[1]/100);
	CellTempText2.invalidate();

  Unicode::snprintfFloat(CellTempText3Buffer, CELLTEMPTEXT3_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[2]/100);
	CellTempText3.invalidate();

  Unicode::snprintfFloat(CellTempText4Buffer, CELLTEMPTEXT4_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[3]/100);
	CellTempText4.invalidate();

  Unicode::snprintfFloat(CellTempText5Buffer, CELLTEMPTEXT5_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText5.invalidate();

  Unicode::snprintfFloat(CellTempText6Buffer, CELLTEMPTEXT6_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText6.invalidate();

  Unicode::snprintfFloat(CellTempText7Buffer, CELLTEMPTEXT7_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText7.invalidate();

  Unicode::snprintfFloat(CellTempText8Buffer, CELLTEMPTEXT8_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText8.invalidate();

  Unicode::snprintfFloat(CellTempText9Buffer, CELLTEMPTEXT9_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText9.invalidate();

  Unicode::snprintfFloat(CellTempText10Buffer, CELLTEMPTEXT10_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText10.invalidate();

  Unicode::snprintfFloat(CellTempText11Buffer, CELLTEMPTEXT11_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText11.invalidate();

  Unicode::snprintfFloat(CellTempText12Buffer, CELLTEMPTEXT12_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText12.invalidate();

    float sum_tem = 0;
    for (int i = 0;i < 12;i++)
    {       
        sum_tem += modelToViewData.BAT_TMP[i]/100;
    }
    float avg_T= sum_tem/12.0;
    Unicode::snprintfFloat(avg_temBuffer, AVG_TEM_SIZE, "%.2f", avg_T);
    avg_tem.invalidate();

  //更新BCMU框选图标
  if(BCMU_SEL_BOX.isVisible() == false)
  {
    if(modelToViewData.BCMU_state[0] == online)
      BCMU1_clicked();
    else if(modelToViewData.BCMU_state[1] == online)
      BCMU2_clicked();
    else if(modelToViewData.BCMU_state[2] == online)
      BCMU3_clicked();
    else if(modelToViewData.BCMU_state[3] == online)
      BCMU4_clicked();
    else if(modelToViewData.BCMU_state[4] == online)
      BCMU5_clicked();
    else if(modelToViewData.BCMU_state[5] == online)
      BCMU6_clicked();
    else if(modelToViewData.BCMU_state[6] == online)
      BCMU7_clicked();
    else if(modelToViewData.BCMU_state[7] == online)
      BCMU8_clicked();
    else if(modelToViewData.BCMU_state[8] == online)
      BCMU9_clicked();
    else if(modelToViewData.BCMU_state[9] == online)
      BCMU10_clicked();
    else if(modelToViewData.BCMU_state[10] == online)
      BCMU11_clicked();
    else if(modelToViewData.BCMU_state[11] == online)
      BCMU12_clicked();
    else if(modelToViewData.BCMU_state[12] == online)
      BCMU13_clicked();
    else if(modelToViewData.BCMU_state[13] == online)
      BCMU14_clicked();
    else if(modelToViewData.BCMU_state[14] == online)
      BCMU15_clicked();
    else if(modelToViewData.BCMU_state[15] == online)
      BCMU16_clicked();
    else if(modelToViewData.BCMU_state[16] == online)
      BCMU17_clicked();
    else if(modelToViewData.BCMU_state[17] == online)
      BCMU18_clicked();
    else if(modelToViewData.BCMU_state[18] == online)
      BCMU19_clicked();
    else if(modelToViewData.BCMU_state[19] == online)
      BCMU20_clicked();
  }

  for (uint8_t i = 0; i < 20; i++)
  {
    if(modelToViewData.BCMU_state[i] == offline && BCMU_SEL_BOX.isVisible() == true && viewToModelData.BCMU_SEL == i+1)
    {
      BCMU_SEL_BOX.setVisible(false);
      BCMU_SEL_BOX.invalidate();
      break;
    }
  }


  for (uint8_t i = 0;i < 20;i++)
  {
      if (modelToViewData.BCMU_state[i] == offline && BCMU[i]->isTouchable() == true)
      {     
          //不使能
          BCMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BCMU[i]->setTouchable(false);
          for (uint8_t j = 0;j < 30;j++)//当前簇不使能，组默认全不使能
          {
              BMU[j]->setLabelText(touchgfx::TypedText(T_BCMU_0));
              BMU[j]->setTouchable(false);
          } 
          /*BCMU_BG.invalidate();*/
          BCMU_Container.invalidate();
          BMU_Container.invalidate();
      }
      else if  (modelToViewData.BCMU_state[i] != offline && BCMU[i]->isTouchable() == false) 
      {            
          //使能BCMU按键
          BCMU[i]->setLabelText(touchgfx::TypedText(T_BCMU1 -i));
          BCMU[i]->setTouchable(true);
          for (uint8_t j = 0;j < 30;j++)
          {
              if (bmu_offline[i][j] == 0) //0表示在线，非0离线
              {
                  BMU[j]->setLabelText(touchgfx::TypedText(T_BMU1 +j));
                  BMU[j]->setTouchable(true);
              }
              else
              {
                  BMU[j]->setLabelText(touchgfx::TypedText(T_BCMU_0));
                  BMU[j]->setTouchable(false);
              }
          }
          BCMU_Container.invalidate();
          BMU_Container.invalidate();
      }
  }

  //更新ip信息

  http_get_wan_ip(local_ip_buff, 16);//给local_ip_buff赋值char类型的变量
  Unicode::UnicodeChar wan_ip_buf[16];
  Unicode::strncpy(wan_ip_buf, local_ip_buff, 16);
  Unicode::snprintf(local_ipBuffer, LOCAL_IP_SIZE, "%s", wan_ip_buf);
  local_ip.invalidate();

  http_get_cloud_ip(cloud_server_ip_buff, 16);//云端服务器IP
  Unicode::UnicodeChar cloud_ip_buf[16];
  Unicode::strncpy(cloud_ip_buf, cloud_server_ipBuffer, 16);
  Unicode::snprintf(cloud_server_ipBuffer, CLOUD_SERVER_IP_SIZE, "%s", cloud_ip_buf);
  cloud_server_ip.invalidate();

  Unicode::snprintf(local_server_ipBuffer, LOCAL_SERVER_IP_SIZE, "%d", bsmuSetting.IP_ADD_1[0]);//本地服务器IP
  Unicode::snprintf(local_server_ip2Buffer, LOCAL_SERVER_IP2_SIZE, "%d", bsmuSetting.IP_ADD_1[1]);
  Unicode::snprintf(local_server_ip3Buffer, LOCAL_SERVER_IP3_SIZE, "%d", bsmuSetting.IP_ADD_1[2]);
  Unicode::snprintf(local_server_ip4Buffer, LOCAL_SERVER_IP4_SIZE, "%d", bsmuSetting.IP_ADD_1[3]);
  local_server_ip.invalidate();
  local_server_ip2.invalidate();
  local_server_ip3.invalidate();
  local_server_ip4.invalidate();



  //更新错误信息
 /* typedef struct
  {
      uint8_t error_id_h;
      uint8_t error_id_l;
      uint16_t error_code;
  } error_info_t;  

  error_info_t Client_errors[cluster_num][MAX_ERROR];*/

 /* for (int i = 0;i < 5;i++)
  {
      Unicode::snprintf(tim[i], 10, "%d", i + 1);
      Unicode::snprintf(id[i], 10, "%d", i + 1);
      inf[i]->setTypedText(touchgfx::TypedText(T_BMU_BOARD_ERR0 + i));
  }

  scrollableContainer1.invalidate();*/
  //遍历错误数组
  





  //if(modelToViewData.BCMU_state[0] == offline && BCMU1.isTouchable()==true){
  //  //BCMU离线的情况，不使能BCMU按键
  //  BCMU1.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU1.setTouchable(false);
  //  BCMU_BG.invalidate();
  //}
  //else if(modelToViewData.BCMU_state[0] != offline && BCMU1.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU1.setLabelText(touchgfx::TypedText(T_BCMU1));
  //  BCMU1.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[1] == offline && BCMU2.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU2.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU2.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[1] != offline && BCMU2.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU2.setLabelText(touchgfx::TypedText(T_BCMU2));
  //  BCMU2.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[2] == offline && BCMU3.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU3.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU3.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[2] != offline && BCMU3.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU3.setLabelText(touchgfx::TypedText(T_BCMU3));
  //  BCMU3.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[3] == offline && BCMU4.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU4.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU4.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[3] != offline && BCMU4.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU4.setLabelText(touchgfx::TypedText(T_BCMU4));
  //  BCMU4.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[4] == offline && BCMU5.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU5.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU5.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[4] != offline && BCMU5.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU5.setLabelText(touchgfx::TypedText(T_BCMU5));
  //  BCMU5.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[5] == offline && BCMU6.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU6.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU6.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[5] != offline && BCMU6.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU6.setLabelText(touchgfx::TypedText(T_BCMU6));
  //  BCMU6.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[6] == offline && BCMU7.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU7.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU7.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[6] != offline && BCMU7.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU7.setLabelText(touchgfx::TypedText(T_BCMU7));
  //  BCMU7.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[7] == offline && BCMU8.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU8.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU8.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[7] != offline && BCMU8.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU8.setLabelText(touchgfx::TypedText(T_BCMU8));
  //  BCMU8.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[8] == offline && BCMU9.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU9.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU9.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[8] != offline && BCMU9.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU9.setLabelText(touchgfx::TypedText(T_BCMU9));
  //  BCMU9.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[9] == offline && BCMU10.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU10.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU10.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[9] != offline && BCMU10.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU10.setLabelText(touchgfx::TypedText(T_BCMU10));
  //  BCMU10.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[10] == offline && BCMU11.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU11.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU11.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[10] != offline && BCMU11.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU11.setLabelText(touchgfx::TypedText(T_BCMU11));
  //  BCMU11.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[11] == offline && BCMU12.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU12.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU12.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[11] != offline && BCMU12.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU12.setLabelText(touchgfx::TypedText(T_BCMU12));
  //  BCMU12.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[12] == offline && BCMU13.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU13.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU13.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[12] != offline && BCMU13.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU13.setLabelText(touchgfx::TypedText(T_BCMU13));
  //  BCMU13.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[13] == offline && BCMU14.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU14.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU14.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[13] != offline && BCMU14.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU14.setLabelText(touchgfx::TypedText(T_BCMU14));
  //  BCMU14.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[14] == offline && BCMU15.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU15.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU15.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[14] != offline && BCMU15.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU15.setLabelText(touchgfx::TypedText(T_BCMU15));
  //  BCMU15.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[15] == offline && BCMU16.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU16.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU16.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[15] != offline && BCMU16.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU16.setLabelText(touchgfx::TypedText(T_BCMU16));
  //  BCMU16.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[16] == offline && BCMU17.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU17.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU17.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[16] != offline && BCMU17.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU17.setLabelText(touchgfx::TypedText(T_BCMU17));
  //  BCMU17.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[17] == offline && BCMU18.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU18.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU18.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[17] != offline && BCMU18.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU18.setLabelText(touchgfx::TypedText(T_BCMU18));
  //  BCMU18.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[18] == offline && BCMU19.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU19.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU19.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[18] != offline && BCMU19.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU19.setLabelText(touchgfx::TypedText(T_BCMU19));
  //  BCMU19.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  //if(modelToViewData.BCMU_state[19] == offline && BCMU20.isTouchable()==true){
  //  //不使能BCMU按键
  //  BCMU20.setLabelText(touchgfx::TypedText(T_BCMU_0));
  //  BCMU20.setTouchable(false);
  //  BCMU_BG.invalidate();	
  //}
  //else if(modelToViewData.BCMU_state[19] != offline && BCMU20.isTouchable()==false){
  //  //使能BCMU按键
  //  BCMU20.setLabelText(touchgfx::TypedText(T_BCMU20));
  //  BCMU20.setTouchable(true);
  //  BCMU_BG.invalidate();	
  //}

  

}


#endif


