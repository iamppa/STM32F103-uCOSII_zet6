#include "key.h"

void KeyInit (void)

{
GPIO_InitTypeDef GPIO_InitStructure;

RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

GPIO_Init(GPIOB , &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;

GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

GPIO_Init(GPIOB , &GPIO_InitStructure);
}

u8 GetKeyIn (uint16_t GPIO_PIN)
{
	u8 keyvalume = 1;
	switch(GPIO_PIN){
		case key_gotoapi:
			keyvalume = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1));
			break;
		case key_updateapp:
			keyvalume = (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0));
			break;
		default:
			break;
	}
	return keyvalume;
}
