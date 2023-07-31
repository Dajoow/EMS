#include <gui/mainscreen_screen/MainScreenView.hpp>
#include <touchgfx/Utils.hpp>
#include <touchgfx/Callback.hpp>
#include <texts/TextKeysAndLanguages.hpp>
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
  /*setStateChangedCallback() 函数注册回调函数*/
  BMUMenu.setStateChangedCallback(BMUMenuCallback);
}

void BMUMenuCallback_t::execute(const SlideMenu& menu)
{
  touchgfx_printf("BMUMenuCallback\n");
  if(view_.GetCellShowState())
    view_.CellStateShow_OFF();
  else
    view_.CellStateShow();
}

/**
 * @description: 查询电池显示的情况
 * @return {*}
 */
bool MainScreenView::GetCellShowState()
{
  if (CellShow.getState() ==touchgfx::SlideMenu::EXPANDED)
    return true;
  else
    return false;
}

/**
 * @description: 展开BMU菜单
 * @return {*}
 */
void MainScreenView::CellStateShow()
{
    BMUMenu.animateToState(touchgfx::SlideMenu::EXPANDED);
    CellShow.animateToState(touchgfx::SlideMenu::EXPANDED);
    BMU_SEL_BOX.setVisible(true);
    BMU_SEL_BOX.invalidate();
}

/**
 * @description: 收回BMU菜单
 * @return {*}
 */
void MainScreenView::CellStateShow_OFF()
{
    // BCMU_SEL_BOX.setVisible(false);
    // BCMU_SEL_BOX.invalidate();
    // BMU_SEL_BOX.setVisible(false);
    // BMU_SEL_BOX.invalidate();
    // viewToModelData.BCMU_SEL = 1;   //1-20
    viewToModelData.BMU_SEL = 1;    //1-30
    
    BMU_SEL_BOX.setXY(BMU1.getX()-(BMU_SEL_BOX.getWidth()-BMU1.getWidth())/2, BMU1.getY()-(BMU_SEL_BOX.getHeight()-BMU1.getHeight())/2);
    BMU_BG.invalidate();
    //更新选定组编号
    Unicode::snprintf(zuBuffer, ZU_SIZE, "%d", viewToModelData.BMU_SEL);
    zu.invalidate();
    presenter->ViewtoModelDat(viewToModelData);

    BMUMenu.animateToState(touchgfx::SlideMenu::COLLAPSED);
    CellShow.animateToState(touchgfx::SlideMenu::COLLAPSED);

}

void MainScreenView::setupScreen()
{
    MainScreenViewBase::setupScreen();
	//通知model更新数据
	viewToModelData.reflashFlag = true;
	presenter->ViewtoModelDat(viewToModelData);
}

