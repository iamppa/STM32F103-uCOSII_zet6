#include "ucos_ii.h"
#include "cabinet.h"
#include "GPIOLIKE51.h"
#include "ds18b20.h"
#include "uart1_dma.h"
#include "uart_printf.h"
#include "DataFlash.h"
#include "utils.h"

#define cmd_tick 0b1010
#define cmd_do 0b0101
#define cmd_fan 0x0e
#define cmd_door 0x04
#define cmd_getinfo 0x0f
#define cmd_hotplagin 0x0c
#define cmd_hotplagout 0x03
#define cmd_gotoiap 0x07

#define FlashDataPrartitionAddr 0x8010000
#define APPPrartitionAddr 0x00004400

#define API_VER 2
static int PadOnline = 0;
typedef struct
{
	unsigned char starts[3];
	unsigned char resure;
	unsigned char cmd:4;//4b,1010 表示需要执行
	unsigned char cmd_takeback:4;//4b,tick 取反等于tick_takeback
	unsigned char opendoor:8; //8b,00001000  表示打开第4个柜子
	unsigned char opendoor_takeback:8; // 8b  opendoor 取反等于opendoor_takeback
	unsigned char openfan:8; //8b,00001001  表示打开第4个和第一个柜子的风扇
	unsigned char openfan_takeback:8; //8b  openfan 取反等于openfan_takeback
	unsigned char boxNO:8;	
}maincontraller2microcontroller_t;

typedef struct
{
	unsigned char decimal:1;
	unsigned char integer:7;
}temperature_t;
typedef struct
{
	unsigned char BoxNO:7;
	unsigned char cabinetinfo_online:1;
	unsigned char doorstate:8;
	unsigned char fanstate:8;
	unsigned char online:8;
	temperature_t temperature[8];
}cabinetinfo_t;

temperature_t gettemperature(unsigned short temperature){
	temperature_t temp;
	temp.integer = (unsigned char)(temperature/10);
	temp.decimal = (unsigned char)(temperature%10>3 ? 1:0);
	if(temperature%10>7){
		temp.integer += 1;
		temp.decimal = 0;
	}
	return temp;	
}

typedef struct 
{
	unsigned char flag[3];
	unsigned char api_ver;
	cabinetinfo_t cabinetinfos[2];
} microcontroller2maincontraller_t;



void opendoor(int BoxNo,u16 cabinetNO )
{
	CabinetDeviec_setDoor(BoxNo,cabinetNO);
	OSTimeDly(20);
	CabinetDeviec_setDoor(BoxNo,0x00);
}

void openfan(int BoxNo,u16 cabinetNO )
{
	CabinetDeviec_setFan(BoxNo,cabinetNO);
}

static void IapProgramRun(void);
static OS_STK startup_task_stk[STARTUP_TASK_STK_SIZE];
static OS_STK task1_stk[TASK1_STK_SIZE];
static OS_STK task2_stk[TASK2_STK_SIZE];
static OS_STK task3_stk[TASK3_STK_SIZE];
static OS_STK task4_stk[TASK4_STK_SIZE];

static u8 ROM_ID[MaxGroupNo][MAXCabinetsPerGroup][8] = {0};
static u8 ROM_ID_current[MAXCabinetsPerGroup][8] = {0};
static u16 ONLINE[MaxGroupNo] = {0};
static u16 FanState[MaxGroupNo] = {0};
static u16 DoorState[MaxGroupNo] = {0};
static short Temperatures[MaxGroupNo][MAXCabinetsPerGroup] = {0};

INT8U err;
OS_EVENT *Sem;
OS_EVENT *flashdata;
microcontroller2maincontraller_t micro2main;
maincontraller2microcontroller_t *main2micro = \
	(maincontraller2microcontroller_t *) &USART1_RecvBuff;

