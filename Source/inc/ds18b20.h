#ifndef __DS18B20_H
#define __DS18B20_H 
#include "stm32f10x.h"  
//#include "stm32f10x_lib.h"

//Mini STM32开发板
//DS18B20 驱动函数 
//正点原子@ALIENTEK
//2010/6/17	
   	
u8 DS18B20_GPIO_Init(void);//初始化DS18B20
short DS18B20_Get_Temp(void);//获取温度
void DS18B20_Start(void);//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);//读出一个字节
u8 DS18B20_Read_Bit(void);//读出一个位
u8 DS18B20_Rst(void);//复位DS18B20    
void delay_us(u32 nus);
void delay_ms(u32 nms);
void DS18B20_Write_Bit(u8 dat);
void selectDQ(u16 *Pin);
short DS18B20_Get_Temp_ROM(u8 rom[8][8],u8 index);
void GetId(u8 *id);
void test_DQ_clk();
void test2_DQ_clk();

u8 Search_Rom(u8 ROM_ID[8][8]);
//unsigned int middle(unsigned int *f);
//unsigned int DQ_ReadTemp(unsigned char cIdx);
//unsigned char DQ_RecByte();
//void DQ_SendByte(unsigned char date);
//u8 DQ_Reset(void);

#endif















