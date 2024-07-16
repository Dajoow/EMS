#ifndef  _LCD_H_
#define _LCD_H_

#include "main.h"

#define LCD_G7_PIN GPIO_PIN_3
#define LCD_G7_GPIO_Port GPIOD


#define LCD_BL(n)    (n?HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET):HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET))

#define LCD_ON                                                                 \
  do{if (HAL_GPIO_ReadPin(LCD_BL_GPIO_Port, LCD_BL_Pin) == GPIO_PIN_RESET)     \
       HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);}while (0)

void LCD_init(void);




#endif