bool hotplag = 0;
static void systick_init(void)
{
	RCC_ClocksTypeDef rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	SysTick_Config(rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC);
}
u8 Get_Crc8(u8 *ptr,u16 len) 
{ 
	u8 crc; 
	u8 i; 
	crc=0; 
	while(len--) 
	{ 
		crc^=*ptr++; 
		for(i=0;i<8;i++) 
		{ 
			if(crc&0x01)crc=(crc>>1)^0x8C; 
			else crc >>= 1;
		} 
	} 
	return crc; 
} 
void Clean_ROM(u8 * ROM)
{
	int j;
	for(j=0;j<8;j++)
		ROM[j] = 0;
}
void SetBit(u16 *Date,int index){
	*Date |= 0x0001<<index; 
}
void CleanBit(u16 *Date,int index){
	*Date &= ~(0x0001<<index); 
}
int isTheSameID(u8 rom_id_old[8],u8 rom_id_current[8])
{
	int i;
	for(i=0;i<8;i++)
	{
		if(rom_id_old[i] != rom_id_current[i])
			return 0;
	}
	return 1;
}
int CabinetDeviecsChange(u8 rom_ids_old[MAXCabinetsPerGroup][8],
					u8 rom_ids_current[MAXCabinetsPerGroup][8])
{
	int i,j;
	for(i=0;i<MAXCabinetsPerGroup;i++)
	{
		if(rom_ids_current[i][0] != 0x28)
			continue;
		for(j=0;j<MAXCabinetsPerGroup;j++)
		{
			if(rom_ids_old[j][0] != 0x28)
				continue;
			if(isTheSameID(rom_ids_current[i],rom_ids_old[j]))
				break;
		}
		if(j==MAXCabinetsPerGroup){ 
			return i+1;
		}
	}
	
	for(i=0;i<MAXCabinetsPerGroup;i++)
	{
		if(rom_ids_old[i][0] != 0x28)
			continue;
		for(j=0;j<MAXCabinetsPerGroup;j++)
		{
			if(rom_ids_current[j][0] != 0x28)
				continue;
			if(isTheSameID(rom_ids_current[j],rom_ids_old[i]))
				break;
		}
		if(j==MAXCabinetsPerGroup){ 
			return -(i+1);
		}
	}
	return 0;
}
void cp_rom_id(u8 oldid[8],u8 newid[8])
{
	int i;
	for(i=0;i<8;i++)
		oldid[i]=newid[i];
}
static void init_Cabinetinfo()
{
	int i,j;
	Flash_Read(FlashDataPrartitionAddr, (uint8_t *)ROM_ID, MaxGroupNo*MAXCabinetsPerGroup*8);
	for(i=0;i<MaxGroupNo;i++)
		for(j=0;j<MAXCabinetsPerGroup;j++){
			if(ROM_ID[i][j][0]== 0x28){
				SetBit(&ONLINE[i],j);
				SetBit(&ONLINE[i],j);
			}
			else
			{
				Clean_ROM(ROM_ID[i][j]);
				CleanBit(&ONLINE[i],j);
			}
		}			
}
static void Refresh_GroupTempratures(u8 ROM_ID_Group[MAXCabinetsPerGroup][8],short Temperatures_Group[MAXCabinetsPerGroup])
{
	int j;
	for(j=0;j<MAXCabinetsPerGroup;j++){
		if(ROM_ID_Group[j][0] == 0x28){
			Temperatures_Group[j] = DS18B20_Get_Temp_ROM(ROM_ID_Group,j);
		}
		else{
			Temperatures_Group[j] = 0;
		}
		OSTimeDly(10);
	}
}
static void SelectDQBus(int GroupNo)
{
	int j;
	u16 DQ_Pin = 13;
	for(j=0;j<16;j++)
		if(CabinetDeviecs[GroupNo].DQ.GPIO_Pin & (1<<j))
			DQ_Pin = j;
	selectDQ(&DQ_Pin);
}
int CrcCheck(u8 rom_id[MAXCabinetsPerGroup][8])
{
	int ret = 0,j;
	for(j=0;j<MAXCabinetsPerGroup;j++)
	{
		if(rom_id[j][0]!=0x28)
			continue;
		if(rom_id[j][7]!=Get_Crc8(rom_id[j],7))
		{
			ret++;
			Clean_ROM(rom_id[j]);
		}
	}
	return ret;
}
void CheckCabinetDeviecsChange(int GroupNo){
	int devicechange = 0;
	u8 i,k=0;
	u16 device=0;
	u16 doorstate = 0;
	short temp ;
	for(i=0;i<MAXCabinetsPerGroup;i++)
		Clean_ROM(ROM_ID_current[i]);
	Search_Rom(ROM_ID_current);
	CrcCheck(ROM_ID_current);
	devicechange = CabinetDeviecsChange(ROM_ID[GroupNo],ROM_ID_current);
	if(devicechange > 0)//有增加新设备
	{
		doorstate = CabinetDeviec_getState(GroupNo);
		
		while(!device){
			OSTimeDly(200);
			LED_ON
			OSTimeDly(300);
			LED_OFF
			device = CabinetDeviec_getState(GroupNo) ^ doorstate;
			if( k>20 )
				break;
			else
				k++;
		}
		if(hotplag) LED_ON
		if(device){
			for(k=0;k<16;k++)
				if(device&(1<<k)) {
					cp_rom_id(ROM_ID[GroupNo][k],ROM_ID_current[devicechange-1]);
					SetBit(&ONLINE[GroupNo],k);
				}
			OSSemPost(flashdata);
		}
	}
	else if(devicechange < 0)//有设备掉线
	{
		k = -(1+devicechange);
		temp = DS18B20_Get_Temp_ROM(ROM_ID[GroupNo],k);
		if(temp == 0){
			Clean_ROM(ROM_ID[GroupNo][k]);
			CleanBit(&ONLINE[GroupNo],k);
			Temperatures[GroupNo][k] = 0;
			OSSemPost(flashdata);
		}
	}
}


