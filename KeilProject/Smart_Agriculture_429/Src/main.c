/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "OLED.h"
#include "DS18B20.h"
#include "delay.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_TIMEOUT 10000
#define TIMEOUT 500 //200
#define RX_BUFFER_SIZE 512

#define B1_PRESS 1
#define B2_PRESS 2
#define B3_PRESS 3
#define B4_PRESS 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rxBuffer[RX_BUFFER_SIZE];
uint16_t rxIndex = 0;
uint8_t rxData;  // 用于接收单个字节

uint8_t dataReceived = 0;

unsigned char Motor_flag = 0;
unsigned char percentRecv[3] = {0};
float threshold = 2.0;

char buffer[17] = {0};
char buffer2[17] = {0};
char buffer_Temperature[17] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
	{
		threshold = (100 - atoi((char*)percentRecv)) / 100.0 * 3.3;
		HAL_UART_Receive_IT(&huart1,percentRecv,2);
	}
	if(huart->Instance == USART2)
	{
		// 将接收到的字节放入接收缓冲区
		if (rxIndex < RX_BUFFER_SIZE - 1)
		{
			rxBuffer[rxIndex++] = rxData;
		
			// 如果接收到换行符或达到缓冲区上限，认为一个数据包完整
			if (rxData == '\n' || rxIndex == RX_BUFFER_SIZE - 1)
			{
					rxBuffer[rxIndex] = '\0';  // 添加字符串结束符
					dataReceived = 1;
					rxIndex = 0;  // 重置索引以便下一次接收
			}
		}
		else
		{
			// 防止溢出
			rxIndex = 0;
		}

		// 重新启动中断接收
		HAL_UART_Receive_IT(&huart2, &rxData, 1);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim2)
	{
		OLED_ShowString(1,1, (char *)buffer); 
		OLED_ShowString(2,1, (char *)buffer2); 
		OLED_ShowString(3,1, (char *)buffer_Temperature);
		float temperature = DS18B20_Get_Temp();
		snprintf(buffer_Temperature,sizeof(buffer_Temperature),"Temp: %.2f",temperature);
	}
}

void UART_Printf(const char *format, ...)	// 串口调试打印函数
{
    uint8_t tmp[128];
     
    va_list argptr;
    va_start(argptr, format);

    // 使用 vsnprintf 限制输出长度
    vsnprintf((char *)tmp, sizeof(tmp), format, argptr);

    va_end(argptr);

    // 发送数据
    HAL_UART_Transmit(&huart1, tmp, strlen((char *)tmp), HAL_MAX_DELAY);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MotorStart(float volt)
{
	// 使用静态变量存储上一次调用的时间
	static uint32_t LastStartTime = 0;
	uint32_t CurrentTime = HAL_GetTick();

	if(Motor_flag == 0)
	{
		if(CurrentTime > LastStartTime + 3000 && CurrentTime < LastStartTime + 10000)
			return;
		else if(CurrentTime > LastStartTime + 10000 && volt < threshold)
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
			LastStartTime = CurrentTime;
			Motor_flag = 1;
			return;
		}
		else 
			return;
	}
	else if(Motor_flag == 1)
	{
		if(CurrentTime < LastStartTime + 3000 )
			return;
		else if(CurrentTime > LastStartTime + 3000 )
		{
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
			Motor_flag = 0;
			return;
		}
	}
}

unsigned char Key_scan(void)
{    
    if((!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)||!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)||!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)||!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)))
    {
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET) return B1_PRESS;
        else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET) return B2_PRESS;
        else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET) return B3_PRESS;
        else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET) return B4_PRESS;
        else return 0;
    } 
    return 0;
}

