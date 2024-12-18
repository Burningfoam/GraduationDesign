#ifndef __OLED_H
#define __OLED_H

#include "i2c_hal.h"

#define Max_Column 128

typedef unsigned char  u8 ;
typedef unsigned short u16 ;
typedef unsigned int   u32 ;
 
void OLED_Init(void);

void OLED_WriteCom(u8 com);

void OLED_WriteDat(u8 dat);

void OLED_SetPos(u8 x, u8 y) ;
 
void OLED_DisplayOn(void);

void OLED_DisplayOff(void);

void OLED_Clear(void) ;

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);

void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 Char_Size);

#endif