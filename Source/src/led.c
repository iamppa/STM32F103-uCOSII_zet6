#include "stm32f10x.h"
#include "led.h"
#include "GPIOLIKE51.h"
#include "uart_printf.h"

void led_init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;

	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE);						 
	
	//=============================================================================
	
	//LED -> PC13
	
	//============================================================================= 		 
	
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

}

void led_on(void)
{
	PCout(13)=0;
	
	//printf("led on\n");
}

void led_off(void)
{
	PCout(13)=1;

	//printf("led off\n");
}
