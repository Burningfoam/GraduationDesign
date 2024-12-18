#include "oled.h"
#include "oledfont.h"

void OLED_WriteCom(u8 com)
{
  IIC_Start();

  IIC_WriteByte(0x78);
  IIC_WaitAck();
  IIC_WriteByte(0x00);
  IIC_WaitAck();
  IIC_WriteByte(com);
  IIC_WaitAck();
    
  IIC_Stop();
}

void OLED_WriteDat(u8 dat)
{
  IIC_Start();

  IIC_WriteByte(0x78);
  IIC_WaitAck();
  IIC_WriteByte(0x40);
  IIC_WaitAck();
  IIC_WriteByte(dat);
  IIC_WaitAck();
    
  IIC_Stop();
}
   
void OLED_Init(void)
{ 
  OLED_WriteCom(0xAE);//--display off
  OLED_WriteCom(0x00);//---set low column address
  OLED_WriteCom(0x10);//---set high column address
  OLED_WriteCom(0x40);//--set start line address 
  OLED_WriteCom(0xB0);//--set page address
  OLED_WriteCom(0x81); // contract control
  OLED_WriteCom(0xFF);//--1
  OLED_WriteCom(0xA1);//set segment remap 
  OLED_WriteCom(0xA6);//--normal / reverse
  OLED_WriteCom(0xA8);//--set multiplex ratio(1 to 64)
  OLED_WriteCom(0x3F);//--1/duty
  OLED_WriteCom(0xC8);//Com scan direction
  OLED_WriteCom(0xD3);//-set display offset
  OLED_WriteCom(0x00);//
    
  OLED_WriteCom(0xD5);//set osc division
  OLED_WriteCom(0x80);//
   
  OLED_WriteCom(0xD8);//set area color mode off
  OLED_WriteCom(0x05);//
    
  OLED_WriteCom(0xD9);//Set Pre-Charge Period
  OLED_WriteCom(0xF1);//
    
  OLED_WriteCom(0xDA);//set com pin configuartion
  OLED_WriteCom(0x12);//
    
  OLED_WriteCom(0xDB);//set Vcomh
  OLED_WriteCom(0x30);//
    
  OLED_WriteCom(0x8D);//set charge pump enable
  OLED_WriteCom(0x14);//
    
  OLED_WriteCom(0xAF);//--turn on oled panel
}

void OLED_SetPos(u8 x, u8 y) 
{ 
  OLED_WriteCom(0xb0+y);
  OLED_WriteCom(((x&0xf0)>>4)|0x10);
  OLED_WriteCom((x&0x0f)); 
}
 
void OLED_DisplayOn(void)
{
  OLED_WriteCom(0X8D); //SET DCDC
  OLED_WriteCom(0X14); //DCDC ON
  OLED_WriteCom(0XAF); //DISPLAY ON
}

void OLED_DisplayOff(void)
{
  OLED_WriteCom(0X8D); //SET DCDC
  OLED_WriteCom(0X10); //DCDC OFF
  OLED_WriteCom(0XAE); //DISPLAY OFF
}
 
void OLED_Clear(void) 
{ 
  u8 i,n;    
  for(i=0;i<8;i++) 
  { 
    OLED_WriteCom(0xb0+i);
    OLED_WriteCom(0x00);
    OLED_WriteCom(0x10);
    for(n=0;n<128;n++){
      OLED_WriteDat(0);
    }
  }
}

//x:0~127
//y:0~63               
//size: 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size)
{          
  unsigned char c=0,i=0;    
  c=chr-' ';     
  if(x>Max_Column-1){
    x=0;
    y=y+2;
  }
  if(Char_Size ==16)
  {
    OLED_SetPos(x,y);    
    for(i=0;i<8;i++)
    {
      OLED_WriteDat(F8X16[c*16+i]);
    }      
    OLED_SetPos(x,y+1);
    for(i=0;i<8;i++)
    {
      OLED_WriteDat(F8X16[c*16+i+8]);
    }    
  } else {    
    OLED_SetPos(x,y);
    for(i=0;i<6;i++)
    {
      OLED_WriteDat(F6X8[c][i]);        
    }  
  }
}

void OLED_ShowString(u8 x,u8 y,u8 *s,u8 Char_Size)
{
  while(*s){
    OLED_ShowChar(x,y,*s++,Char_Size);
    x+=8;
    if(x>120){
      x=0;
      y+=2;
    }
  }
}