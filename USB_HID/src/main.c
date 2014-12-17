/****************************************************************************
* Copyright (C), 2011 �ܶ�Ƕ��ʽ������ www.ourstm.net
*
* �������� �ܶ���STM32������V3�ϵ���ͨ��           
* QQ: 9191274, ������sun68, Email: sun68@163.com 
* �Ա����̣�ourstm.taobao.com  
*
* �ļ���: main.c
* ���ݼ���:	
*       
*	��ʾ�ü��̵�K1,K4,K7,K10ģ���������������ƶ���PC�Ͽ��Կ�����ͬ��걻�ƶ�

*
* �ļ���ʷ:
* �汾��  ����       ����    ˵��
* v0.2    2011-7-29 sun68  �������ļ�
*
*/
  
/* Includes ------------------------------------------------------------------*/
#define GLOBALS
#include "stm32f10x.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "stdio.h"	   
#include "demo.h"
#include "hw_config.h"

void Usart1_Init(void);
extern uint16_t MAL_Init (uint8_t lun);
extern void SPI_Flash_Init(void);
extern void USB_Disconnect_Config(void);
__IO uint8_t PrevXferComplete = 1;

/****************************************************************************
* ��    �ƣ�void RCC_Configuration(void)
* ��    �ܣ�ϵͳʱ������Ϊ72MHZ�� ����ʱ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void RCC_Configuration(void){

  SystemInit();	  
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC 
  						| RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOE , ENABLE);
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/****************************************************************************
* ��    �ƣ�void GPIO_Configuration(void)
* ��    �ܣ�ͨ��IO������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //״̬LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			 //ͨ���������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //���ģʽ����ٶ�50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure); 	
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		             //����1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* ������Ϸ�˷�������� */
  GPIO_InitStructure.GPIO_Pin = JOY_UP | JOY_DOWN ;	      	 //K1--��  K4---�¼�
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Key left + Key right */
  GPIO_InitStructure.GPIO_Pin = JOY_LEFT | JOY_RIGHT;		 //K7--�Ҽ�  K10--���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOC, GPIO_Pin_5);			  
}
/****************************************************************************
* ��    �ƣ�void NVIC_Configuration(void)
* ��    �ܣ��ж�Դ����
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void NVIC_Configuration(void)
{
  /*  �ṹ����*/
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  /* Configure one bit for preemption priority */
  /* ���ȼ��� ˵������ռ���ȼ����õ�λ�����������ȼ����õ�λ��  */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  
  
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	    //USB�����ȼ��ж�����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//��ռ���ȼ� 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//�����ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;			//USB�����ȼ��ж�����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//��ռ���ȼ� 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				//�����ȼ�Ϊ0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

 
}
/****************************************************************************
* ��    �ƣ�int main(void)
* ��    �ܣ�������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
int main(void)
{
  RCC_Configuration(); 		//����ϵͳʱ�� 
  USB_Disconnect_Config();	//����USB���ӿ�����    	   
  GPIO_Configuration();     //״̬LED�ĳ�ʼ��   
  NVIC_Configuration();     //USBͨ���ж�����
  USB_Init();				//USB��ʼ��
  while (1)
  {	
    if (JoyState() != 0)	//��ѯ�Ƿ�����Ч������
    {
      Joystick_Send(JoyState());    //����ֵͨ��USB���͵�PC
    }
  }
}

/******************* (C) COPYRIGHT 2011 �ܶ�STM32 *****END OF FILE****/
