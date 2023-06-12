#include "lcd.h"


void LCD_init()
{
    uint8_t G7_state = 1;
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};  
    GPIO_InitStruct.Pin = LCD_G7_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;                
    HAL_GPIO_Init(LCD_G7_GPIO_Port, &GPIO_InitStruct);

    G7_state = HAL_GPIO_ReadPin(LCD_G7_GPIO_Port, LCD_G7_PIN);
    //等待LCD上电
    while(!G7_state)
    {
        /* code */
			G7_state = HAL_GPIO_ReadPin(LCD_G7_GPIO_Port, LCD_G7_PIN);
			HAL_Delay(10);
    }
    HAL_Delay(10);
}






