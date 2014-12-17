#ifndef __DS18B20_H
#define __DS18B20_H 
#include "stm32f10x.h"  
//#include "stm32f10x_lib.h"

//Mini STM32������
//DS18B20 �������� 
//����ԭ��@ALIENTEK
//2010/6/17	
   	
u8 DS18B20_GPIO_Init(void);//��ʼ��DS18B20
short DS18B20_Get_Temp(void);//��ȡ�¶�
void DS18B20_Start(void);//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);//����һ���ֽ�
u8 DS18B20_Read_Bit(void);//����һ��λ
u8 DS18B20_Rst(void);//��λDS18B20    
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















