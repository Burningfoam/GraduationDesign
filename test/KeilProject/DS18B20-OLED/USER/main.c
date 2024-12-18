#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "OLED.h"
#include "ds18b20.h"
#include "string.h" 	

/*****************辰哥单片机设计******************
											STM32
 * 项目			:	DS18B20数字温度传感器读取实验                   
 * 版本			: V1.0
 * 日期			: 2024.8.13
 * MCU			:	STM32F103C8T6
 * 接口			:	见ds18b20.h							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/

int main(void)
{	
	unsigned char p[16]=" ";

	short temperature = 0; 				//温度值
	delay_init(72);	  
  LED_Init();		  				//初始化与控制设备连接的硬件接口
	OLED_Init();					//OLED初始化
	delay_ms(50);
	OLED_Clear();						//清屏
	//显示“温度：”
	OLED_ShowChinese(0,0,0,16,1);
	OLED_ShowChinese(16,0,1,16,1);
	OLED_ShowChar(40,0,':',16,1);

	while(DS18B20_Init())	//DS18B20初始化	
	{
		OLED_ShowString(0,0,"DS18B20 Error",16,1);
		delay_ms(200);
		OLED_ShowString(60,0,"        " ,16,1);	
		delay_ms(200);
	}
	delay_ms(1000);
	USART1_Config();//串口初始化
	
 	while(1)
	{	

			temperature=DS18B20_Get_Temp();	//读取温度

			printf("T:%4.1f \r\n",(float)temperature/10);	//串口发送出去
			sprintf((char*)p,"%4.1f    ",(float)temperature/10);
			OLED_ShowString(60,0,p ,16,1);
		
//			OLED_ShowChar(60,16,temperature/100+'0',16,1);
//			OLED_ShowChar(68,16,temperature%100/10+'0',16,1);
//			OLED_ShowChar(76,16,'.',16,1);
//			OLED_ShowChar(84,16,temperature%10+'0',16,1);
		
			delay_ms(100);
	}	
}
