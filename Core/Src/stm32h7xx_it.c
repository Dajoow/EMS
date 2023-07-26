/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "usart.h"
#include "AT_module_4g.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ETH_HandleTypeDef heth;
extern DMA2D_HandleTypeDef hdma2d;
extern FDCAN_HandleTypeDef hfdcan2;
extern LTDC_HandleTypeDef hltdc;
extern TIM_HandleTypeDef htim6;
extern DMA_HandleTypeDef hdma_uart4_rx;
extern DMA_HandleTypeDef hdma_uart5_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim7;

/* USER CODE BEGIN EV */
extern osThreadId modbus_taskhandle;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  HAL_RCC_NMI_IRQHandler();
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

  /* USER CODE END DMA1_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart5_rx);
  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart4_rx);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream2 global interrupt.
  */
void DMA1_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream2_IRQn 0 */

  /* USER CODE END DMA1_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Stream2_IRQn 1 */

  /* USER CODE END DMA1_Stream2_IRQn 1 */
}

/**
  * @brief This function handles FDCAN2 interrupt 0.
  */
void FDCAN2_IT0_IRQHandler(void)
{
  /* USER CODE BEGIN FDCAN2_IT0_IRQn 0 */

  /* USER CODE END FDCAN2_IT0_IRQn 0 */
  HAL_FDCAN_IRQHandler(&hfdcan2);
  /* USER CODE BEGIN FDCAN2_IT0_IRQn 1 */

  /* USER CODE END FDCAN2_IT0_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	uint16_t temp;
  BaseType_t xHigherPriorityTaskWoken = pdTRUE;
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
  if(huart2.Instance == USART2)
  {
    // 如果串口接收完一帧数据，处于空闲状态（IDLE 中断已置位）
    if(__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE) != RESET)
    {
      // 重置 IDLE 位（读取 SR 和 DR 寄存器后即可重置）
      __HAL_UART_CLEAR_IDLEFLAG(&huart2);
      // 停止 DMA 传输，因为不停止的话拷贝数据起来就会容易造成数据缺失
      HAL_UART_DMAStop(&huart2);
      // 读取 CNDTR 寄存器，获取 DMA 中未传输的数据个数
      temp = __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
      // 获得接收数据的长度（缓冲区总长度 - 未传输的数据个数）
      uart2_buff.recv_len = BUFFERSIZE - temp;
      // 将已接收到的数据进行拷贝，防止数据覆盖造成丢失
      memcpy(uart2_buff.recv_buf, uart2_buff.dma_buf, uart2_buff.recv_len);
      // 接收完成标志置位
      uart2_buff.recv_end_flag = 1;
      if(modbus_taskhandle != NULL){
        vTaskNotifyGiveFromISR(modbus_taskhandle, &xHigherPriorityTaskWoken);
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
        should be performed to ensure the interrupt returns directly to the highest
        priority task.  The macro used for this purpose is dependent on the port in
        use and may be called portEND_SWITCHING_ISR(). */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      // 因为前面停止了 DMA 传输，现在要重新打开（这个视个人需求而定要不要重新打开）//现在放在数据处理中
  //			while(HAL_UART_Receive_DMA(&huart2, uart2_buff.dma_buf, BUFFERSIZE) != HAL_OK){}
    }
  }  
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */
  uint16_t temp;
  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */
  if(huart4.Instance == UART4)
  {
    // 如果串口接收完一帧数据，处于空闲状态（IDLE 中断已置位）
    if(__HAL_UART_GET_FLAG(&huart4, UART_FLAG_IDLE) != RESET)
    {
      // 重置 IDLE 位（读取 SR 和 DR 寄存器后即可重置）
      __HAL_UART_CLEAR_IDLEFLAG(&huart4);
      // 停止 DMA 传输，因为不停止的话拷贝数据起来就会容易造成数据缺失
      HAL_UART_DMAStop(&huart4);
      // 读取 CNDTR 寄存器，获取 DMA 中未传输的数据个数
      temp = __HAL_DMA_GET_COUNTER(&hdma_uart4_rx);
      // 获得接收数据的长度（缓冲区总长度 - 未传输的数据个数）
      uart4_buff.recv_len = BUFFERSIZE - temp;
      // 将已接收到的数据进行拷贝，防止数据覆盖造成丢失
      memcpy(uart4_buff.recv_buf, uart4_buff.dma_buf, uart4_buff.recv_len);
      // 接收完成标志置位
      uart4_buff.recv_end_flag = 1;
      // 因为前面停止了 DMA 传输，现在要重新打开（这个视个人需求而定要不要重新打开）//现在放在数据处理中
  //			while(HAL_UART_Receive_DMA(&huart4, uart4_buff.dma_buf, BUFFERSIZE) != HAL_OK){}
    }
  }  
  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */
  uint16_t temp;
  char* dmabBufStart;
  /* USER CODE END UART5_IRQn 0 */
  HAL_UART_IRQHandler(&huart5);
  /* USER CODE BEGIN UART5_IRQn 1 */
	if(huart5.Instance == UART5)
	{
		// 如果串口接收完一帧数据，处于空闲状态（IDLE 中断已置位）
    if(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_IDLE) != RESET)
    {
      // 重置 IDLE 位（读取 SR 和 DR 寄存器后即可重置）
      __HAL_UART_CLEAR_IDLEFLAG(&huart5);
      // 停止 DMA 传输，因为不停止的话拷贝数据起来就会容易造成数据缺失
      HAL_UART_DMAStop(&huart5);
      // 读取 CNDTR 寄存器，获取 DMA 中未传输的数据个数
      temp = __HAL_DMA_GET_COUNTER(&hdma_uart5_rx);
      // 获得接收数据的长度（缓冲区总长度 - 未传输的数据个数）
      uart5_buff.recv_len = BUFFERSIZE - temp;
      temp = 0;
      // 将已接收到的数据进行拷贝，防止数据覆盖造成丢失
      dmabBufStart = (char*)uart5_buff.dma_buf;
      while (*dmabBufStart == 0x00 && temp < BUFFERSIZE)
      {
        dmabBufStart++;
        temp++;
      }
      memcpy(uart5_buff.recv_buf, dmabBufStart, uart5_buff.recv_len);
      // 接收完成标志置位
      uart5_buff.recv_end_flag = 1;
      // 因为前面停止了 DMA 传输，现在要重新打开（这个视个人需求而定要不要重新打开）//现在放在数据处理中
      //			while(HAL_UART_Receive_DMA(&huart5, uart5_buff.dma_buf, BUFFERSIZE) != HAL_OK){}
    }
  }
  /* USER CODE END UART5_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1_CH1 and DAC1_CH2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles Ethernet global interrupt.
  */
void ETH_IRQHandler(void)
{
  /* USER CODE BEGIN ETH_IRQn 0 */

  /* USER CODE END ETH_IRQn 0 */
  HAL_ETH_IRQHandler(&heth);
  /* USER CODE BEGIN ETH_IRQn 1 */

  /* USER CODE END ETH_IRQn 1 */
}

/**
  * @brief This function handles LTDC global interrupt.
  */
void LTDC_IRQHandler(void)
{
  /* USER CODE BEGIN LTDC_IRQn 0 */

  /* USER CODE END LTDC_IRQn 0 */
  HAL_LTDC_IRQHandler(&hltdc);
  /* USER CODE BEGIN LTDC_IRQn 1 */

  /* USER CODE END LTDC_IRQn 1 */
}

/**
  * @brief This function handles DMA2D global interrupt.
  */
void DMA2D_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2D_IRQn 0 */

  /* USER CODE END DMA2D_IRQn 0 */
  HAL_DMA2D_IRQHandler(&hdma2d);
  /* USER CODE BEGIN DMA2D_IRQn 1 */

  /* USER CODE END DMA2D_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