void MainScreenView::tearDownScreen()
{
    MainScreenViewBase::tearDownScreen();
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
  
	//更新电池电压值
	Unicode::snprintfFloat(CellText1Buffer, CELLTEXT1_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[0]/10000);
	CellText1.invalidate();
	
	Unicode::snprintfFloat(CellText2Buffer, CELLTEXT2_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[1]/10000);
	CellText2.invalidate();
	
	Unicode::snprintfFloat(CellText3Buffer, CELLTEXT3_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[2]/10000);
	CellText3.invalidate();

	Unicode::snprintfFloat(CellText4Buffer, CELLTEXT4_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[3]/10000);
	CellText4.invalidate();

	Unicode::snprintfFloat(CellText5Buffer, CELLTEXT5_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[4]/10000);
	CellText5.invalidate();

	Unicode::snprintfFloat(CellText6Buffer, CELLTEXT6_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[5]/10000);
	CellText6.invalidate();

	Unicode::snprintfFloat(CellText7Buffer, CELLTEXT7_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[6]/10000);
	CellText7.invalidate();

	Unicode::snprintfFloat(CellText8Buffer, CELLTEXT8_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[7]/10000);
	CellText8.invalidate();

	Unicode::snprintfFloat(CellText9Buffer, CELLTEXT9_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[8]/10000);
	CellText9.invalidate();

	Unicode::snprintfFloat(CellText10Buffer, CELLTEXT10_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[9]/10000);
	CellText10.invalidate();

	Unicode::snprintfFloat(CellText11Buffer, CELLTEXT11_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[10]/10000);
	CellText11.invalidate();

	Unicode::snprintfFloat(CellText12Buffer, CELLTEXT12_SIZE, "%.3f", (float)modelToViewData.BAT_VOL[11]/10000);
	CellText12.invalidate();

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

  Unicode::snprintf(cudianyaBuffer, CUDIANYA_SIZE, "%d", modelToViewData.cluster_VOL);
  cudianya.invalidate();

  Unicode::snprintf(cudianliuBuffer, CUDIANLIU_SIZE, "%d",  modelToViewData.cluster_CUR);
  cudianliu.invalidate();

  Unicode::snprintf(zhengjueyuanBuffer, ZHENGJUEYUAN_SIZE, "%d",  modelToViewData.insulation_res_n);
  zhengjueyuan.invalidate();

  Unicode::snprintf(fujueyuanBuffer, FUJUEYUAN_SIZE, "%d",  modelToViewData.insulation_res_p);
  fujueyuan.invalidate();


	//更新电池指示图
  SOC1_view.setValue(modelToViewData.BAT_VOL[0]);
  SOC2_view.setValue(modelToViewData.BAT_VOL[1]);
  SOC3_view.setValue(modelToViewData.BAT_VOL[2]);
  SOC4_view.setValue(modelToViewData.BAT_VOL[3]);
  SOC5_view.setValue(modelToViewData.BAT_VOL[4]);
  SOC6_view.setValue(modelToViewData.BAT_VOL[5]);
  SOC7_view.setValue(modelToViewData.BAT_VOL[6]);
  SOC8_view.setValue(modelToViewData.BAT_VOL[7]);
  SOC9_view.setValue(modelToViewData.BAT_VOL[8]);
  SOC10_view.setValue(modelToViewData.BAT_VOL[9]);
  SOC11_view.setValue(modelToViewData.BAT_VOL[10]);
  SOC12_view.setValue(modelToViewData.BAT_VOL[11]);

	//更新电池SOC
  Unicode::snprintf(CellSOCText1Buffer, CELLSOCTEXT1_SIZE, "%d", modelToViewData.BAT_SOC[0]/10);
	CellSOCText1.invalidate();

  Unicode::snprintf(CellSOCText2Buffer, CELLSOCTEXT2_SIZE, "%d", modelToViewData.BAT_SOC[1]/10);
	CellSOCText2.invalidate();

  Unicode::snprintf(CellSOCText3Buffer, CELLSOCTEXT3_SIZE, "%d", modelToViewData.BAT_SOC[2]/10);
	CellSOCText3.invalidate();

  Unicode::snprintf(CellSOCText4Buffer, CELLSOCTEXT4_SIZE, "%d", modelToViewData.BAT_SOC[3]/10);
	CellSOCText4.invalidate();

  Unicode::snprintf(CellSOCText5Buffer, CELLSOCTEXT5_SIZE, "%d", modelToViewData.BAT_SOC[4]/10);
	CellSOCText5.invalidate();

  Unicode::snprintf(CellSOCText6Buffer, CELLSOCTEXT6_SIZE, "%d", modelToViewData.BAT_SOC[5]/10);
	CellSOCText6.invalidate();

  Unicode::snprintf(CellSOCText7Buffer, CELLSOCTEXT7_SIZE, "%d", modelToViewData.BAT_SOC[6]/10);
	CellSOCText7.invalidate();

  Unicode::snprintf(CellSOCText8Buffer, CELLSOCTEXT8_SIZE, "%d", modelToViewData.BAT_SOC[7]/10);
	CellSOCText8.invalidate();

  Unicode::snprintf(CellSOCText9Buffer, CELLSOCTEXT9_SIZE, "%d", modelToViewData.BAT_SOC[8]/10);
	CellSOCText9.invalidate();

  Unicode::snprintf(CellSOCText10Buffer, CELLSOCTEXT10_SIZE, "%d", modelToViewData.BAT_SOC[9]/10);
	CellSOCText10.invalidate();

  Unicode::snprintf(CellSOCText11Buffer, CELLSOCTEXT11_SIZE, "%d", modelToViewData.BAT_SOC[10]/10);
	CellSOCText11.invalidate();

  Unicode::snprintf(CellSOCText12Buffer, CELLSOCTEXT12_SIZE, "%d", modelToViewData.BAT_SOC[11]/10);
	CellSOCText12.invalidate();
	//更新电池温度
  Unicode::snprintfFloat(CellTempText1Buffer, CELLTEMPTEXT1_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText1.invalidate();

  Unicode::snprintfFloat(CellTempText2Buffer, CELLTEMPTEXT2_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[1]/100);
	CellTempText2.invalidate();

  Unicode::snprintfFloat(CellTempText3Buffer, CELLTEMPTEXT3_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[2]/100);
	CellTempText3.invalidate();

  Unicode::snprintfFloat(CellTempText4Buffer, CELLTEMPTEXT4_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[3]/100);
	CellTempText4.invalidate();

  Unicode::snprintfFloat(CellTempText5Buffer, CELLTEMPTEXT5_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText5.invalidate();

  Unicode::snprintfFloat(CellTempText6Buffer, CELLTEMPTEXT6_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText6.invalidate();

  Unicode::snprintfFloat(CellTempText7Buffer, CELLTEMPTEXT7_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText7.invalidate();

  Unicode::snprintfFloat(CellTempText8Buffer, CELLTEMPTEXT8_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText8.invalidate();

  Unicode::snprintfFloat(CellTempText9Buffer, CELLTEMPTEXT9_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText9.invalidate();

  Unicode::snprintfFloat(CellTempText10Buffer, CELLTEMPTEXT10_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText10.invalidate();

  Unicode::snprintfFloat(CellTempText11Buffer, CELLTEMPTEXT11_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText11.invalidate();

  Unicode::snprintfFloat(CellTempText12Buffer, CELLTEMPTEXT12_SIZE, "%.1f", (float)modelToViewData.BAT_TMP[0]/100);
	CellTempText12.invalidate();

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

  if(modelToViewData.BCMU_state[0] == offline && BCMU1.isTouchable()==true){
    //BCMU离线的情况，不使能BCMU按键
    BCMU1.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU1.setTouchable(false);
    BCMU_BG.invalidate();
  }
  else if(modelToViewData.BCMU_state[0] != offline && BCMU1.isTouchable()==false){
    //使能BCMU按键
    BCMU1.setLabelText(touchgfx::TypedText(T_BCMU1));
    BCMU1.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[1] == offline && BCMU2.isTouchable()==true){
    //不使能BCMU按键
    BCMU2.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU2.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[1] != offline && BCMU2.isTouchable()==false){
    //使能BCMU按键
    BCMU2.setLabelText(touchgfx::TypedText(T_BCMU2));
    BCMU2.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[2] == offline && BCMU3.isTouchable()==true){
    //不使能BCMU按键
    BCMU3.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU3.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[2] != offline && BCMU3.isTouchable()==false){
    //使能BCMU按键
    BCMU3.setLabelText(touchgfx::TypedText(T_BCMU3));
    BCMU3.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[3] == offline && BCMU4.isTouchable()==true){
    //不使能BCMU按键
    BCMU4.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU4.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[3] != offline && BCMU4.isTouchable()==false){
    //使能BCMU按键
    BCMU4.setLabelText(touchgfx::TypedText(T_BCMU4));
    BCMU4.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[4] == offline && BCMU5.isTouchable()==true){
    //不使能BCMU按键
    BCMU5.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU5.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[4] != offline && BCMU5.isTouchable()==false){
    //使能BCMU按键
    BCMU5.setLabelText(touchgfx::TypedText(T_BCMU5));
    BCMU5.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[5] == offline && BCMU6.isTouchable()==true){
    //不使能BCMU按键
    BCMU6.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU6.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[5] != offline && BCMU6.isTouchable()==false){
    //使能BCMU按键
    BCMU6.setLabelText(touchgfx::TypedText(T_BCMU6));
    BCMU6.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[6] == offline && BCMU7.isTouchable()==true){
    //不使能BCMU按键
    BCMU7.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU7.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[6] != offline && BCMU7.isTouchable()==false){
    //使能BCMU按键
    BCMU7.setLabelText(touchgfx::TypedText(T_BCMU7));
    BCMU7.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[7] == offline && BCMU8.isTouchable()==true){
    //不使能BCMU按键
    BCMU8.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU8.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[7] != offline && BCMU8.isTouchable()==false){
    //使能BCMU按键
    BCMU8.setLabelText(touchgfx::TypedText(T_BCMU8));
    BCMU8.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[8] == offline && BCMU9.isTouchable()==true){
    //不使能BCMU按键
    BCMU9.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU9.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[8] != offline && BCMU9.isTouchable()==false){
    //使能BCMU按键
    BCMU9.setLabelText(touchgfx::TypedText(T_BCMU9));
    BCMU9.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[9] == offline && BCMU10.isTouchable()==true){
    //不使能BCMU按键
    BCMU10.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU10.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[9] != offline && BCMU10.isTouchable()==false){
    //使能BCMU按键
    BCMU10.setLabelText(touchgfx::TypedText(T_BCMU10));
    BCMU10.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[10] == offline && BCMU11.isTouchable()==true){
    //不使能BCMU按键
    BCMU11.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU11.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[10] != offline && BCMU11.isTouchable()==false){
    //使能BCMU按键
    BCMU11.setLabelText(touchgfx::TypedText(T_BCMU11));
    BCMU11.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[11] == offline && BCMU12.isTouchable()==true){
    //不使能BCMU按键
    BCMU12.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU12.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[11] != offline && BCMU12.isTouchable()==false){
    //使能BCMU按键
    BCMU12.setLabelText(touchgfx::TypedText(T_BCMU12));
    BCMU12.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[12] == offline && BCMU13.isTouchable()==true){
    //不使能BCMU按键
    BCMU13.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU13.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[12] != offline && BCMU13.isTouchable()==false){
    //使能BCMU按键
    BCMU13.setLabelText(touchgfx::TypedText(T_BCMU13));
    BCMU13.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[13] == offline && BCMU14.isTouchable()==true){
    //不使能BCMU按键
    BCMU14.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU14.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[13] != offline && BCMU14.isTouchable()==false){
    //使能BCMU按键
    BCMU14.setLabelText(touchgfx::TypedText(T_BCMU14));
    BCMU14.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[14] == offline && BCMU15.isTouchable()==true){
    //不使能BCMU按键
    BCMU15.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU15.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[14] != offline && BCMU15.isTouchable()==false){
    //使能BCMU按键
    BCMU15.setLabelText(touchgfx::TypedText(T_BCMU15));
    BCMU15.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[15] == offline && BCMU16.isTouchable()==true){
    //不使能BCMU按键
    BCMU16.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU16.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[15] != offline && BCMU16.isTouchable()==false){
    //使能BCMU按键
    BCMU16.setLabelText(touchgfx::TypedText(T_BCMU16));
    BCMU16.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[16] == offline && BCMU17.isTouchable()==true){
    //不使能BCMU按键
    BCMU17.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU17.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[16] != offline && BCMU17.isTouchable()==false){
    //使能BCMU按键
    BCMU17.setLabelText(touchgfx::TypedText(T_BCMU17));
    BCMU17.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[17] == offline && BCMU18.isTouchable()==true){
    //不使能BCMU按键
    BCMU18.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU18.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[17] != offline && BCMU18.isTouchable()==false){
    //使能BCMU按键
    BCMU18.setLabelText(touchgfx::TypedText(T_BCMU18));
    BCMU18.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[18] == offline && BCMU19.isTouchable()==true){
    //不使能BCMU按键
    BCMU19.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU19.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[18] != offline && BCMU19.isTouchable()==false){
    //使能BCMU按键
    BCMU19.setLabelText(touchgfx::TypedText(T_BCMU19));
    BCMU19.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  if(modelToViewData.BCMU_state[19] == offline && BCMU20.isTouchable()==true){
    //不使能BCMU按键
    BCMU20.setLabelText(touchgfx::TypedText(T_BCMU_0));
    BCMU20.setTouchable(false);
    BCMU_BG.invalidate();	
  }
  else if(modelToViewData.BCMU_state[19] != offline && BCMU20.isTouchable()==false){
    //使能BCMU按键
    BCMU20.setLabelText(touchgfx::TypedText(T_BCMU20));
    BCMU20.setTouchable(true);
    BCMU_BG.invalidate();	
  }

  

}


#endif


