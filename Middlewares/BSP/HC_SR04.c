#include "HC_SR04.h"

RUN_STATE state;
uint16_t Buff[2] = {0};
extern TIM_HandleTypeDef htim4;

float read_len(void)
{
    static float len;
    TRIG_RESET;
    if (state == HCSR04_RESET) {
        TRIG_SET;
        delay_us(20);
        TRIG_RESET;
        __HAL_TIM_SET_CAPTUREPOLARITY(&htim4, TIM_CHANNEL_1,
                                      TIM_INPUTCHANNELPOLARITY_RISING);
        HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
        state = RISING;
    }
    if (state == OVER) {
        len   = (float)(Buff[1] - Buff[0]) * 0.017f;
        state = HCSR04_RESET;
    }
    return len;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) //
{

    if (TIM4 == htim->Instance) 
	{
        if (state == RISING) {
            __HAL_TIM_SET_COUNTER(&htim4, 0);
            Buff[0] = __HAL_TIM_GetCounter(&htim4);

            __HAL_TIM_SET_CAPTUREPOLARITY(&htim4, TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
            state = FALLING;
        } else if (state == FALLING) {
            Buff[1] = __HAL_TIM_GetCounter(&htim4); // 获取下降沿时的CNT值
            state   = OVER;                         // 设置标志为完成
        }
    }
}
