#include "cabinet.h"
#include "ucos_ii.h"

OS_EVENT *Sem_DBus;
INT8U Sem_err;
char printfbuff[50]={0};
u16 stateval = 0;
u16 DBUS_L_pin[8] = {GPIO_Pin_0,GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7};
u16 DBUS_H_pin[8] = {GPIO_Pin_0,GPIO_Pin_1,GPIO_Pin_2,GPIO_Pin_3,GPIO_Pin_4,GPIO_Pin_5,GPIO_Pin_6,GPIO_Pin_7};
/*
CabinetDeviec CabinetDeviecs[MaxGroupNo] = {
	//------------
	
	{
		(uint8_t) 0,
		{"Door_OE_A",GPIO_Pin_8,RCC_APB2Periph_GPIOB,GPIOB},
		{"Door_LE_A",GPIO_Pin_7,RCC_APB2Periph_GPIOB,GPIOB},
		{"Fan_OE_A",GPIO_Pin_6,RCC_APB2Periph_GPIOB,GPIOB},
		{"Fan_LE_A",GPIO_Pin_5,RCC_APB2Periph_GPIOB,GPIOB},
		{"State_OE_A",GPIO_Pin_4,RCC_APB2Periph_GPIOB,GPIOB},
		{"State_LE_A",GPIO_Pin_3,RCC_APB2Periph_GPIOB,GPIOB},
		
		{"QD_A",GPIO_Pin_13,RCC_APB2Periph_GPIOC,GPIOC},
	},
	{
		(uint8_t) 0,
		{"Door_OE_B",GPIO_Pin_0,RCC_APB2Periph_GPIOB,GPIOB},
		{"Door_LE_B",GPIO_Pin_1,RCC_APB2Periph_GPIOB,GPIOB},
		{"Fan_OE_B",GPIO_Pin_10,RCC_APB2Periph_GPIOB,GPIOB},
		{"Fan_LE_B",GPIO_Pin_11,RCC_APB2Periph_GPIOB,GPIOB},
		{"State_OE_B",GPIO_Pin_12,RCC_APB2Periph_GPIOB,GPIOB},
		{"State_LE_B",GPIO_Pin_13,RCC_APB2Periph_GPIOB,GPIOB},
		
		{"QD_A",GPIO_Pin_14,RCC_APB2Periph_GPIOC,GPIOC},
	},
	{
		(uint8_t) 2,
		{"Door_OE_C",GPIO_Pin_14,RCC_APB2Periph_GPIOB,GPIOB},
		{"Door_LE_C",GPIO_Pin_15,RCC_APB2Periph_GPIOB,GPIOB},
		{"Fan_OE_C",GPIO_Pin_8,RCC_APB2Periph_GPIOA,GPIOA},
		{"Fan_LE_C",GPIO_Pin_14,RCC_APB2Periph_GPIOA,GPIOA},
		{"State_OE_C",GPIO_Pin_9,RCC_APB2Periph_GPIOB,GPIOB},
		{"State_LE_C",GPIO_Pin_13,RCC_APB2Periph_GPIOA,GPIOA},
		
		{"QD_C",GPIO_Pin_15,RCC_APB2Periph_GPIOC,GPIOC},
	},
	{
		(uint8_t) 2,
		{"Door_OE_D",GPIO_Pin_15,RCC_APB2Periph_GPIOG,GPIOG},
		{"Door_LE_D",GPIO_Pin_14,RCC_APB2Periph_GPIOG,GPIOG},
		{"Fan_OE_D",GPIO_Pin_13,RCC_APB2Periph_GPIOG,GPIOG},
		{"Fan_LE_D",GPIO_Pin_12,RCC_APB2Periph_GPIOG,GPIOG},
		{"State_OE_D",GPIO_Pin_11,RCC_APB2Periph_GPIOG,GPIOG},
		{"State_LE_D",GPIO_Pin_10,RCC_APB2Periph_GPIOG,GPIOG},
		
		{"QD_D",GPIO_Pin_12,RCC_APB2Periph_GPIOC,GPIOC},
	},
	{
		(uint8_t) 2,
		{"Door_OE_E",GPIO_Pin_9,RCC_APB2Periph_GPIOG,GPIOG},
		{"Door_LE_E",GPIO_Pin_7,RCC_APB2Periph_GPIOD,GPIOD},
		{"Fan_OE_E",GPIO_Pin_6,RCC_APB2Periph_GPIOD,GPIOD},
		{"Fan_LE_E",GPIO_Pin_5,RCC_APB2Periph_GPIOD,GPIOD},
		{"State_OE_E",GPIO_Pin_4,RCC_APB2Periph_GPIOD,GPIOD},
		{"State_LE_E",GPIO_Pin_3,RCC_APB2Periph_GPIOD,GPIOD},
		
		{"QD_E",GPIO_Pin_11,RCC_APB2Periph_GPIOC,GPIOC},
	},
};*/
/*//2
CabinetDeviec CabinetDeviecs[MaxGroupNo] = {
	//------------
	{
		(uint8_t) 0,
		{"Door_OE_A",GPIO_Pin_8,RCC_APB2Periph_GPIOB,GPIOB},
		{"Door_LE_A",GPIO_Pin_7,RCC_APB2Periph_GPIOB,GPIOB},
		{"Fan_OE_A",GPIO_Pin_6,RCC_APB2Periph_GPIOB,GPIOB},
		{"Fan_LE_A",GPIO_Pin_5,RCC_APB2Periph_GPIOB,GPIOB},
		{"State_OE_A",GPIO_Pin_4,RCC_APB2Periph_GPIOB,GPIOB},
		{"State_LE_A",GPIO_Pin_3,RCC_APB2Periph_GPIOB,GPIOB},
		
		{"QD_A",GPIO_Pin_12,RCC_APB2Periph_GPIOC,GPIOC},
	},
	{
		(uint8_t) 1,
		{"Door_OE_B",GPIO_Pin_13,RCC_APB2Periph_GPIOD,GPIOD},
		{"Door_LE_B",GPIO_Pin_12,RCC_APB2Periph_GPIOD,GPIOD},
		{"Fan_OE_B",GPIO_Pin_11,RCC_APB2Periph_GPIOD,GPIOD},
		{"Fan_LE_B",GPIO_Pin_10,RCC_APB2Periph_GPIOD,GPIOD},
		{"State_OE_B",GPIO_Pin_8,RCC_APB2Periph_GPIOD,GPIOD},
		{"State_LE_B",GPIO_Pin_9,RCC_APB2Periph_GPIOD,GPIOD},
		
		{"QD_B",GPIO_Pin_14,RCC_APB2Periph_GPIOC,GPIOC},
	},
	{
		(uint8_t) 2,
		{"Door_OE_C",GPIO_Pin_3,RCC_APB2Periph_GPIOG,GPIOG},
		{"Door_LE_C",GPIO_Pin_2,RCC_APB2Periph_GPIOG,GPIOG},
		{"Fan_OE_C",GPIO_Pin_5,RCC_APB2Periph_GPIOG,GPIOG},
		{"Fan_LE_C",GPIO_Pin_4,RCC_APB2Periph_GPIOG,GPIOG},
		{"State_OE_C",GPIO_Pin_7,RCC_APB2Periph_GPIOG,GPIOG},
		{"State_LE_C",GPIO_Pin_6,RCC_APB2Periph_GPIOG,GPIOG},
		
		{"QD_C",GPIO_Pin_15,RCC_APB2Periph_GPIOC,GPIOC},
	},
};*/

