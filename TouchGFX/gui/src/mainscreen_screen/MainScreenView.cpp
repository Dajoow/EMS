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
extern BCMU_Mail_t BCMU[cluster_num];
extern Client_Sd_t Client_Sd[cluster_num];	
extern error_info_t Client_errors[cluster_num][MAX_ERROR];			
};

#endif


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
  err_time.setHeight(errcount*25+25); //设置错误信息打印的显示区域
  err_id.setHeight(errcount*25+25);
  err_inf.setHeight(errcount * 25+25);

	BCMU_ui[0] = &BCMU1; //用指针数组指向组件，可以在代码中使用循环进行遍历操作
    BCMU_ui[1] = &BCMU2;//BCMU按键
    BCMU_ui[2] = &BCMU3;
    BCMU_ui[3] = &BCMU4;
    BCMU_ui[4] = &BCMU5;
    BCMU_ui[5] = &BCMU6;
    BCMU_ui[6] = &BCMU7;
    BCMU_ui[7] = &BCMU8;
    BCMU_ui[8] = &BCMU9;
    BCMU_ui[9] = &BCMU10;
    BCMU_ui[10] = &BCMU11;
    BCMU_ui[11] = &BCMU12;
    BCMU_ui[12] = &BCMU13;
    BCMU_ui[13] = &BCMU14;
    BCMU_ui[14] = &BCMU15;
    BCMU_ui[15] = &BCMU16;
    BCMU_ui[16] = &BCMU17;
    BCMU_ui[17] = &BCMU18;
    BCMU_ui[18] = &BCMU19;
    BCMU_ui[19] = &BCMU20;

    BMU[0] = &BMU1;//BMU按键
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

    SOC_view[0] = &SOC1_view;//SOC进度条
    SOC_view[1] = &SOC2_view;
    SOC_view[2] = &SOC3_view;
    SOC_view[3] = &SOC4_view;
    SOC_view[4] = &SOC5_view;
    SOC_view[5] = &SOC6_view;
    SOC_view[6] = &SOC7_view;
    SOC_view[7] = &SOC8_view;
    SOC_view[8] = &SOC9_view;
    SOC_view[9] = &SOC10_view;
    SOC_view[10] = &SOC11_view;
    SOC_view[11] = &SOC12_view;

    tim[0] = &err_time0Buffer[0];//错误信息时间戳，在第一列
    tim[1] = &err_time1Buffer[0];
    tim[2] = &err_time2Buffer[0];
    tim[3] = &err_time3Buffer[0];
    tim[4] = &err_time4Buffer[0];
    tim[5] = &err_time5Buffer[0];
    tim[6] = &err_time6Buffer[0];
    tim[7] = &err_time7Buffer[0];
    tim[8] = &err_time8Buffer[0];
    tim[9] = &err_time9Buffer[0];
    tim[10] = &err_time10Buffer[0];
    tim[11] = &err_time11Buffer[0];
    tim[12] = &err_time12Buffer[0];
    tim[13] = &err_time13Buffer[0];
    tim[14] = &err_time14Buffer[0];
    tim[15] = &err_time15Buffer[0];
    tim[16] = &err_time16Buffer[0];
    tim[17] = &err_time17Buffer[0];
    tim[18] = &err_time18Buffer[0];
    tim[19] = &err_time19Buffer[0];


    id[0] = &err_id0Buffer[0];//错误信息id，之前测试用，现在没用到
    id[1] = &err_id1Buffer[0];
    id[2] = &err_id2Buffer[0];
    id[3] = &err_id3Buffer[0];
    id[4] = &err_id4Buffer[0];
    id[5] = &err_id5Buffer[0];
    id[6] = &err_id6Buffer[0];
    id[7] = &err_id7Buffer[0];
    id[8] = &err_id8Buffer[0];
    id[9] = &err_id9Buffer[0];
    id[10] = &err_id10Buffer[0];
    id[11] = &err_id11Buffer[0];
    id[12] = &err_id12Buffer[0];
    id[13] = &err_id13Buffer[0];
    id[14] = &err_id14Buffer[0];
    id[15] = &err_id15Buffer[0];
    id[16] = &err_id16Buffer[0];
    id[17] = &err_id17Buffer[0];
    id[18] = &err_id18Buffer[0];
    id[19] = &err_id19Buffer[0];

    id_wild[0] = &err_id0;//错误信息id，现在使用wild动态通配符显示自定义类型，在第二列
    id_wild[1] = &err_id1;
    id_wild[2] = &err_id2;
    id_wild[3] = &err_id3;
    id_wild[4] = &err_id4;
    id_wild[5] = &err_id5;
    id_wild[6] = &err_id6;
    id_wild[7] = &err_id7;
    id_wild[8] = &err_id8;
    id_wild[9] = &err_id9;
    id_wild[10] = &err_id10;
    id_wild[11] = &err_id11;
    id_wild[12] = &err_id12;
    id_wild[13] = &err_id13;
    id_wild[14] = &err_id14;
    id_wild[15] = &err_id15;
    id_wild[16] = &err_id16;
    id_wild[17] = &err_id17;
    id_wild[18] = &err_id18;
    id_wild[19] = &err_id19;



    inf[0] = &err_inf0;//错误信息类型，在第三列
    inf[1] = &err_inf1;
    inf[2] = &err_inf2;
    inf[3] = &err_inf3;
    inf[4] = &err_inf4;
    inf[5] = &err_inf5;
    inf[6] = &err_inf6;
    inf[7] = &err_inf7;
    inf[8] = &err_inf8;
    inf[9] = &err_inf9;
    inf[10] = &err_inf10;
    inf[11] = &err_inf11;
    inf[12] = &err_inf12;
    inf[13] = &err_inf13;
    inf[14] = &err_inf14;
    inf[15] = &err_inf15;
    inf[16] = &err_inf16;
    inf[17] = &err_inf17;
    inf[18] = &err_inf18;
    inf[19] = &err_inf19;
	
	
    for (int i = 0;i < errcount;i++) //初始化错误信息
    {
        t_gen[i].setXY(30, 25*(i+1));
        t_gen[i].setColor(touchgfx::Color::getColorFromRGB(255, 0, 0));
        t_gen[i].setLinespacing(0);
        t_gen[i].setWildcard(t_gen_Buffer[i]);
        t_gen[i].resizeToCurrentText();
        t_gen[i].setTypedText(touchgfx::TypedText(T___SINGLEUSE_R4DJ));
        err_time.add(t_gen[i]);

        id_gen[i].setXY(0, 25 * (i + 1));
        id_gen[i].setColor(touchgfx::Color::getColorFromRGB(255, 0, 0));
        id_gen[i].setLinespacing(0);
        id_gen[i].setWildcard(id_gen_Buffer[i]);
        id_gen[i].resizeToCurrentText();
        id_gen[i].setTypedText(touchgfx::TypedText(T___SINGLEUSE_R4DJ));
        err_id.add(id_gen[i]);

        e_gen[i].setXY(0, 25 * (i + 1));
        e_gen[i].setColor(touchgfx::Color::getColorFromRGB(255, 0, 0));
        e_gen[i].setLinespacing(0);
        e_gen[i].setWildcard(e_gen_Buffer[i]);
        e_gen[i].resizeToCurrentText();
        e_gen[i].setTypedText(touchgfx::TypedText(T___SINGLEUSE_R4DJ));
        err_inf.add(e_gen[i]);

        e_temp[i] = &e_gen[i];
    }        
  
    //gettime();  

    //for (int i = 0;i < errcount;i++)
    //{
    //    /*touchgfx::Unicode::snprintf(e_gen_Buffer[i], 10, "%d", 12);*/
    //    //e_gen[i].setTypedText(touchgfx::TypedText(T_BSMU_ERR1));
    //    //id_gen[i].setTypedText(touchgfx::TypedText(T_ERR_TYPE3)); //错误板    
    //   /* Unicode::snprintf(t_gen_Buffer[i], 10, "%s", u_time);*/

    //    /*Unicode::fromUTF8(str_di, u_di, 128);*/
    //  
    //  
    //    //sprintf(id_temp,"第%d组",i); //char 类型  
    //    //Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[i], 10);//touchgfx仅支持显示unicode类型
    //   
    //    //sprintf(id_temp, "第%d簇BCMU错误错错错错错", 2); //char 类型
    //    //Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[i], 20);//touchgfx仅支持显示unicode类型
    //  
    //    sprintf(id_temp, "第%d簇BCMU错误(故障)", i); //char 类型
    //    Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[i], 20);//touchgfx仅支持显示unicode类型
    //    //touchgfx::Unicode::snprintf(id_gen_Buffer[i], 10, "%s", u_di);
    //      
    //  /*  Unicode::fromUTF8(str, test2Buffer1, TEST2BUFFER1_SIZE);*/

    //    t_gen[i].resizeToCurrentText();
    //    id_gen[i].resizeToCurrentText();
    //    e_gen[i].resizeToCurrentText();
    //}  
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
		
    for (int i = 0;i < 30;i++)
    {
        BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
        BMU[i]->setTouchable(false);
    }
    BMU_Container.invalidate();
}

