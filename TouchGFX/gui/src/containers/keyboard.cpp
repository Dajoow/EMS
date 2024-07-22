/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2023-04-19 20:53:51
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-20 19:01:36
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\touchgfx\gui\src\containers\keyboard.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include <gui/containers/keyboard.hpp>
#include <touchgfx/Utils.hpp>
#ifndef SIMULATOR
extern "C"{
#include "string.h"

};
#endif

keyboard::keyboard():buff(0),refreshFlag(0),weishu(0),des(NULL),dessize(1)
{

}

void keyboard::initialize()
{
    keyboardBase::initialize();
    des = NULL;
    dessize = 1;
    refreshFlag = 0;
    weishu = 0;
    buff = 0;
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
}

void keyboard::SetBufPar(void* p, uint8_t size)
{
    des = p;
    dessize = size;
}

void keyboard::fun_num_1()
{
    if(weishu == 0){ //当初值为0，点击1后，数值为1
        buff = 1;
        weishu = 1;
    }
    else{//初值不为0
        buff = buff*10 + 1;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);

    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_2()
{
    if(weishu == 0){
        buff = 2;
        weishu = 1;
    }
    else{
        buff = buff*10 + 2;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_3()
{
    if(weishu == 0){
        buff = 3;
        weishu = 1;
    }
    else{
        buff = buff*10 + 3;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}
void keyboard::fun_num_4()
{
    if(weishu == 0){
        buff = 4;
        weishu = 1;
    }
    else{
        buff = buff*10 + 4;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_5()
{
    if(weishu == 0){
        buff = 5;
        weishu = 1;
    }
    else{
        buff = buff*10 + 5;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_6()
{
    if(weishu == 0){
        buff = 6;
        weishu = 1;
    }
    else{
        buff = buff*10 + 6;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_7()
{
    if(weishu == 0){
        buff = 7;
        weishu = 1;
    }
    else{
        buff = buff*10 + 7;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_8()
{
    if(weishu == 0){
        buff = 8;
        weishu = 1;
    }
    else{
        buff = buff*10 + 8;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_9()
{
    if(weishu == 0){
        buff = 9;
        weishu = 1;
    }
    else{
        buff = buff*10 + 9;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_num_0()
{
    if(weishu == 0){
        buff = 0;
        weishu = 0;
    }
    else{
        buff = buff*10;
        weishu++;
        if(buff >= 65535){
            buff = 65535;
            weishu = 5;
        }
    }
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

//清零
void keyboard::fun_ce()
{
    buff = 0;
    weishu = 0;
    touchgfx_printf("buff: %d , weishu:%d\n", buff, weishu);
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

//删除末位
void keyboard::fun_de()
{
    if(!buff){//初值为0
        buff = 0;
        weishu = 0;
    }
    else{//初值不为0
        buff = buff/10;
        weishu--;
        if(!buff){
            buff = 0;
            weishu = 0;
        }
    }
    touchgfx_printf("buff: %d , weishu:%d\n", buff, weishu);
    Unicode::snprintf(shownumBuffer, SHOWNUM_SIZE, "%d", buff);
    shownum.invalidate();
    fun_ok();
}

void keyboard::fun_ok()
{
    long tem;
    tem = buff;
    if(des!=NULL){
        refreshFlag = 1;
        if(dessize == 1){ //变量的字节数
            if(tem > 255)
                tem = 255;
            *(uint8_t*)(des) = tem;
        }
        else if(dessize == 2)
            *(uint16_t*)(des) = tem;
    }
}

