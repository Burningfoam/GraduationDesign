#ifndef __I2C_HAL_H
#define __I2C_HAL_H

#include "main.h"

#define SCL_H (HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)) 
#define SCL_L (HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)) 

#define SDA_H (HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)) 
#define SDA_L (HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)) 

#define Read_SDA (GPIOB->IDR & (1<<7))

void Set_SCL_Out_Mode(void);

void Set_SDA_Out_Mode(void);

void Set_SDA_In_Mode(void);

void IIC_Delay(void);

void IIC_Start(void);

void IIC_Stop(void);

unsigned char IIC_WaitAck(void);

void IIC_SendAck(unsigned char s);

void IIC_WriteByte(unsigned char dat);

unsigned char IIC_ReadByte(void);

void IIC_Init(void);

#endif