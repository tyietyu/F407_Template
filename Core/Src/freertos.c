/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_DateTypeDef GetData; // 获取日期结构
RTC_TimeTypeDef GetTime; // 获取时间结构

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern RTC_HandleTypeDef hrtc;
extern ADC_HandleTypeDef hadc1;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint16_t ADC1_Sample     = 0;
uint16_t ADC1_Value		 = 0;  
extern  uint8_t keyVal;
/* USER CODE END Variables */
/* Definitions for LEDTask */
osThreadId_t LEDTaskHandle;
const osThreadAttr_t LEDTask_attributes = {
  .name = "LEDTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for RTC_TASK */
osThreadId_t RTC_TASKHandle;
const osThreadAttr_t RTC_TASK_attributes = {
  .name = "RTC_TASK",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GUANGMIN_Task */
osThreadId_t GUANGMIN_TaskHandle;
const osThreadAttr_t GUANGMIN_Task_attributes = {
  .name = "GUANGMIN_Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for HCSR04Task */
osThreadId_t HCSR04TaskHandle;
const osThreadAttr_t HCSR04Task_attributes = {
  .name = "HCSR04Task",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for ESP8266TASK */
osThreadId_t ESP8266TASKHandle;
const osThreadAttr_t ESP8266TASK_attributes = {
  .name = "ESP8266TASK",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for KEYTASK */
osThreadId_t KEYTASKHandle;
const osThreadAttr_t KEYTASK_attributes = {
  .name = "KEYTASK",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void ledTask(void *argument);
void rtcTask(void *argument);
void guangMinTask(void *argument);
void hcsr04Task(void *argument);
void esp8266Task(void *argument);
void keyTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

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
  /* creation of LEDTask */
  LEDTaskHandle = osThreadNew(ledTask, NULL, &LEDTask_attributes);

  /* creation of RTC_TASK */
  RTC_TASKHandle = osThreadNew(rtcTask, NULL, &RTC_TASK_attributes);

  /* creation of GUANGMIN_Task */
  GUANGMIN_TaskHandle = osThreadNew(guangMinTask, NULL, &GUANGMIN_Task_attributes);

  /* creation of HCSR04Task */
  HCSR04TaskHandle = osThreadNew(hcsr04Task, NULL, &HCSR04Task_attributes);

  /* creation of ESP8266TASK */
  ESP8266TASKHandle = osThreadNew(esp8266Task, NULL, &ESP8266TASK_attributes);

  /* creation of KEYTASK */
  KEYTASKHandle = osThreadNew(keyTask, NULL, &KEYTASK_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_ledTask */
/**
 * @brief  Function implementing the LEDTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_ledTask */
void ledTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN ledTask */
    /* Infinite loop */
    for (;;) {
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_SET);
        osDelay(100);
        HAL_GPIO_WritePin(LED0_GPIO_Port, LED0_Pin, GPIO_PIN_RESET);
        osDelay(400);
    }
  /* USER CODE END ledTask */
}

/* USER CODE BEGIN Header_rtcTask */
/**
 * @brief Function implementing the RTC_TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_rtcTask */
void rtcTask(void *argument)
{
  /* USER CODE BEGIN rtcTask */
    /* Infinite loop */
    for (;;) {
        /* Get the RTC current Time */
        HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);
        /* Get the RTC current Date */
        HAL_RTC_GetDate(&hrtc, &GetData, RTC_FORMAT_BIN);

        /* Display date Format : yy/mm/dd */
        printf("%02d/%02d/%02d\r\n", 2000 + GetData.Year, GetData.Month, GetData.Date);
        /* Display time Format : hh:mm:ss */
        printf("%02d:%02d:%02d\r\n", GetTime.Hours, GetTime.Minutes, GetTime.Seconds);
        printf("\r\n");
        osDelay(1000);
    }
  /* USER CODE END rtcTask */
}

/* USER CODE BEGIN Header_guangMinTask */
/**
 * @brief Function implementing the GUANGMIN_Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_guangMinTask */
void guangMinTask(void *argument)
{
  /* USER CODE BEGIN guangMinTask */
    /* Infinite loop */
    for (;;) {
        HAL_ADC_Start(&hadc1);
        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
            ADC1_Sample = HAL_ADC_GetValue(&hadc1);
            ADC1_Value  = ADC1_Sample * 330 / 4096;
        }
        printf("Light intensity: %d.%d%d\n", ADC1_Value / 100, (ADC1_Value / 10 % 10), ADC1_Value % 10);
        HAL_ADC_Stop(&hadc1);
        osDelay(500);
    }
  /* USER CODE END guangMinTask */
}

/* USER CODE BEGIN Header_hcsr04Task */
/**
 * @brief Function implementing the HCSR04Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_hcsr04Task */
void hcsr04Task(void *argument)
{
  /* USER CODE BEGIN hcsr04Task */
    /* Infinite loop */
    for (;;) {
        float distant = 0;
        distant       = read_len();
        printf("radarDistant: %.f cm\r\n, ", distant);
        osDelay(200);
    }
  /* USER CODE END hcsr04Task */
}

/* USER CODE BEGIN Header_esp8266Task */
/**
 * @brief Function implementing the ESP8266TASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_esp8266Task */
void esp8266Task(void *argument)
{
  /* USER CODE BEGIN esp8266Task */
    /* Infinite loop */
    for (;;) {
        osDelay(1);
    }
  /* USER CODE END esp8266Task */
}

/* USER CODE BEGIN Header_keyTask */
/**
 * @brief Function implementing the KEYTASK thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_keyTask */
void keyTask(void *argument)
{
  /* USER CODE BEGIN keyTask */

    /* Infinite loop */
    for (;;) {
        if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) {
            osDelay(10);
            if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) {
                keyVal++;
            }
        }
        if (keyVal == 5) {
            keyVal = 0;
        }
    }
  /* USER CODE END keyTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