static void task1(void *p_arg)
{
	int i;
	int GroupNO;
	DS18B20_Init();
	for(;;)
	{
		OSTimeDly(1000);
		for(GroupNO=0;GroupNO<MaxGroupNo;GroupNO++)
		{
			SelectDQBus(GroupNO);
			//test2_DQ_clk();
			if(hotplag){
				CheckCabinetDeviecsChange(GroupNO);
				OSTimeDly(10);
			}
			Refresh_GroupTempratures(ROM_ID[GroupNO],Temperatures[GroupNO]);
			OSTimeDly(500);
		}
	}
}
void GetMicro2MainDate(int GroupNo)
{
	int j,i;
	u16 doorstate = 0;
	if(GroupNo>(MaxGroupNo-1)) return;
	for(j=0;j<2;j+=2)
	{	
		micro2main.cabinetinfos[j].BoxNO = 2*GroupNo;
		doorstate = CabinetDeviec_getState(GroupNo);
		micro2main.cabinetinfos[j].online = (ONLINE[GroupNo])&0xff;
		micro2main.cabinetinfos[j].doorstate = doorstate&0xff;
		micro2main.cabinetinfos[j].fanstate = (FanState[GroupNo])&0xff;
	
		for(i =0;i<8;i++){
			micro2main.cabinetinfos[j].temperature[i] = gettemperature(\
					Temperatures[GroupNo][i]);
		}
		micro2main.cabinetinfos[j+1].BoxNO = 2*GroupNo+1;
		micro2main.cabinetinfos[j+1].online = (ONLINE[GroupNo]>>8)&0xff;
		micro2main.cabinetinfos[j+1].doorstate = (doorstate>>8)&0xff;
		micro2main.cabinetinfos[j+1].fanstate = (FanState[GroupNo]>>8)&0xff;
	
		for(i =0;i<8;i++){
			micro2main.cabinetinfos[j+1].temperature[i] = gettemperature(\
					Temperatures[GroupNo][8+i]);
		}
		GroupNo++;
	}
}