CabinetDeviec CabinetDeviecs[MaxGroupNo] = {
		//------------
		{
			(uint8_t) 0,
			{"Door_OE_A",GPIO_Pin_10,RCC_APB2Periph_GPIOE,GPIOE},
			{"Door_LE_A",GPIO_Pin_11,RCC_APB2Periph_GPIOE,GPIOE},
			{"Fan_OE_A",GPIO_Pin_12,RCC_APB2Periph_GPIOE,GPIOE},
			{"Fan_LE_A",GPIO_Pin_13,RCC_APB2Periph_GPIOE,GPIOE},
			{"State_OE_A",GPIO_Pin_14,RCC_APB2Periph_GPIOE,GPIOE},
			{"State_LE_A",GPIO_Pin_15,RCC_APB2Periph_GPIOE,GPIOE},
			
			{"QD_A",GPIO_Pin_12,RCC_APB2Periph_GPIOC,GPIOC},
		},
		{
			(uint8_t) 1,
			{"Door_OE_B",GPIO_Pin_13,RCC_APB2Periph_GPIOD,GPIOD},
			{"Door_LE_B",GPIO_Pin_12,RCC_APB2Periph_GPIOD,GPIOD},
			{"Fan_OE_B",GPIO_Pin_11,RCC_APB2Periph_GPIOD,GPIOD},
			{"Fan_LE_B",GPIO_Pin_10,RCC_APB2Periph_GPIOD,GPIOD},
			{"State_OE_B",GPIO_Pin_8,RCC_APB2Periph_GPIOD,GPIOD},
			{"State_LE_B",GPIO_Pin_9,RCC_APB2Periph_GPIOD,GPIOD},
			
			{"QD_B",GPIO_Pin_14,RCC_APB2Periph_GPIOC,GPIOC},
		},
		{
			(uint8_t) 2,
			{"Door_OE_C",GPIO_Pin_3,RCC_APB2Periph_GPIOG,GPIOG},
			{"Door_LE_C",GPIO_Pin_2,RCC_APB2Periph_GPIOG,GPIOG},
			{"Fan_OE_C",GPIO_Pin_5,RCC_APB2Periph_GPIOG,GPIOG},
			{"Fan_LE_C",GPIO_Pin_4,RCC_APB2Periph_GPIOG,GPIOG},
			{"State_OE_C",GPIO_Pin_7,RCC_APB2Periph_GPIOG,GPIOG},
			{"State_LE_C",GPIO_Pin_6,RCC_APB2Periph_GPIOG,GPIOG},
			
			{"QD_C",GPIO_Pin_15,RCC_APB2Periph_GPIOC,GPIOC},
		},
};

