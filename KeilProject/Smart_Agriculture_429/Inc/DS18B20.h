#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f4xx_hal.h"

// 初始化DS18B20
uint8_t DS18B20_Init(void);

// 获取温度值
float DS18B20_Get_Temp(void);

// 启动温度转换
void DS18B20_Start(void);

#endif // __DS18B20_H
