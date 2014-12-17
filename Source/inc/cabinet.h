#ifndef __CABINET_H
#define __CABINET_H 
#include "stm32f10x.h" 
#include "GPIOLIKE51.h"

#define MaxGroupNo 3
#define MAXCabinetsPerGroup 16
typedef struct
{
	uint8_t name[10];
	uint16_t  GPIO_Pin;
	uint32_t  GPIO_CCR;
	GPIO_TypeDef*  GPIO_Port;
	
}CabinetDeviec_GPIO;

typedef struct
{
	uint8_t CabinetNO;
	//----------Door
	CabinetDeviec_GPIO Door_OE;
	CabinetDeviec_GPIO Door_LE;

	//----------Fan
	CabinetDeviec_GPIO Fan_OE;
	CabinetDeviec_GPIO Fan_LE;
	
	//----------State
	CabinetDeviec_GPIO State_OE;
	CabinetDeviec_GPIO State_LE;
	
	//18b20
	CabinetDeviec_GPIO DQ;
}CabinetDeviec;

#define LED_RCC_GPIO RCC_APB2Periph_GPIOC
#define LED_GPIO GPIOC
#define LED_PIN GPIO_Pin_13
#define LED_PIN_NO 13



#define DBUS_L_RCC_GPIO RCC_APB2Periph_GPIOA
#define DBUS_L_GPIO GPIOA
extern u16 DBUS_L_pin[8];

#define DBUS_H_RCC_GPIO RCC_APB2Periph_GPIOF
#define DBUS_H_GPIO GPIOF
extern u16 DBUS_H_pin[8];

extern CabinetDeviec CabinetDeviecs[MaxGroupNo];
/*
#define DBUS_pin0 GPIO_Pin_0
#define DBUS_pin1 GPIO_Pin_1
#define DBUS_pin2 GPIO_Pin_2
#define DBUS_pin3 GPIO_Pin_3
#define DBUS_pin4 GPIO_Pin_4
#define DBUS_pin5 GPIO_Pin_5
#define DBUS_pin6 GPIO_Pin_6
#define DBUS_pin7 GPIO_Pin_7
*/
#define DBUS_L_pins DBUS_L_pin[0] | DBUS_L_pin[1] | DBUS_L_pin[2] | DBUS_L_pin[3] | DBUS_L_pin[4] | DBUS_L_pin[5] | DBUS_L_pin[6] | DBUS_L_pin[7]
#define DBUS_H_pins DBUS_H_pin[0] | DBUS_H_pin[1] | DBUS_H_pin[2] | DBUS_H_pin[3] | DBUS_H_pin[4] | DBUS_H_pin[5] | DBUS_H_pin[6] | DBUS_H_pin[7]

#define LED_ON {PCout(LED_PIN_NO)=0;}
#define LED_OFF {PCout(LED_PIN_NO)=1;}

void CabinetDeviec_Init(void);
void CabinetDeviec_setDoor(u8 mCabinetNO,u16 val);
void CabinetDeviec_setFan(u8 mCabinetNO,u16 val);
u16 CabinetDeviec_getState(u8 mCabinetNO);

#define DBUS_LOW  0
#define DBUS_HIGH  1

#define DBUS_IN  0
#define DBUS_OUT 1

#endif