void WaitDateFor573()
{
	int i,j;
	for(i=100;i>0;i--)
		for(j=100;j>0;j--);
	return;
}
void Cabinet_GPIO_SetBit(CabinetDeviec_GPIO * mCabinetDeviec_GPIO,bool flag)
{
	CabinetDeviec_GPIO * cabinetDeviec_GPIO = mCabinetDeviec_GPIO;
	//sprintf(printfbuff,"name =%s,f=%d,pin=%x,port=%x",cabinetDeviec_GPIO->name,flag,cabinetDeviec_GPIO->GPIO_Pin,cabinetDeviec_GPIO->GPIO_Port);

	if(flag){
		GPIO_SetBits(cabinetDeviec_GPIO->GPIO_Port,cabinetDeviec_GPIO->GPIO_Pin);
	}else{
		GPIO_ResetBits(cabinetDeviec_GPIO->GPIO_Port,cabinetDeviec_GPIO->GPIO_Pin);
	}
	return;
}

void DBUS_init(bool in_out)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( DBUS_L_RCC_GPIO,ENABLE);
	GPIO_InitStructure.GPIO_Pin = DBUS_L_pins;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(in_out){
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
		GPIO_ResetBits(DBUS_L_GPIO,DBUS_L_pins);
	}else{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	}
	GPIO_Init(DBUS_L_GPIO, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd( DBUS_H_RCC_GPIO,ENABLE);
	GPIO_InitStructure.GPIO_Pin = DBUS_H_pins;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if(in_out){
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
		GPIO_ResetBits(DBUS_H_GPIO,DBUS_H_pins);
	}else{
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	}
	GPIO_Init(DBUS_H_GPIO, &GPIO_InitStructure);
	
	return;
}

void SetDBUS(u16 val) 
{
	GPIO_SetBits(DBUS_L_GPIO,val&0xff);
	GPIO_ResetBits(DBUS_L_GPIO,(~val)&0xff);
	GPIO_SetBits(DBUS_H_GPIO,(val>>8)&0xff);
	GPIO_ResetBits(DBUS_H_GPIO,(~val>>8)&0xff);
	return;
}