void MainScreenView::inf_play(TextAreaWithOneWildcard* a,int b)
{
    if (direction==1) //向右移动
    {
        if (movecounter[b] < 80)
            a->moveTo(movecounter[b]++, a->getY());
        else
            direction = 0;
    }
    else 
    {
        if (movecounter[b] > 0)
            a->moveTo(movecounter[b]--, a->getY());
        
        else direction = 1;
    }
}

void MainScreenView::handleTickEvent()
{
        if (++digitalSeconds >= 60)
        {
            digitalSeconds = 0;
            if (++digitalMinutes >= 60)
            {
                digitalMinutes = 0;
                if (++digitalHours >= 24)
                {
                    digitalHours = 0;
                }
            }     
        }
        if(digitalSeconds%2==0)
        {     
            for (int j = 0;j < errcount;j++)
            inf_play(e_temp[j], j);   
        }
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

  //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[0][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 

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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[1][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 

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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[2][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[3][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[4][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[5][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[6][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[7][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[8][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[9][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[10][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[11][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[12][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[13][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[14][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[15][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[16][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[17][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[18][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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
    //更新BMU按键使能
#ifndef SIMULATOR
  for (int i = 0;i < 30;i++)
  {
      if (bmu_offline[19][i] == 0)//0表示在线，非0离线
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
          BMU[i]->setTouchable(true);
      }
      else
      {
          BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BMU[i]->setTouchable(false);
      }
  }
  BCMU_Container.invalidate();
  BMU_Container.invalidate();

#endif 
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


 void MainScreenView::gettime()
 {
#ifndef SIMULATOR
     struct tm* current_time = rtc_get_localtime();
     current_time->tm_hour = current_time->tm_hour + 8;
     if (current_time->tm_hour > 23) current_time->tm_hour -= 24;
     if (current_time != NULL) {
         sprintf(testtime, "%02d:%02d:%02d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec); //打印时间
         //printf("Current local time: %02d:%02d:%02d %02d/%02d/%04d\n",
         //    current_time->tm_hour, current_time->tm_min, current_time->tm_sec,
         //    current_time->tm_mon + 1, current_time->tm_mday, current_time->tm_year + 1900);
     }
     else {
         sprintf(testtime, "%s", "NO Internet\0");
     }
     Unicode::strncpy(u_time, testtime, 128);

#endif // !SIMULATOR

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
  Unicode::snprintf(zongdianliuBuffer, ZONGDIANLIU_SIZE, "%d", modelToViewData.station_CUR / 100);
  zongdianliu.invalidate();

  Unicode::snprintf(zongdianyaBuffer, ZONGDIANYA_SIZE, "%d", modelToViewData.station_VOL / 100);
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

  Unicode::snprintf(zhengjueyuanBuffer, ZHENGJUEYUAN_SIZE, "%d",  modelToViewData.insulation_res_p);
  zhengjueyuan.invalidate();

  Unicode::snprintf(fujueyuanBuffer, FUJUEYUAN_SIZE, "%d",  modelToViewData.insulation_res_n);
  fujueyuan.invalidate();


	//更新电池指示图，增加变色功能：0~20显示红色 20~60黄色 60~100绿色
  //SOC1_view.setValue((float)modelToViewData.BAT_SOC[0] / 10);
  //SOC2_view.setValue((float)modelToViewData.BAT_SOC[1] / 10);
  //SOC3_view.setValue((float)modelToViewData.BAT_SOC[2] / 10);
  //SOC4_view.setValue((float)modelToViewData.BAT_SOC[3] / 10);
  //SOC5_view.setValue((float)modelToViewData.BAT_SOC[4] / 10);
  //SOC6_view.setValue((float)modelToViewData.BAT_SOC[5] / 10);
  //SOC7_view.setValue((float)modelToViewData.BAT_SOC[6] / 10);
  //SOC8_view.setValue((float)modelToViewData.BAT_SOC[7] / 10);
  //SOC9_view.setValue((float)modelToViewData.BAT_SOC[8] / 10);
  //SOC10_view.setValue((float)modelToViewData.BAT_SOC[9] / 10);
  //SOC11_view.setValue((float)modelToViewData.BAT_SOC[10] / 10);
  //SOC12_view.setValue((float)modelToViewData.BAT_SOC[11] / 10);
  for (int i = 0;i < 12;i++)
  {
      SOC_view[i]->setValue((float)modelToViewData.BAT_SOC[i] / 10);
      if (((float)modelToViewData.BAT_SOC[i] / 10) >= 0 && ((float)modelToViewData.BAT_SOC[i] / 10) <= 20)
          SOC_view[i]->setColor(0xff0000);//red

      else if(((float)modelToViewData.BAT_SOC[i] / 10)>20 && ((float)modelToViewData.BAT_SOC[i] / 10) <=60)
          SOC_view[i]->setColor(0xffff00);//yellow

      else if(((float)modelToViewData.BAT_SOC[i] / 10)>60 && ((float)modelToViewData.BAT_SOC[i] / 10) <=100)
          SOC_view[i]->setColor(0x00ff00);//green
  }


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

  Unicode::snprintfFloat(CellTempText5Buffer, CELLTEMPTEXT5_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[4]/100);
	CellTempText5.invalidate();

  Unicode::snprintfFloat(CellTempText6Buffer, CELLTEMPTEXT6_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[5]/100);
	CellTempText6.invalidate();

  Unicode::snprintfFloat(CellTempText7Buffer, CELLTEMPTEXT7_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[6]/100);
	CellTempText7.invalidate();

  Unicode::snprintfFloat(CellTempText8Buffer, CELLTEMPTEXT8_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[7]/100);
	CellTempText8.invalidate();

  Unicode::snprintfFloat(CellTempText9Buffer, CELLTEMPTEXT9_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[8]/100);
	CellTempText9.invalidate();

  Unicode::snprintfFloat(CellTempText10Buffer, CELLTEMPTEXT10_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[9]/100);
	CellTempText10.invalidate();

  Unicode::snprintfFloat(CellTempText11Buffer, CELLTEMPTEXT11_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[10]/100);
	CellTempText11.invalidate();

  Unicode::snprintfFloat(CellTempText12Buffer, CELLTEMPTEXT12_SIZE, "%.2f", (float)modelToViewData.BAT_TMP[11]/100);
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

  for (int i = 0;i < 20;i++) {
      if (modelToViewData.BCMU_state[i] == offline)//不使能
      {
          BCMU_ui[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
          BCMU_ui[i]->setTouchable(false);
          BCMU_Container.invalidate();
      }
      else  if (modelToViewData.BCMU_state[i] == online)//使能
      {
          BCMU_ui[i]->setLabelText(touchgfx::TypedText(T_BCMU1-i));
          BCMU_ui[i]->setTouchable(true);
          BCMU_Container.invalidate();
      }
  }

 
      if (modelToViewData.BCMU_state[viewToModelData.BCMU_SEL - 1] == online)//当前选中的BCMU使能，刷新其BMU的状态
      {
          for (int i = 0;i < 30;i++)
          {
              if (bmu_offline[viewToModelData.BCMU_SEL - 1][i] == 0)//0表示在线，非0离线)
              {
                  BMU[i]->setLabelText(touchgfx::TypedText(T_BMU1 + i));
                  BMU[i]->setTouchable(true);
              }
              else
              {
                  BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
                  BMU[i]->setTouchable(false);
              }
          }
      }  
      else if (modelToViewData.BCMU_state[viewToModelData.BCMU_SEL - 1] == offline)
      {
          for (int i = 0;i < 30;i++)
          {
                  BMU[i]->setLabelText(touchgfx::TypedText(T_BCMU_0));
                  BMU[i]->setTouchable(false);             
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
  Unicode::strncpy(cloud_ip_buf, cloud_server_ip_buff, 16);
  Unicode::snprintf(cloud_server_ipBuffer, CLOUD_SERVER_IP_SIZE, "%s", cloud_ip_buf);
  cloud_server_ip.invalidate();



  for (int i = 0;i < 4;i++) //
  {
      if (bsmuSetting.IP_ADD_1[i] > 255)
          bsmuSetting.IP_ADD_1[i] = 0;
  }
  Unicode::snprintf(local_server_ipBuffer, LOCAL_SERVER_IP_SIZE, "%d", bsmuSetting.IP_ADD_1[0]);//本地服务器IP
  Unicode::snprintf(local_server_ip2Buffer, LOCAL_SERVER_IP2_SIZE, "%d", bsmuSetting.IP_ADD_1[1]);
  Unicode::snprintf(local_server_ip3Buffer, LOCAL_SERVER_IP3_SIZE, "%d", bsmuSetting.IP_ADD_1[2]);
  Unicode::snprintf(local_server_ip4Buffer, LOCAL_SERVER_IP4_SIZE, "%d", bsmuSetting.IP_ADD_1[3]);
  local_server_ip.invalidate();
  local_server_ip2.invalidate();
  local_server_ip3.invalidate();
  local_server_ip4.invalidate();


  int err_counter = 0;
  int err_sum = 0;
  char id_temp[128];
  char err_temp[128];

  //Client_Sd[cluster_num].error_count
  //Client_errors[cluster_num][MAX_ERROR]
 //gettime();
 //for (int i = 0;i < errcount;i++)
 //{
 //    /*touchgfx::Unicode::snprintf(e_gen_Buffer[i], 10, "%d", 12);*/
 //    e_gen[i].setTypedText(touchgfx::TypedText(T_BSMU_ERR1));
 //    id_gen[i].setTypedText(touchgfx::TypedText(T_ERR_TYPE3)); //错误板    
 //    Unicode::snprintf(t_gen_Buffer[i], 10, "%s", u_time);
 //    t_gen[i].resizeToCurrentText();
 //    id_gen[i].resizeToCurrentText();
 //    e_gen[i].resizeToCurrentText();
 //}

  //计算总错误数量并决定是否输出

  for (int m = 0;m < cluster_num;m++)
  {
      err_sum += Client_Sd[m].error_count;
  }

  if (err_sum > 0)//有错误数据，遍历输出
  {

      for (int i = 0;i < errcount;i++)//清空上轮显示
      {
          Unicode::snprintf(t_gen_Buffer[i],10,"%c",' ');
          Unicode::snprintf(id_gen_Buffer[i],10,"%c",' ');
          e_gen[i].setTypedText(touchgfx::TypedText(T_NO_ERR));

      }
      scrollableContainer1.setVisible(true); //错误显示区域使能
      scrollableContainer1.invalidate();
      /*共五种错误信息格式：
      1、BSMU错误
      2、第X簇BCMU错误，其中位宽为2的信息 00 无故障   01和11均有故障  10预警
      3、第X簇X组BMU(组内)板错误
      4、第X簇X组BMU(组内)X号电池错误
      5、第X簇BMU(组间错误)*/

      for (int k = 0;k < cluster_num;k++) //遍历错误二维数组Client_errors[cluster_num][MAX_ERROR]中的cluster_num
      {
          if (BCMU[k].OnlineOrOffline == Offline) continue;
          for (int i = 0;i < Client_Sd[k].error_count;i++) //Client_errors[cluster_num][MAX_ERROR]中的MAX_ERROR
          {                       
              if (Client_errors[k][i].error_id_h == 0x00) //判断id高8位
              {
                  if (Client_errors[k][i].error_id_l == 0x00)//判断id低8位，BSMU错误
                  {
                      for (int j = 0;j < 16;j++) //逐位判断错误信息，并输出
                      {
                          if (get_one_bit_value(Client_errors[k][i].error_code, j + 1) == 1)
                          {
                              if (err_counter >= errcount) err_counter = err_counter % errcount;
                              gettime();
                              Unicode::snprintf(t_gen_Buffer[err_counter], 20, "%s", u_time);//打印错误时间
                              id_gen[err_counter].setTypedText(touchgfx::TypedText(T_ERR_TYPE0)); //错误板             
                              e_gen[err_counter].setTypedText(touchgfx::TypedText(T_BSMU_ERR0 - j)); //错误类型
                              t_gen[err_counter].resizeToCurrentText(); //调整文本大小以正常显示
                              id_gen[err_counter].resizeToCurrentText();
                              e_gen[err_counter].resizeToCurrentText();
                              err_counter++;

                          }
                      }

                  }
                  else if (Client_errors[k][i].error_id_l == 0x01 || Client_errors[k][i].error_id_l == 0x02)//判断id低8位，BCMU错误
                  {
                      if (Client_errors[k][i].error_id_l == 0x01) //错误位宽为2
                      {
                          for (int j = 0;j < 8;j++)
                          {
                              if (get_two_bit_value(Client_errors[k][i].error_code, j + 1) == 2)//预警
                              {
                                  if (err_counter >= errcount) err_counter = err_counter % errcount;
                                  gettime();
                                  Unicode::snprintf(t_gen_Buffer[err_counter], 20, "%s", u_time);//打印错误时间                                 
                                  sprintf(id_temp, "第%d簇BCMU错误(预警)", k+1); //char 类型
                                  Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[err_counter], 20);//touchgfx仅支持显示unicode类型
                                  //id_gen[err_counter].setTypedText(touchgfx::TypedText(T_ERR_TYPE1)); //错误板    
                                  e_gen[err_counter].setTypedText(touchgfx::TypedText(T_BCMU_ERR_2BIT_0 - j));
                                  t_gen[err_counter].resizeToCurrentText(); //调整文本大小以正常显示
                                  id_gen[err_counter].resizeToCurrentText();
                                  e_gen[err_counter].resizeToCurrentText();
                                  err_counter++;
                              }
                              else if (get_two_bit_value(Client_errors[k][i].error_code, j + 1) == 3 || get_two_bit_value(Client_errors[k][i].error_code, j + 1) == 1)//预警过，故障发生
                              {
                                  if (err_counter >= errcount) err_counter = err_counter % errcount;
                                  gettime();
                                  Unicode::snprintf(t_gen_Buffer[err_counter], 20, "%s", u_time);//打印错误时间                                 
                                  sprintf(id_temp, "第%d簇BCMU错误(故障)", k+1); //char 类型
                                  Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[err_counter], 20);//touchgfx仅支持显示unicode类型
                                  //id_gen[err_counter].setTypedText(touchgfx::TypedText(T_ERR_TYPE1)); //错误板    
                                  e_gen[err_counter].setTypedText(touchgfx::TypedText(T_BCMU_ERR_2BIT_0 - j));
                                  t_gen[err_counter].resizeToCurrentText(); //调整文本大小以正常显示
                                  id_gen[err_counter].resizeToCurrentText();
                                  e_gen[err_counter].resizeToCurrentText();
                                  err_counter++;
                              }

                          }
                      }
                      if (Client_errors[k][i].error_id_l == 0x02)//错误位宽为1
                      {
                          for (int j = 0;j < 16;j++)
                          {
                              if (get_one_bit_value(Client_errors[k][i].error_code, j + 1) == 0) //无故障
                              {

                              }
                              else if (get_one_bit_value(Client_errors[k][i].error_code, j + 1) == 1) //无故障
                              {
                                  if (err_counter >= errcount) err_counter = err_counter % errcount;
                                  gettime();
                                  Unicode::snprintf(t_gen_Buffer[err_counter], 20, "%s", u_time);//打印错误时间
                                  sprintf(id_temp, "第%d簇BCMU错误", k+1); //char 类型
                                  Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[err_counter], 20);//touchgfx仅支持显示unicode类型
                                  //id_gen[err_counter].setTypedText(touchgfx::TypedText(T_ERR_TYPE1)); //错误板    
                                  e_gen[err_counter].setTypedText(touchgfx::TypedText(T_BCMU_ERR0 - j));
                                  t_gen[err_counter].resizeToCurrentText(); //调整文本大小以正常显示
                                  id_gen[err_counter].resizeToCurrentText();
                                  e_gen[err_counter].resizeToCurrentText();
                                  err_counter++;
                              }
                          }
                      }
                  }
              }
              else if (Client_errors[k][i].error_id_h >= 0x01 && Client_errors[k][i].error_id_h <= 0x1F) //判断id高8位
              {
                  if (Client_errors[k][i].error_id_l == 0x00) //BMU（组内）板错误
                  {
                      /*Unicode::snprintf(id[i], 20, "%s", "BMU(组内)板错误!\0");*/
                      for (int j = 0;j < 16;j++)
                      {
                          if (get_one_bit_value(Client_errors[k][i].error_code, j + 1) == 1)
                          {
                              if (err_counter >= errcount) err_counter = err_counter % errcount;
                              gettime();
                              Unicode::snprintf(t_gen_Buffer[err_counter], 20, "%s", u_time);//打印错误时间
                              //id_gen[err_counter].setTypedText(touchgfx::TypedText(T_ERR_TYPE2)); //错误板   
                              sprintf(id_temp, "第%d簇%d组BMU(组内)板错误", k+1, Client_errors[k][i].error_id_h);//char 类型
                              Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[err_counter], 20);//touchgfx仅支持显示unicode类型
                              e_gen[err_counter].setTypedText(touchgfx::TypedText(T_BMU_BOARD_ERR0 - j));
                             
                              t_gen[err_counter].resizeToCurrentText();//调整文本大小以正常显示
                              id_gen[err_counter].resizeToCurrentText();
                              e_gen[err_counter].resizeToCurrentText();
                              err_counter++;
                          }
                      }
                  }
                  else if (Client_errors[k][i].error_id_l >= 0x01 && Client_errors[k][i].error_id_l <= 0x0C)//BMU(组内)电池错误
                  {
                      for (int j = 0;j < 16;j++)
                      {
                          if (get_one_bit_value(Client_errors[k][i].error_code, j + 1) == 1)
                          {
                              if (err_counter >= errcount) err_counter = err_counter % errcount;
                              gettime();
                              Unicode::snprintf(t_gen_Buffer[err_counter], 20, "%s", u_time);//打印错误时间
                              //id_gen[err_counter].setTypedText(touchgfx::TypedText(T_ERR_TYPE3)); //错误板   
                              sprintf(id_temp, "第%d簇%d组BMU(组内)%d号电池错误",k+1, Client_errors[k][i].error_id_h, Client_errors[k][i].error_id_l);//char 类型
                              Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[err_counter], 20);//touchgfx仅支持显示unicode类型
                              e_gen[err_counter].setTypedText(touchgfx::TypedText(T_BMU_BATTERY_ERR0 - j));

                              t_gen[err_counter].resizeToCurrentText(); //调整文本大小以正常显示
                              id_gen[err_counter].resizeToCurrentText();
                              e_gen[err_counter].resizeToCurrentText();
                              err_counter++;
                          }
                      }
                  }
              }
              else if (Client_errors[k][i].error_id_h == 0x20) //判断id高8位
              {
                  if (Client_errors[k][i].error_id_l == 0)//BMU(组间错误)
                  {            
                      for (int j = 0;j < 16;j++)
                      {
                          if (get_one_bit_value(Client_errors[k][i].error_code, j + 1) == 1)
                          {
                              if (err_counter >= errcount) err_counter = err_counter % errcount;
                              gettime();
                              Unicode::snprintf(t_gen_Buffer[err_counter], 20, "%s", u_time);//打印错误时间
                              //id_gen[err_counter].setTypedText(touchgfx::TypedText(T_ERR_TYPE4)); //错误板 
                              sprintf(id_temp, "第%d簇BMU(组间错误)", k+1);//char 类型
                              Unicode::fromUTF8((const uint8_t*)id_temp, id_gen_Buffer[err_counter], 20);//touchgfx仅支持显示unicode类型
                              e_gen[err_counter].setTypedText(touchgfx::TypedText(T_BMU_BETWEEN_ERR0 - j));
                              t_gen[err_counter].resizeToCurrentText(); //调整文本大小以正常显示
                              id_gen[err_counter].resizeToCurrentText();
                              e_gen[err_counter].resizeToCurrentText();
                              err_counter++;
                          }
                      }
                  }
              }
          }
          scrollableContainer1.setVisible(true);
          scrollableContainer1.invalidate();
      }
  }
  else if (err_sum == 0)
  {
      for (int i = 0;i < errcount;i++)
      {
          Unicode::snprintf(t_gen_Buffer[i], 10, "%c", ' ');
          Unicode::snprintf(id_gen_Buffer[i], 10, "%c", ' ');
          e_gen[i].setTypedText(touchgfx::TypedText(T_NO_ERR));
      }
      scrollableContainer1.setVisible(false);
      scrollableContainer1.invalidate();
  }
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



  

}


#endif