void KeyProcess(void)
{    
    static uint32_t KeyScan_uwTick = 0;             //控制KeyProcess的执行速度
    static uint16_t Key_KeepTime=0;                 //记录按键按下持续的时间 
    static uint8_t keyvalue_Last=0;                 //记录上次按键键值
    uint8_t keyvalue_Current;
    if((uwTick -  KeyScan_uwTick)<50)	return;//减速函数    //uwTick为1ms的计数器 ,50ms扫描一次        
	KeyScan_uwTick = uwTick;
    
    keyvalue_Current=Key_scan();           
    switch(keyvalue_Current)
    {
        case B1_PRESS:
            Key_KeepTime++;
            if(Key_KeepTime==1)                 //第一次检测到按下，可认为是键按下的动作，单次按键在此处理
            {
               
            }
            else if(Key_KeepTime%4==0)          //每隔4*50ms=200ms持续动作，连续按键在此处理
            {
            
            }                
            break;
        case B2_PRESS:
            Key_KeepTime++;
            if(Key_KeepTime==1)                 //第一次按下该键
            {
                ;
            }
            break;
        case B3_PRESS:
            Key_KeepTime++;
            if(Key_KeepTime==1)                 //第一次按下该键
            {
                ;
            }
            break;
        case B4_PRESS:
            Key_KeepTime++;
            if(Key_KeepTime==1)                 //第一次按下该键
            {
                ;
            }
            break;
        default:
            if(Key_KeepTime>0)      //有键按下，现在松开了
            {
                switch(keyvalue_Last)
                {
                    case B1_PRESS:                      //该键松开
                        if(Key_KeepTime<10)             // 500ms以内为短键          
                        {
//                            LED_State=LED_State^0x02;
//                            LED_Control(LED_State);
                        }
                        else                            // 500ms以上为短键 
                        {
//                            LED_State=LED_State^0x04;
//                            LED_Control(LED_State);
                        }
                        break;
                    case B2_PRESS:                      //该键松开
                        if(Key_KeepTime<20)             // 500ms以内为短键          
                        {
                            ;
                        }
                        else                            // 500ms以上为短键 
                        {
                            ;
                        }
                        break;
                }
//                sprintf((char *)Lcd_Disp_String," ButtonValue=%2d",keyvalue_Last);
//                LCD_DisplayStringLine(Line3, (uint8_t *)Lcd_Disp_String);
            }
            Key_KeepTime=0;
            break;
    }
    keyvalue_Last=keyvalue_Current;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC3_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	/* 测试WiFi模块 */
	// 1. 设置ESP8266为STA + AP 模式
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_UART_Receive_IT(&huart1,percentRecv,2);
	HAL_TIM_Base_Start(&htim7);
	DS18B20_Init();
	OLED_Init();
	HAL_TIM_Base_Start_IT(&htim2);
  while (1)
  {
		KeyProcess(); 
		//启动ADC常规序列
		HAL_ADC_Start(&hadc3);
		//轮询等待常规序列转换完成
		HAL_ADC_PollForConversion(&hadc3,HAL_MAX_DELAY);
		
		//获取常规序列转换结果
		uint32_t result = HAL_ADC_GetValue(&hadc3);
		//result 范围是0 - 4096 正好对应电压是0-3.3V 
		float volt = result/4096.0 * 3.3;
		int percent =100 -  volt/3.3 * 100.0;
		
		//char buffer[17] = {0};
		snprintf(buffer,sizeof(buffer),"result:%.2fV ",volt);
		//OLED_ShowString(0,0, (uint8_t *)buffer, 15); 
		//char buffer2[17] = {0};
		snprintf(buffer2,sizeof(buffer2),"       %d%%     \n",percent);
		//OLED_ShowString(0,10, (uint8_t *)buffer2, 15); 
		HAL_UART_Transmit(&huart1,(uint8_t *)buffer,sizeof(buffer),1000);
		char buffer3[17] = {0};
		snprintf(buffer3,sizeof(buffer3),"threshold:%.2fV ",threshold);
		HAL_UART_Transmit(&huart1,(uint8_t *)buffer3,sizeof(buffer3),1000);
		HAL_UART_Transmit(&huart1,(uint8_t *)buffer2,sizeof(buffer2),1000);
		HAL_UART_Transmit(&huart1,(uint8_t *)buffer_Temperature,sizeof(buffer_Temperature),1000);
		MotorStart(volt);

		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		
		HAL_Delay(1000);
		if (dataReceived)
		{
			dataReceived = 0;
			UART_Printf((char *)rxBuffer);
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
