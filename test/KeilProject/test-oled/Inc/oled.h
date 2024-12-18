#ifndef __OLED_H
#define __OLED_H 
#include "stdint.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	OLED显示屏h文件                     
 * 版本			: V1.0
 * 日期			: 2024.8.7
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥 

**********************BEGIN***********************/

//----------------OLED端口定义----------------- 
/***************根据自己需求更改****************/
#define OLED_SCL_PROT  			GPIOB
#define OLED_SCL_PIN				GPIO_Pin_11
#define OLED_SCL_GPIO_CLK   RCC_APB2Periph_GPIOB
#define OLED_SDA_PROT  			GPIOB
#define OLED_SDA_PIN				GPIO_Pin_10
#define OLED_SDA_GPIO_CLK   RCC_APB2Periph_GPIOB
/*********************END**********************/

#define OLED_SCL_Clr() GPIO_ResetBits(OLED_SCL_PROT,OLED_SCL_PIN)//SCL
#define OLED_SCL_Set() GPIO_SetBits(OLED_SCL_PROT,OLED_SCL_PIN)

#define OLED_SDA_Clr() GPIO_ResetBits(OLED_SDA_PROT,OLED_SDA_PIN)//DIN
#define OLED_SDA_Set() GPIO_SetBits(OLED_SDA_PROT,OLED_SDA_PIN)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

void OLED_ClearPoint(uint8_t x,uint8_t y);
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_I2C_Start(void);
void OLED_I2C_Stop(void);
void OLED_I2C_WaitAck(void);
void OLED_Send_Byte(uint8_t dat);
void OLED_WR_Byte(uint8_t dat,uint8_t mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t t);
void OLED_DrawLine(uint8_t x1,uint8_t y1,uint8_t x2,uint8_t y2,uint8_t mode);
void OLED_DrawCircle(uint8_t x,uint8_t y,uint8_t r);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void OLED_ShowChar6x8(uint8_t x,uint8_t y,uint8_t chr,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t size1,uint8_t mode);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode);
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode);
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);
void OLED_Init(void);

#endif

