/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include "HC_SR04.h"
#include "usDelay.h"
#include "wifi.h"
#include "gpio.h"
#include "usart.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "mqtt.h"
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
#define KEY0_Pin GPIO_PIN_0
#define KEY0_GPIO_Port GPIOA
#define GUANG_MIN_Pin GPIO_PIN_1
#define GUANG_MIN_GPIO_Port GPIOA
#define WIFI_TX_Pin GPIO_PIN_2
#define WIFI_TX_GPIO_Port GPIOA
#define WIFI_RX_Pin GPIO_PIN_3
#define WIFI_RX_GPIO_Port GPIOA
#define TRIG_Pin GPIO_PIN_4
#define TRIG_GPIO_Port GPIOC
#define LED0_Pin GPIO_PIN_2
#define LED0_GPIO_Port GPIOB
#define WIFI_RST_Pin GPIO_PIN_11
#define WIFI_RST_GPIO_Port GPIOD
#define ECHO_Pin GPIO_PIN_12
#define ECHO_GPIO_Port GPIOD
#define PWM_DUOJI_Pin GPIO_PIN_6
#define PWM_DUOJI_GPIO_Port GPIOC
#define SD_CARD_DET_Pin GPIO_PIN_3
#define SD_CARD_DET_GPIO_Port GPIOD
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
