/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2023-04-22 11:12:31
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-22 11:20:37
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\touchgfx\gui\src\containers\customcontainer2.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include <gui/containers/CustomContainer2.hpp>

CustomContainer2::CustomContainer2()
{

}

void CustomContainer2::initialize()
{
    CustomContainer2Base::initialize();
}

void CustomContainer2::SetText(int val1)
{
    Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%d", val1);
    textArea1.invalidate();
}
