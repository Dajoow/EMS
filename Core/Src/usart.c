/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdarg.h>
#include <stdio.h>
#include "string.h"
#include "cmsis_os.h"
#include "at24cxx.h"
/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_uart4_rx;
DMA_HandleTypeDef hdma_uart5_rx;
DMA_HandleTypeDef hdma_usart2_rx;

/* UART4 init function */
void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 2000000;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */
 //初始话之后会进一次空闲中断，要先等待清除一次
	while(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_IDLE))   
		__HAL_UART_CLEAR_IDLEFLAG(&huart4);
 // 开启空闲中断
  __HAL_UART_ENABLE_IT(&huart4, UART_IT_IDLE);
  /* USER CODE END UART4_Init 2 */

}
/* UART5 init function */
void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 115200;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */
 //初始话之后会进一次空闲中断，要先等待清除一次
	while(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE))   
		__HAL_UART_CLEAR_IDLEFLAG(&huart5);
 // 开启空闲中断
  __HAL_UART_ENABLE_IT(&huart5, UART_IT_IDLE);
  /* USER CODE END UART5_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */
  if(bsmuSetting.RS485Bps == 0){
	huart2.Init.BaudRate = 115200;
  }
  if(bsmuSetting.RS485Bps == 1){
	huart2.Init.BaudRate = 57600;
  }
  if(bsmuSetting.RS485Bps == 2){
	huart2.Init.BaudRate = 38400;
  }
  if(bsmuSetting.RS485Bps == 3){
	huart2.Init.BaudRate = 19200;
  }
  if(bsmuSetting.RS485Bps == 4){
	huart2.Init.BaudRate = 9600;
  }
  if(bsmuSetting.RS485Bps == 5){
	huart2.Init.BaudRate = 4800;
  }
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
//  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
 //初始话之后会进一次空闲中断，要先等待清除一次
	while(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
 // 开启空闲中断
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspInit 0 */

  /* USER CODE END UART4_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART4 clock enable */
    __HAL_RCC_UART4_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**UART4 GPIO Configuration
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* UART4 DMA Init */
    /* UART4_RX Init */
    hdma_uart4_rx.Instance = DMA1_Stream1;
    hdma_uart4_rx.Init.Request = DMA_REQUEST_UART4_RX;
    hdma_uart4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart4_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart4_rx.Init.Mode = DMA_NORMAL;
    hdma_uart4_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart4_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart4_rx);

    /* UART4 interrupt Init */
    HAL_NVIC_SetPriority(UART4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspInit 1 */

  /* USER CODE END UART4_MspInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspInit 0 */

  /* USER CODE END UART5_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART5;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* UART5 clock enable */
    __HAL_RCC_UART5_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**UART5 GPIO Configuration
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART5;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* UART5 DMA Init */
    /* UART5_RX Init */
    hdma_uart5_rx.Instance = DMA1_Stream0;
    hdma_uart5_rx.Init.Request = DMA_REQUEST_UART5_RX;
    hdma_uart5_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_uart5_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_uart5_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_uart5_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_uart5_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_uart5_rx.Init.Mode = DMA_NORMAL;
    hdma_uart5_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_uart5_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_uart5_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_uart5_rx);

    /* UART5 interrupt Init */
    HAL_NVIC_SetPriority(UART5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspInit 1 */

  /* USER CODE END UART5_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* USART2 DMA Init */
    /* USART2_RX Init */
    hdma_usart2_rx.Instance = DMA1_Stream2;
    hdma_usart2_rx.Init.Request = DMA_REQUEST_USART2_RX;
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart2_rx);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==UART4)
  {
  /* USER CODE BEGIN UART4_MspDeInit 0 */

  /* USER CODE END UART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PC10     ------> UART4_TX
    PC11     ------> UART4_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_10|GPIO_PIN_11);

    /* UART4 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* UART4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART4_IRQn);
  /* USER CODE BEGIN UART4_MspDeInit 1 */

  /* USER CODE END UART4_MspDeInit 1 */
  }
  else if(uartHandle->Instance==UART5)
  {
  /* USER CODE BEGIN UART5_MspDeInit 0 */

  /* USER CODE END UART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_UART5_CLK_DISABLE();

    /**UART5 GPIO Configuration
    PC12     ------> UART5_TX
    PD2     ------> UART5_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* UART5 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* UART5 interrupt Deinit */
    HAL_NVIC_DisableIRQ(UART5_IRQn);
  /* USER CODE BEGIN UART5_MspDeInit 1 */

  /* USER CODE END UART5_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PD5     ------> USART2_TX
    PD6     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5|GPIO_PIN_6);

    /* USART2 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//使用之前要清零
Uart_BUFF uart5_buff __attribute__((at(0x30000000)));
Uart_BUFF uart4_buff __attribute__((at(0x30001000)));
Uart_BUFF uart2_buff __attribute__((at(0x30002000)));

extern osMutexId DebugUartMutexHandle;
//初始化打印信息
void Init_printf()
{
	memset(&uart4_buff, 0x00, sizeof(uart4_buff));
	memset(&uart5_buff, 0x00, sizeof(uart5_buff));
	memset(&uart2_buff, 0x00, sizeof(uart2_buff));
  //开启DMA 传输，（这个视个人需求而定要不要重新打开）
	if(HAL_UART_Receive_DMA(&huart4, uart4_buff.dma_buf, BUFFERSIZE) != HAL_OK)//main函数while(1)前，启动一次DMA接收
	{
			Error_Handler();
	}
	if(HAL_UART_Receive_DMA(&huart5, uart5_buff.dma_buf, BUFFERSIZE) != HAL_OK)//main函数while(1)前，启动一次DMA接收
	{
			Error_Handler();
	}
	if(HAL_UART_Receive_DMA(&huart2, uart2_buff.dma_buf, BUFFERSIZE) != HAL_OK)//main函数while(1)前，启动一次DMA接收
	{
			Error_Handler();
	}
}

//接收完一次数据后调用，准备接收下一帧数据
void ResetUart5REV()
{
		uart5_buff.recv_end_flag = 0;
		uart5_buff.recv_len = 0;
		memset(uart5_buff.recv_buf, 0x00, BUFFERSIZE);
		HAL_UART_Receive_DMA(&huart5, uart5_buff.dma_buf, BUFFERSIZE);
//		while(HAL_UART_Receive_DMA(&huart5, uart5_buff.dma_buf, BUFFERSIZE) != HAL_OK){}
}

//接收完一次数据后调用，准备接收下一帧数据
void ResetUart4REV()
{
		uart4_buff.recv_end_flag = 0;
		uart4_buff.recv_len = 0;
		memset(uart4_buff.recv_buf, 0x00, BUFFERSIZE);
		HAL_UART_Receive_DMA(&huart4, uart4_buff.dma_buf, BUFFERSIZE);
//		while(HAL_UART_Receive_DMA(&huart4, uart4_buff.dma_buf, BUFFERSIZE) != HAL_OK){}
}

//接收完一次数据后调用，准备接收下一帧数据
void ResetUart2REV()
{
		uart2_buff.recv_end_flag = 0;
		uart2_buff.recv_len = 0;
		memset(uart2_buff.recv_buf, 0x00, BUFFERSIZE);
		HAL_UART_Receive_DMA(&huart2, uart2_buff.dma_buf, BUFFERSIZE);
//		while(HAL_UART_Receive_DMA(&huart2, uart4_buff.dma_buf, BUFFERSIZE) != HAL_OK){}
}

// uart4打印调试信息
void Debug_printf(char *format, ...)
{
	uint16_t length = 0;
	va_list args;

	va_start(args, format);
	memset(uart4_buff.send_buf, 0x00, BUFFERSIZE);
	length = vsnprintf((char*)uart4_buff.send_buf, sizeof(uart4_buff.send_buf), (char*)format, args);
	length = length>=BUFFERSIZE?BUFFERSIZE:length;
	
	xSemaphoreTake(DebugUartMutexHandle, portMAX_DELAY);
	{
		for(int i = 0; i < length; i++)
		{
				while(huart4.gState != HAL_UART_STATE_READY){}
					HAL_UART_Transmit_IT(&huart4, (uint8_t*)&uart4_buff.send_buf[i], 1);
		}
	}
	xSemaphoreGive(DebugUartMutexHandle);  
	
	va_end(args);
}

// 重写printf函数，后续发送时间确定后加适当延时可以换成DMA发送
void Module4G_printf(char *format, ...)
{
	uint16_t length = 0;
	va_list args;

	va_start(args, format);
	memset(uart5_buff.send_buf, 0x00, BUFFERSIZE);
	length = vsnprintf((char*)uart5_buff.send_buf, sizeof(uart5_buff.send_buf), (char*)format, args);
	length = length>=BUFFERSIZE?BUFFERSIZE:length;
	for(int i = 0; i < length; i++)
	{
		  while(huart5.gState != HAL_UART_STATE_READY){}
	      HAL_UART_Transmit_IT(&huart5, (uint8_t*)&uart5_buff.send_buf[i], 1);
	}
	va_end(args);
}

//对应中断中的代码
//void UART4_IRQHandler(void)
//{
//  /* USER CODE BEGIN UART4_IRQn 0 */
//	uint16_t temp;
//  /* USER CODE END UART4_IRQn 0 */
//  HAL_UART_IRQHandler(&huart4);
//  /* USER CODE BEGIN UART4_IRQn 1 */
//	if(huart4.Instance == UART4)
//	{
//		// 如果串口接收完一帧数据，处于空闲状态（IDLE 中断已置位）
//		if(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_IDLE) != RESET)
//		{
//			// 重置 IDLE 位（读取 SR 和 DR 寄存器后即可重置）
//			__HAL_UART_CLEAR_IDLEFLAG(&huart4);
//			// 停止 DMA 传输，因为不停止的话拷贝数据起来就会容易造成数据缺失
//			HAL_UART_DMAStop(&huart4);
//			// 读取 CNDTR 寄存器，获取 DMA 中未传输的数据个数
//			temp = __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
//			// 获得接收数据的长度（缓冲区总长度 - 未传输的数据个数）
//			uart4_buff.recv_len = BUFFERSIZE - temp;
//			// 将已接收到的数据进行拷贝，防止数据覆盖造成丢失
//			memcpy(uart4_buff.recv_buf, uart4_buff.dma_buf, uart4_buff.recv_len);
//			// 接收完成标志置位
//			uart4_buff.recv_end_flag = 1;
//			// 因为前面停止了 DMA 传输，现在要重新打开（这个视个人需求而定要不要重新打开）
//			while(HAL_UART_Receive_DMA(&huart4, uart4_buff.dma_buf, BUFFERSIZE) != HAL_OK){}
//		}
//	}
//  /* USER CODE END UART4_IRQn 1 */
//}

//void UART5_DMA_Send(uint8_t *buffer, uint16_t length)
//{
//	//确保DMA
//	while(HAL_DMA_GetState(&hdma_uart5_tx) != HAL_DMA_STATE_READY || (&huart5)->gState != HAL_UART_STATE_READY){}   
//    //关闭DMA
//   __HAL_DMA_DISABLE(&hdma_uart5_tx);

//    //开始发送数据
//  if(HAL_UART_Transmit_DMA(&huart5, buffer, length)!= HAL_OK) //判断是否发送正常，如果出现异常则进入异常中断函数
//  {
//   Error_Handler();
//  }
//}
/* USER CODE END 1 */