int ChechMain2MicroDate(){
	if((main2micro->cmd&main2micro->cmd_takeback)
		|(main2micro->opendoor&main2micro->opendoor_takeback)
		|(main2micro->openfan&main2micro->openfan_takeback))
		return 0;
	else
		return 1;

}
static void task2(void *p_arg)
{
	int uart_ticks = 0;
	INT8U err_task2 = 0;
	int i,j,startflag;
	u16 cmddata ;
	char cmd_buff[25] = {0};
	DMA_Cmd(DMA1_Channel5, ENABLE); //使能DMA通道5

	for (;;)
	{
		//OSTimeDly(300);
		OSSemPend(Sem_recv,1000*10,&err_task2);
		if(err_task2 == OS_TIMEOUT)
		{
			uart_ticks ++;
			uprintf("Sem_recv timeout!");
			PadOnline = 0;
			goto err;
		}
		else if(err_task2 == OS_NO_ERR)
		{
			uart_ticks = 0;
			OSTimeDly(1);
			PadOnline = 1;
			startflag = findPPA(USART1_RecvBuff,USART1_RECVBUFF_SIZE);
			if(startflag >0){
				sprintf(cmd_buff,"%X,%X,%X,%X,%X,%X,%X,%X,%X,%X!moveLeftK %d",USART1_RecvBuff[0],\
				USART1_RecvBuff[1],USART1_RecvBuff[2],USART1_RecvBuff[3],\
				USART1_RecvBuff[4],USART1_RecvBuff[5],USART1_RecvBuff[6],\
				USART1_RecvBuff[7],USART1_RecvBuff[8],USART1_RecvBuff[9],startflag);
				uprintf(cmd_buff);
				moveLeftK( USART1_RecvBuff,USART1_RECVBUFF_SIZE, startflag);
				DMA1_USART1_RX_reset();
			}else if(startflag < 0){
				uprintf("cannot find PPA!");
				goto err;
			}
			if(ChechMain2MicroDate()!=0){
				u8 groupno = main2micro->boxNO/2;
				u8 H_L = main2micro->boxNO%2;
				switch(main2micro->cmd){
					case cmd_door:
						uprintf("cmd_door");
						cmddata = main2micro->opendoor;
						if(H_L) cmddata = cmddata << 8;
						opendoor(groupno,cmddata);
						break;
					case  cmd_hotplagin :
						uprintf("cmd_hotplagin");
						hotplag = 1;
						LED_ON
						break;
					case  cmd_hotplagout:
						uprintf("cmd_hotplagout");
						hotplag = 0;
						LED_OFF
						break;
					case  cmd_gotoiap:
						uprintf("cmd_gotoiap");
						OSSemPost(Sem);
						break;
					case cmd_fan:
						uprintf("cmd_fan");
						if(H_L){
							FanState[groupno] = FanState[groupno]&0x00ff;
							FanState[groupno] += main2micro->openfan << 8;
						}else{
							FanState[groupno] = FanState[groupno]&0xff00;
							FanState[groupno] += main2micro->openfan;
						}
						openfan(groupno,FanState[groupno]);
						break;
					case cmd_getinfo:
						uprintf("cmd_getinfo");
						break;
					default:
						uprintf("unknow cmd");
						goto err;
						break;
				}
			}else{
				uprintf("ChechMain2MicroDate err!");
				goto err;
			}

		}else{
			uprintf("Sem_recv err unknow!");
			goto err;
		}
		goto ko;
err:
		printf(cmd_buff,"%X,%X,%X,%X,%X,%X,%X,%X,%X,%X!",USART1_RecvBuff[0],\
				USART1_RecvBuff[1],USART1_RecvBuff[2],USART1_RecvBuff[3],\
				USART1_RecvBuff[4],USART1_RecvBuff[5],USART1_RecvBuff[6],\
				USART1_RecvBuff[7],USART1_RecvBuff[8],USART1_RecvBuff[9]);
		uprintf(cmd_buff);
		DMA1_USART1_RX_reset();

ko:
		DMA_Cmd(DMA1_Channel5,ENABLE);
	}
}
static void task3(void *p_arg)
{
	INT8U err_task3 = 0;
	INT32U cpu_sr;
	u8 GroupNo = 0;
	micro2main.flag[0] = 'P';
	micro2main.flag[1] = 'P';
	micro2main.flag[2] = 'A';
	micro2main.api_ver = API_VER;
	for(;;){
		OSSemPend(Sem,250,&err_task3);
		if(err_task3 == OS_NO_ERR){
			OS_ENTER_CRITICAL();
			IapProgramRun();
			OS_EXIT_CRITICAL();
		}else{
			if(PadOnline){
				GetMicro2MainDate(GroupNo);
				OSSemPend(Sem_send,0,&err_task3);
				DMA_Cmd(DMA1_Channel4,ENABLE);
				GroupNo++;
				if(GroupNo>2) GroupNo = 0;
			}
		}
	}
}

