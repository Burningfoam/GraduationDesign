#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "OLED.h"
#include "ds18b20.h"
#include "string.h" 	

/*****************���絥Ƭ�����******************
											STM32
 * ��Ŀ			:	DS18B20�����¶ȴ�������ȡʵ��                   
 * �汾			: V1.0
 * ����			: 2024.8.13
 * MCU			:	STM32F103C8T6
 * �ӿ�			:	��ds18b20.h							
 * BILIBILI	:	���絥Ƭ�����
 * CSDN			:	���絥Ƭ�����
 * ����			:	����

**********************BEGIN***********************/

int main(void)
{	
	unsigned char p[16]=" ";

	short temperature = 0; 				//�¶�ֵ
	delay_init(72);	  
  LED_Init();		  				//��ʼ��������豸���ӵ�Ӳ���ӿ�
	OLED_Init();					//OLED��ʼ��
	delay_ms(50);
	OLED_Clear();						//����
	//��ʾ���¶ȣ���
	OLED_ShowChinese(0,0,0,16,1);
	OLED_ShowChinese(16,0,1,16,1);
	OLED_ShowChar(40,0,':',16,1);

	while(DS18B20_Init())	//DS18B20��ʼ��	
	{
		OLED_ShowString(0,0,"DS18B20 Error",16,1);
		delay_ms(200);
		OLED_ShowString(60,0,"        " ,16,1);	
		delay_ms(200);
	}
	delay_ms(1000);
	USART1_Config();//���ڳ�ʼ��
	
 	while(1)
	{	

			temperature=DS18B20_Get_Temp();	//��ȡ�¶�

			printf("T:%4.1f \r\n",(float)temperature/10);	//���ڷ��ͳ�ȥ
			sprintf((char*)p,"%4.1f    ",(float)temperature/10);
			OLED_ShowString(60,0,p ,16,1);
		
//			OLED_ShowChar(60,16,temperature/100+'0',16,1);
//			OLED_ShowChar(68,16,temperature%100/10+'0',16,1);
//			OLED_ShowChar(76,16,'.',16,1);
//			OLED_ShowChar(84,16,temperature%10+'0',16,1);
		
			delay_ms(100);
	}	
}
