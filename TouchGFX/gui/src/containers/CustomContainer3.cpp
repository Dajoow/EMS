/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2023-04-21 20:11:14
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-04-21 20:15:52
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\touchgfx\gui\src\containers\customcontainer3.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#include <gui/containers/CustomContainer3.hpp>

CustomContainer3::CustomContainer3()
{

}

void CustomContainer3::initialize()
{
    CustomContainer3Base::initialize();
}

void CustomContainer3::SetText(int val1, int val2)
{
    Unicode::snprintf(textArea1Buffer1, TEXTAREA1BUFFER1_SIZE, "%d", val1);
    Unicode::snprintf(textArea1Buffer2, TEXTAREA1BUFFER2_SIZE, "%d", val2);
    textArea1.invalidate();
}
