#include "delay.h"

// 微秒延迟函数，使用TIM7计时器
void delay_us(uint32_t us) 
{
    uint32_t startTick = __HAL_TIM_GET_COUNTER(&htim7);  // 获取当前计数器值
    uint32_t endTick = startTick + us;                    // 计算结束计数值
	
    // 处理计数器溢出的问题
    if (endTick >= 65536) 
    {
        endTick -= 65536; // 修正为溢出后的目标值
    }
		
    // 等待计数器达到目标值
    while (__HAL_TIM_GET_COUNTER(&htim7) != endTick) 
    {
        // 空循环，直到计数器达到目标值
    }
}
