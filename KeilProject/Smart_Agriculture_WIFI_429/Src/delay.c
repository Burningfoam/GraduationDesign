#include "delay.h"
#include "tim.h"

void HAL_Delay_us(uint16_t nus)
{
    uint16_t diff = 0xffff - nus - 5;
    HAL_TIM_Base_Start(&htim7);
    __HAL_TIM_SetCounter(&htim7, diff);//set initial value
    while(diff < 0xffff - 5)
    {
        diff = __HAL_TIM_GetCounter(&htim7);
    }
    HAL_TIM_Base_Stop(&htim7);
}

void HAL_Delay_ms(uint16_t nms){
    while(nms--){
        HAL_Delay_us(1000);
    }
}