static void task4(void *p_arg)
{
	INT8U err_task4 = 0;
	for (;;)
	{
		OSSemPend(flashdata,0,&err_task4);
		Flash_Write(FlashDataPrartitionAddr, (uint8_t *)ROM_ID, MaxGroupNo*MAXCabinetsPerGroup*8);
	}
}

static void startup_task(void *p_arg)
{
	INT8U err_task0 = 0;
	systick_init();     /* Initialize the SysTick. */
	CabinetDeviec_Init();
#if (OS_TASK_STAT_EN > 0)
    OSStatInit();      /* Determine CPU capacity. */
#endif
	/* TODO: create application tasks here */
	init_Cabinetinfo();
	err_task0 = OSTaskCreate(task1, (void *)0,
	                   &task1_stk[TASK1_STK_SIZE-1], TASK1_PRIO);      

	err_task0 = OSTaskCreate(task2, (void *)0,
	                   &task2_stk[TASK2_STK_SIZE-1], TASK2_PRIO);
	
	err_task0 = OSTaskCreate(task3, (void *)0,
	                   &task3_stk[TASK3_STK_SIZE-1], TASK3_PRIO);

	err_task0 = OSTaskCreate(task4, (void *)0,
	                   &task4_stk[TASK4_STK_SIZE-1], TASK4_PRIO);
	if (OS_ERR_NONE != err_task0)
		while(1)
			;
	OSTaskDel(OS_PRIO_SELF);
}


void RCC_Configuration(void)
{
	ErrorStatus HSEStartUpStatus;
  	RCC_DeInit();
  	RCC_HSEConfig(RCC_HSE_ON);
  	HSEStartUpStatus = RCC_WaitForHSEStartUp();
  	if(HSEStartUpStatus == SUCCESS)
  	{
    	RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    	RCC_PCLK2Config(RCC_HCLK_Div1); 
    	RCC_PCLK1Config(RCC_HCLK_Div2);
    	RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    	RCC_PLLCmd(ENABLE);
    	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    	while(RCC_GetSYSCLKSource() != 0x08);
  	} 
}
void NVIC_DeInit(void) 
{ 
  u32 index = 0; 
   
  NVIC->ICER[0] = 0xFFFFFFFF; 
  NVIC->ICER[1] = 0x000007FF; 
  NVIC->ICPR[0] = 0xFFFFFFFF; 
  NVIC->ICPR[1] = 0x000007FF; 
   
  for(index = 0; index < 0x0B; index++) 
  { 
     NVIC->IP[index] = 0x00000000; 
  }  
}

#define IAP_ADDR  0x08000000

void IapProgramRun(void)

{
    void    (*pIapFun)(void);
		NVIC_DeInit();
    __set_PSP (*(INT32U *)IAP_ADDR);                      
		__set_CONTROL(0);
    __set_MSP (*(INT32U *)IAP_ADDR);                       
    pIapFun = (void (*)(void))(*(INT32U *)(IAP_ADDR + 4));             
    (*pIapFun) ();

}

int main(void)
{
	//RCC_Configuration();
	NVIC_DeInit();
	OSInit();
	UartInit(115200);
	uprintf("start");
	USART1_SendBuff = (u8 *)& micro2main;
	DMA1_USART1_Init();
	Sem      = OSSemCreate(0);
	Sem_recv = OSSemCreate(0);
	Sem_send = OSSemCreate(1);
	flashdata  = OSSemCreate(0);
	OSTaskCreate(startup_task, (void *)0,
	             &startup_task_stk[STARTUP_TASK_STK_SIZE - 1],
	             STARTUP_TASK_PRIO);
	NVIC_SetVectorTable (NVIC_VectTab_FLASH, APPPrartitionAddr);
	OSStart();
	return 0;
}
