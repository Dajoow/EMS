/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart4;

extern UART_HandleTypeDef huart5;

extern UART_HandleTypeDef huart2;

/* USER CODE BEGIN Private defines */
#define BUFFERSIZE 		1024				// 缓冲区大小（一次性最大发送、接收256字节，多出的舍弃）
typedef struct _UART_BUFF_ {
	uint8_t  recv_end_flag;				// 接收完成标志
	uint8_t  send_buf[BUFFERSIZE];		// 发送缓冲区
  uint8_t  recv_buf[BUFFERSIZE];		// 接收缓冲区
	uint8_t  dma_buf[BUFFERSIZE];
	uint16_t recv_len;					// 接收数据的长度
} Uart_BUFF;
 
extern Uart_BUFF uart5_buff;
extern Uart_BUFF uart4_buff;
extern Uart_BUFF uart2_buff;
/* USER CODE END Private defines */

void MX_UART4_Init(void);
void MX_UART5_Init(void);
void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void Init_printf(void);
void ResetUart5REV(void);
void ResetUart4REV(void);
void ResetUart2REV(void);
void Debug_printf(const char *format, ...);
void cmb_printf(const char *format, ...);
void Module4G_printf(char *format, ...);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

