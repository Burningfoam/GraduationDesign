#include "DS18B20.h"
#include "delay.h"

/**************************************************************************************
 * 描  述 : 配置DS18B20用到的I/O口
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
static void DS18B20_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOE_CLK_ENABLE();  // 启用GPIOE时钟

    GPIO_InitStruct.Pin = GPIO_PIN_2;    // PE2
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;  // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  // 高速
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);  // 初始化GPIOE Pin2

    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);  // 设置为高电平
}

/**************************************************************************************
 * 描  述 : 配置使DS18B20-DATA引脚变为输入模式
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
static void DS18B20_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;  // 输入模式
    GPIO_InitStruct.Pull = GPIO_PULLUP;      // 上拉输入
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);  // 初始化GPIOE Pin2为输入
}

/**************************************************************************************
 * 描  述 : 配置使DS18B20-DATA引脚变为输出模式
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
static void DS18B20_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;          // 无上下拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;  // 高速
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);  // 初始化GPIOE Pin2为输出
}

/**************************************************************************************
 * 描  述 : 主机给从机发送复位脉冲
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
static void DS18B20_Rst(void)
{
    DS18B20_Mode_Out_PP();     // 主机设置为推挽输出

    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);  // 产生低电平复位信号
    delay_us(750);  // 750us的低电平
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);    // 拉高总线
    delay_us(15);    // 等待从机发送存在脉冲
}

/**************************************************************************************
 * 描  述 : 检测从机给主机返回的存在脉冲
 * 入  参 : 无
 * 返回值 : 0：成功   1：失败
 **************************************************************************************/
static uint8_t DS18B20_Presence(void)
{
    uint8_t pulse_time = 0;

    DS18B20_Mode_IPU();    // 设置为上拉输入

    // 等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号
    while (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) && pulse_time < 100)
    {
        pulse_time++;
        delay_us(1);
    }

    if (pulse_time >= 100)
        return 1;  // 超时，读取失败

    pulse_time = 0;

    while (!HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) && pulse_time < 240)
    {
        pulse_time++;
        delay_us(1);
    }

    if (pulse_time >= 240)
        return 1;  // 超时，读取失败

    return 0;  // 成功
}

/**************************************************************************************
 * 描  述 : 从DS18B20读取一个bit
 * 入  参 : 无
 * 返回值 : uint8_t 
 **************************************************************************************/
static uint8_t DS18B20_Read_Bit(void)
{
    uint8_t dat;

    DS18B20_Mode_Out_PP();

    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);  // 产生低电平信号
    delay_us(10);  // 10us低电平时间

    DS18B20_Mode_IPU();  // 设置为输入

    if (HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2) == GPIO_PIN_SET)
        dat = 1;
    else
        dat = 0;

    delay_us(45);  // 等待稳定

    return dat;
}

/**************************************************************************************
 * 描  述 : 从DS18B20读一个字节，低位先行
 * 入  参 : 无
 * 返回值 : uint8_t  
 **************************************************************************************/
uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i, j, dat = 0;

    for (i = 0; i < 8; i++)
    {
        j = DS18B20_Read_Bit();  // 从DS18B20读取一个bit
        dat = (dat) | (j << i);
    }

    return dat;
}

/**************************************************************************************
 * 描  述 : 写一个字节到DS18B20，低位先行
 * 入  参 : uint8_t
 * 返回值 : 无  
 **************************************************************************************/
void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t i, testb;
    DS18B20_Mode_Out_PP();

    for (i = 0; i < 8; i++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;

        if (testb)
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);  // 写0
            delay_us(8);   // 1us < 延时 < 15us

            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);  // 拉高总线
            delay_us(58);    // 58us + 8us > 60us
        }
        else
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);  // 写0
            delay_us(70);  // 60us < Tx 0 < 120us

            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);  // 拉高总线
            delay_us(2);   // 1us < Trec < 无限
        }
    }
}

/**************************************************************************************
 * 描  述 : 起始DS18B20
 * 入  参 : 无
 * 返回值 : 无  
 **************************************************************************************/
void DS18B20_Start(void)
{
    DS18B20_Rst();           // 主机给从机发送复位脉冲
    DS18B20_Presence();      // 检测从机给主机返回的存在脉冲
    DS18B20_Write_Byte(0xCC);  // 跳过ROM
    DS18B20_Write_Byte(0x44);  // 开始转换
}

/**************************************************************************************
 * 描  述 : DS18B20初始化函数
 * 入  参 : 无
 * 返回值 : uint8_t
 **************************************************************************************/
uint8_t DS18B20_Init(void)
{
    DS18B20_GPIO_Config();
    DS18B20_Rst();

    return DS18B20_Presence();
}

/**************************************************************************************
 * 描  述 : 从DS18B20读取温度值
 * 入  参 : 无  
 * 返回值 : float 
 **************************************************************************************/
float DS18B20_Get_Temp(void)
{
    uint8_t tpmsb, tplsb;
    short s_tem;
    float f_tem;

    DS18B20_Rst();
    DS18B20_Presence();
    DS18B20_Write_Byte(0xCC);  // 跳过ROM
    DS18B20_Write_Byte(0x44);  // 开始转换

    DS18B20_Rst();
    DS18B20_Presence();
    DS18B20_Write_Byte(0xCC);  // 跳过ROM
    DS18B20_Write_Byte(0xBE);  // 读取温度值

    tplsb = DS18B20_Read_Byte();
    tpmsb = DS18B20_Read_Byte();

    s_tem = tpmsb << 8;
    s_tem = s_tem | tplsb;

    if (s_tem < 0)  // 负温度
        f_tem = (~s_tem + 1) * 0.0625;
    else
        f_tem = (s_tem * 0.0625);

    return f_tem;
}
