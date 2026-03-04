/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stdio.h"
#include "bsp_usart.h"
#include "bsp_i2c.h"
#include "i2c.h"
#include "bsp_spi_flash.h"
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
extern UART_HandleTypeDef huart1;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for myTask03 */
osThreadId_t myTask03Handle;
const osThreadAttr_t myTask03_attributes = {
  .name = "myTask03",
  .stack_size = 128 * 8,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Timer_Sys_Run */
osTimerId_t Timer_Sys_RunHandle;
const osTimerAttr_t Timer_Sys_Run_attributes = {
  .name = "Timer_Sys_Run"
};
/* Definitions for Mutex_USART1 */
osMutexId_t Mutex_USART1Handle;
const osMutexAttr_t Mutex_USART1_attributes = {
  .name = "Mutex_USART1"
};
/* Definitions for Mutex_SPI1 */
osMutexId_t Mutex_SPI1Handle;
const osMutexAttr_t Mutex_SPI1_attributes = {
  .name = "Mutex_SPI1"
};
/* Definitions for Event_USART1_RX */
osEventFlagsId_t Event_USART1_RXHandle;
const osEventFlagsAttr_t Event_USART1_RX_attributes = {
  .name = "Event_USART1_RX"
};
/* Definitions for Event_USART1_TX */
osEventFlagsId_t Event_USART1_TXHandle;
const osEventFlagsAttr_t Event_USART1_TX_attributes = {
  .name = "Event_USART1_TX"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);
void Sys_Run_Callback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of Mutex_USART1 */
  Mutex_USART1Handle = osMutexNew(&Mutex_USART1_attributes);

  /* creation of Mutex_SPI1 */
  Mutex_SPI1Handle = osMutexNew(&Mutex_SPI1_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of Timer_Sys_Run */
  Timer_Sys_RunHandle = osTimerNew(Sys_Run_Callback, osTimerPeriodic, NULL, &Timer_Sys_Run_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

  /* creation of myTask03 */
  myTask03Handle = osThreadNew(StartTask03, NULL, &myTask03_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* creation of Event_USART1_RX */
  Event_USART1_RXHandle = osEventFlagsNew(&Event_USART1_RX_attributes);

  /* creation of Event_USART1_TX */
  Event_USART1_TXHandle = osEventFlagsNew(&Event_USART1_TX_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask02 */
}

void Delay(__IO uint32_t nCount)
{
    for (; nCount != 0; nCount--);
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/


/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
    uint32_t id = 0;
  /* Infinite loop */
    for(;;)
    {
        id = spi_flash_read_id();
        debug_info("spi flash id : 0x%x\r\n", id);
        
        HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
        osDelay(1000);
    }
  /* USER CODE END StartTask03 */
}

/* Sys_Run_Callback function */
void Sys_Run_Callback(void *argument)
{
  /* USER CODE BEGIN Sys_Run_Callback */

  /* USER CODE END Sys_Run_Callback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

