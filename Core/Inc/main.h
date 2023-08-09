/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BufStartAdd_3 0xC0258000
#define BufStartAdd_1 0xC0000000
#define BufStartAdd_2 0xC012C000
#define StationBuffAdd 0xC03AF000
#define ETHSendBuffAdd 0xC0384000
#define CANRevBuffStartAdd 0xC03B0000
#define CANSndBuffStartAdd 0xC03F8000
#define LED_G_Pin GPIO_PIN_2
#define LED_G_GPIO_Port GPIOE
#define LED_B_Pin GPIO_PIN_3
#define LED_B_GPIO_Port GPIOE
#define VSYNC_FREQ_Pin GPIO_PIN_4
#define VSYNC_FREQ_GPIO_Port GPIOE
#define RENDER_TIME_Pin GPIO_PIN_5
#define RENDER_TIME_GPIO_Port GPIOE
#define FRAME_RATE_Pin GPIO_PIN_6
#define FRAME_RATE_GPIO_Port GPIOE
#define MCU_ACTIVE_Pin GPIO_PIN_13
#define MCU_ACTIVE_GPIO_Port GPIOC
#define ETH_RESET_Pin GPIO_PIN_0
#define ETH_RESET_GPIO_Port GPIOA
#define AT24CXX_SCL_Pin GPIO_PIN_12
#define AT24CXX_SCL_GPIO_Port GPIOD
#define AT24CXX_SDA_Pin GPIO_PIN_13
#define AT24CXX_SDA_GPIO_Port GPIOD
#define LCD_BL_Pin GPIO_PIN_7
#define LCD_BL_GPIO_Port GPIOC
#define SPI2_NSS_Pin GPIO_PIN_9
#define SPI2_NSS_GPIO_Port GPIOC
#define BEEP_Pin GPIO_PIN_15
#define BEEP_GPIO_Port GPIOA
#define EC600_RST_Pin GPIO_PIN_4
#define EC600_RST_GPIO_Port GPIOD
#define EC600_EN_Pin GPIO_PIN_9
#define EC600_EN_GPIO_Port GPIOG
#define LCD_RESET_Pin GPIO_PIN_3
#define LCD_RESET_GPIO_Port GPIOB
#define T_CS_Pin GPIO_PIN_4
#define T_CS_GPIO_Port GPIOB
#define T_PEN_Pin GPIO_PIN_5
#define T_PEN_GPIO_Port GPIOB
#define T_IC2_SCL_Pin GPIO_PIN_6
#define T_IC2_SCL_GPIO_Port GPIOB
#define T_IC2_SDA_Pin GPIO_PIN_7
#define T_IC2_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define TOUCHGFX_ENABLE 1
#define CPU_PRINTF 1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
