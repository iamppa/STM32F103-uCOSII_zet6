/****************************************************************************
* Copyright (C), 2011 奋斗嵌入式工作室 www.ourstm.net
*
* 本例程在 奋斗版STM32开发板V3上调试通过           
* QQ: 9191274, 旺旺：sun68, Email: sun68@163.com 
* 淘宝店铺：ourstm.taobao.com  
*
* 文件名: main.c
* 内容简述:	
*       
*	演示用键盘的K1,K4,K7,K10模拟鼠标的上下左右移动，PC上可以看到如同鼠标被移动

*
* 文件历史:
* 版本号  日期       作者    说明
* v0.2    2011-7-29 sun68  创建该文件
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
* 名    称：void RCC_Configuration(void)
* 功    能：系统时钟配置为72MHZ， 外设时钟配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
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
* 名    称：void GPIO_Configuration(void)
* 功    能：通用IO口配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
****************************************************************************/  
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;				     //状态LED1
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			 //通用推挽输出模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			 //输出模式最大速度50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure); 	
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;		             //列线1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* 配置游戏杆方向键定义 */
  GPIO_InitStructure.GPIO_Pin = JOY_UP | JOY_DOWN ;	      	 //K1--上  K4---下键
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Key left + Key right */
  GPIO_InitStructure.GPIO_Pin = JOY_LEFT | JOY_RIGHT;		 //K7--右键  K10--左键
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOC, GPIO_Pin_5);			  
}
/****************************************************************************
* 名    称：void NVIC_Configuration(void)
* 功    能：中断源配置
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void NVIC_Configuration(void)
{
  /*  结构声明*/
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  /* Configure one bit for preemption priority */
  /* 优先级组 说明了抢占优先级所用的位数，和子优先级所用的位数  */    
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);	  
  
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;	    //USB低优先级中断请求
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//抢占优先级 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				//子优先级为1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  NVIC_InitStructure.NVIC_IRQChannel = USB_HP_CAN1_TX_IRQn;			//USB高优先级中断请求
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;			//抢占优先级 1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;				//子优先级为0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

 
}
/****************************************************************************
* 名    称：int main(void)
* 功    能：主函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
int main(void)
{
  RCC_Configuration(); 		//设置系统时钟 
  USB_Disconnect_Config();	//设置USB连接控制线    	   
  GPIO_Configuration();     //状态LED的初始化   
  NVIC_Configuration();     //USB通信中断配置
  USB_Init();				//USB初始化
  while (1)
  {	
    if (JoyState() != 0)	//查询是否有有效键按下
    {
      Joystick_Send(JoyState());    //将键值通过USB发送到PC
    }
  }
}

/******************* (C) COPYRIGHT 2011 奋斗STM32 *****END OF FILE****/
