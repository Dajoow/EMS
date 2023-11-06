/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AT_module_4g.h"
#include "CAN_Control.h"
#include "RS485.h"
#include "client.h"
#include "fdcan.h"
#include "gt911.h"
#include "http_client.h"
#include "lcd.h"
#include "sntp_client.h"
#include "station_ctl.h"
#include "stdint.h"
#include "usart.h"
#include "at24cxx.h"
#include "data_persistence.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern TIM_HandleTypeDef htim6;
extern osThreadId Client_threadHandle;
//osThreadId TCP_Send_threadHandle;
extern osThreadId TCP_Recv_threadHandle;
extern osMessageQId CANQueueHandle[cluster_num];
extern osThreadId CAN_Rev_TaskHandle;
extern osThreadId CAN_Poll_TaskHandle;
extern osSemaphoreId ETHSndSemHandle;
extern osThreadId httpc_handle;
extern osThreadId modbus_taskhandle;

osThreadId CPU_Task_ThreadHandle;
/* USER CODE END Variables */
osThreadId startup_ThreadHandle;
osThreadId TouchGFXHandle;
osMutexId DebugUartMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void CPU_Task (void const *argument);
/* USER CODE END FunctionPrototypes */

void Startup(void const * argument);
void TouchGFX_Task(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
extern uint32_t g_osRuntimeCounter;
__weak void
configureTimerForRunTimeStats (void)
{
  g_osRuntimeCounter = 0;
}

__weak unsigned long
getRunTimeCounterValue (void)
{
  return g_osRuntimeCounter;
}
/* USER CODE END 1 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void
vApplicationGetIdleTaskMemory (StaticTask_t **ppxIdleTaskTCBBuffer,
                               StackType_t **ppxIdleTaskStackBuffer,
                               uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void
vApplicationGetTimerTaskMemory (StaticTask_t **ppxTimerTaskTCBBuffer,
                                StackType_t **ppxTimerTaskStackBuffer,
                                uint32_t *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of DebugUartMutex */
  osMutexDef(DebugUartMutex);
  DebugUartMutexHandle = osMutexCreate(osMutex(DebugUartMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */

  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of startup_Thread */
  osThreadDef(startup_Thread, Startup, osPriorityRealtime, 0, 256);
  startup_ThreadHandle = osThreadCreate(osThread(startup_Thread), NULL);

  /* definition and creation of TouchGFX */
  osThreadDef(TouchGFX, TouchGFX_Task, osPriorityBelowNormal, 0, 3072);
  TouchGFXHandle = osThreadCreate(osThread(TouchGFX), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Startup */
/**
 * @brief  Function implementing the startup_Thread thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_Startup */
void Startup(void const * argument)
{
  /* USER CODE BEGIN Startup */
  StationDataInit();
  flash_save_init();
  mbedtls_net_init (NULL);
  sntp_client_init ();
  // 开启CAN接收线程
  BSMU_CANInit ();
  // 开启4G模块线程
  Module4G_Init ();

  modbus_init ();

  if (bsmuSetting.local_flag){
  // 开启客户端线程
    ClientInit ();
  }

  if (bsmuSetting.yunduan_flag){
    http_client_init ();
  }

  // 开启CPU_Task线程
  osThreadDef (CPU_Task_Thread, CPU_Task, osPriorityIdle, 0, 256);
  CPU_Task_ThreadHandle = osThreadCreate (osThread (CPU_Task_Thread), NULL);
#if TOUCHGFX_ENABLE
  vTaskDelay (200); // 延迟200ms再开启背光
  LCD_BL (1);       // 开启背光
#endif
  vTaskDelete (startup_ThreadHandle); // 删除该任务
  /* Infinite loop */
  for (;;)
    {
      osDelay (1);
    }
  /* USER CODE END Startup */
}

/* USER CODE BEGIN Header_TouchGFX_Task */
/**
 * @brief Function implementing the TouchGFX thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_TouchGFX_Task */
__weak void TouchGFX_Task(void const * argument)
{
  /* USER CODE BEGIN TouchGFX_Task */
  /* Infinite loop */
  for (;;)
    {
      osDelay (1);
    }
  /* USER CODE END TouchGFX_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE BEGIN Header_CPU_Task */
/**
 * @brief Function implementing the CPU_Task_Thread thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_CPU_Task */
void
CPU_Task (void const *argument)
{
  /* USER CODE BEGIN CPU_Task */
  /* Infinite loop */
  for (;;)
    {
#if CPU_PRINTF
      char *pbuffer = (char *)malloc(300* sizeof (char));
      vTaskList (pbuffer);
      Debug_printf ("----------------------------------------------\r\n");
      Debug_printf ("Task_name         state    priority     remaining_stack  "
                    "Task_Num\r\n");
      Debug_printf ("%s", pbuffer);
      memset (pbuffer, 0, sizeof (char) * 200);
      Debug_printf ("----------------------------------------------\r\n");
      Debug_printf ("Task_name   Running count                 usage\r\n");
      vTaskGetRunTimeStats (pbuffer);
      Debug_printf ("%s", pbuffer);
      free(pbuffer);
      Debug_printf ("----------------------------------------------\r\n");
      Debug_printf ("FreeHeapSize:%d\n", xPortGetFreeHeapSize ());
      Debug_printf ("MiniFreeHeapSize:%d\n",
                    xPortGetMinimumEverFreeHeapSize ());
      Debug_printf ("CPU_Task_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (CPU_Task_ThreadHandle));
      Debug_printf ("GFX_Task_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (TouchGFXHandle));
      Debug_printf ("Client_Task_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (Client_threadHandle));
      Debug_printf ("TCP_Rev_Task_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (TCP_Recv_threadHandle));
      Debug_printf ("CAN_Rev_Thread_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (CAN_Rev_TaskHandle));
      Debug_printf ("CAN_Poll_Thread_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (CAN_Poll_TaskHandle));
      Debug_printf ("Module4G_Process_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (Module4G_TaskHandle));
      Debug_printf ("modbus_Process_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (modbus_taskhandle));
      Debug_printf ("https_Process_minStack:%d\n",
                    (int)uxTaskGetStackHighWaterMark (httpc_handle));
      Debug_printf ("----------------------------------------------\r\n");
      Debug_printf (
          "Queue_name      QueueMessagesWaiting   QueueSpacesAvailable\r\n");
      for (uint8_t i = 0; i < cluster_num; i++)
        Debug_printf ("CANQueue%d       %d                       %d\n", i + 1,
                      (int)uxQueueMessagesWaiting (CANQueueHandle[i]),
                      (int)uxQueueSpacesAvailable (CANQueueHandle[i]));

        // 测试485
//	RS485_REV();
#endif
      HAL_GPIO_TogglePin (LED_G_GPIO_Port, LED_G_Pin);
      modelToViewData.frameRate = modelToViewData.frameRateCount;
      modelToViewData.frameRateCount = 0;
      vTaskDelay (1000);
    }
  //    osDelay(1);
  /* USER CODE END CPU_Task */
}
/* USER CODE END Application */