u16 GetDBUS() 
{
	u16 DBUS_VAL=0;
	int i;
	for(i=0;i<8;i++){
		if(GPIO_ReadInputDataBit(DBUS_L_GPIO,DBUS_L_pin[i])){
			DBUS_VAL |= (0x01<<i);
		}
	}
	for(i=0;i<8;i++){
		if(GPIO_ReadInputDataBit(DBUS_H_GPIO,DBUS_H_pin[i])){
			DBUS_VAL |= (0x01<<(8+i));
		}
	}
	return DBUS_VAL;
}

void CabinetDeviec_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd( LED_RCC_GPIO,ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_GPIO, &GPIO_InitStructure);
	LED_OFF
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB| RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	return;
}
void Ds18b20_Gpio_Ini(CabinetDeviec_GPIO * DQ_GPIO)
{
	GPIO_InitTypeDef GPIO_InitStructure;	//GPIO
	//RCC->APB2ENR|=1<<2;    //使能PORTC口时钟 
	RCC_APB2PeriphClockCmd(	DQ_GPIO->GPIO_CCR, ENABLE );	 //使能PORTC口时钟  
	GPIO_InitStructure.GPIO_Pin = DQ_GPIO->GPIO_Pin;  //SPI CS
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DQ_GPIO->GPIO_Port, &GPIO_InitStructure);
	/* Deselect the PA0 Select high */
	GPIO_SetBits(DQ_GPIO->GPIO_Port,GPIO_InitStructure.GPIO_Pin);
}
void CabinetDeviec_Init(void)
{
	int i = 0; 
	DBUS_init( DBUS_OUT );
	Sem_DBus = OSSemCreate(1);
	CabinetDeviec_GPIO_Init();
	for (i=0;i<MaxGroupNo;i++){
		CabinetDeviec *mCabinetDeviec = & CabinetDeviecs[i];
		Ds18b20_Gpio_Ini(&(mCabinetDeviec->DQ));
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->State_OE), 1 );//State
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->State_LE), 0 );
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->Door_OE), 0 );//Door
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->Door_LE), 1 );
		WaitDateFor573();
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->Door_LE),0);
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->Fan_OE), 0 );//Fan
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->Fan_LE), 1 );
		WaitDateFor573();
		Cabinet_GPIO_SetBit(&(mCabinetDeviec->Fan_LE),0);
	}
	return ;
}

void CabinetDeviec_setDoor(u8 mCabinetNO,u16 val)
{
	CabinetDeviec * mCabinetDeviec =  & CabinetDeviecs[mCabinetNO];
	OSSemPend(Sem_DBus,1000*10,&Sem_err);
	SetDBUS(val);
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->Door_LE),1);
	WaitDateFor573();
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->Door_LE),0);
	OSSemPost(Sem_DBus);
	return;
}
void CabinetDeviec_setFan(u8 mCabinetNO,u16 val)
{
	CabinetDeviec *mCabinetDeviec = & CabinetDeviecs[mCabinetNO];
	OSSemPend(Sem_DBus,1000*10,&Sem_err);
	SetDBUS(val);
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->Fan_LE),1);
	WaitDateFor573();
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->Fan_LE),0);
	OSSemPost(Sem_DBus);
	return;
}
u16 CabinetDeviec_getState(u8 mCabinetNO)
{
	CabinetDeviec * mCabinetDeviec =  & CabinetDeviecs[mCabinetNO];
	OSSemPend(Sem_DBus,1000*10,&Sem_err);
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->State_LE),1);
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->State_OE),0);
	WaitDateFor573();
	DBUS_init(DBUS_IN);
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->State_LE),0);
	stateval = GetDBUS();
	Cabinet_GPIO_SetBit(&(mCabinetDeviec->State_OE),1);
	DBUS_init(DBUS_OUT);
	OSSemPost(Sem_DBus);
	return stateval;
}



