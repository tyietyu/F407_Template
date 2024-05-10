/*
 * @Author       : lichaung
 * @Date         : 2024-04-25 
 * @LastEditors  : yzy
 * @LastEditTime : 2024-04-25
 * @Description  :
 * @FilePath     : hc-sr04.h
 */
#ifndef HCSR04_H_
#define HCSR04_H_

#include "main.h"
#include "cmsis_os2.h"

#define TRIG_SET      HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET)
#define TRIG_RESET    HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET)

typedef enum run_state 
{
	HCSR04_RESET=0,
	RISING,
	FALLING,
	OVER
}RUN_STATE;


extern void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

float read_len(void);

#endif /* HCSR04_H_ */


