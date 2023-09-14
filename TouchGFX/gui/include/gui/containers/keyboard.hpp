/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2023-04-19 20:53:51
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-20 16:49:33
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\TouchGFX\gui\include\gui\containers\keyboard.hpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <gui_generated/containers/keyboardBase.hpp>

class keyboard : public keyboardBase
{
public:
    keyboard();
    virtual ~keyboard() {}

    virtual void fun_num_1();
    virtual void fun_num_2();
    virtual void fun_num_3();
    virtual void fun_num_4();
    virtual void fun_num_5();
    virtual void fun_num_6();
    virtual void fun_num_7();
    virtual void fun_num_8();
    virtual void fun_num_9();
    virtual void fun_num_0();
    virtual void fun_ce();
    virtual void fun_de();
    virtual void fun_ok();


    virtual void initialize();
    void SetBufPar(void* p, uint8_t size);

    // char buff[10];
    long buff;
    uint8_t refreshFlag;
protected:
    int weishu;
    void* des;
    uint8_t dessize;
};

#endif // KEYBOARD_HPP
