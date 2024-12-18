#include "gpio.h"
#include "i2c_hal.h"
#include "delay.h"


void IIC_Init(){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
    
  __HAL_RCC_GPIOB_CLK_ENABLE();
    
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Set_SCL_Out_Mode(){
  GPIO_InitTypeDef GPIO_InitStruct = {0};
    
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void Set_SDA_Out_Mode(){    
  GPIO_InitTypeDef GPIO_InitStruct;
    
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
} 

void Set_SDA_In_Mode(){
  GPIO_InitTypeDef GPIO_InitStruct;
    
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

void IIC_Delay(void)    
{
  HAL_Delay_us(5);
}

void IIC_Start()
{
  SCL_H; 
  Set_SDA_Out_Mode();
  SDA_H;  
  IIC_Delay();
  SDA_L; 
  IIC_Delay();
}

void IIC_Stop()
{  
  IIC_Delay();
  Set_SDA_Out_Mode();
  SDA_L;
  SCL_H; 
  IIC_Delay();
  SDA_H;
  IIC_Delay();
}

unsigned char IIC_WaitAck()
{
  unsigned char s = 0;
  IIC_Delay();
  Set_SDA_In_Mode();  
  SCL_H;
  IIC_Delay();
  s = Read_SDA;
  SCL_L;    
  return s;   
}

void IIC_SendAck(unsigned char s)
{
  SCL_L;
  Set_SDA_Out_Mode();
  s ? SDA_H : SDA_L;
  SCL_H;
  IIC_Delay();
  SCL_L;
  SDA_H;
}

void IIC_WriteByte(unsigned char dat)
{
  unsigned char i;
  Set_SDA_Out_Mode();
  for(i=0; i<8; i++)
  {
    SCL_L;
    if(dat & 0x80){
      SDA_H;
    }else{
      SDA_L;
    }
    dat<<=1;        
    IIC_Delay();
        
    SCL_H;
    IIC_Delay();        
  }
  SCL_L;
}

unsigned char IIC_ReadByte() 
{
  unsigned char i, dat = 0;
  for(i=0; i<8; i++)
  {
    SCL_L;
    IIC_Delay();
    SCL_H;       
    Set_SDA_In_Mode();
    IIC_Delay();
    dat = (dat << 1) | (Read_SDA > 0 ? 1 : 0); 
  }
  SCL_L;
  Set_SDA_Out_Mode();
  SDA_H;
  return dat;
}