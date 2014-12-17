#include "ds18b20.h"
#include "ucos_ii.h"
//#include "SysTickDelay.h"					 		   
//Mini STM32������
//DS18B20 �������� 
//����ԭ��@ALIENTEK
//2010/6/17	 
//extern void          OSTimeDly             (INT16U           ticks);
#define MAXNUM 16
#define DS18B20_DQ_IN_PORT              GPIOC
#define DS18B20_DQ_IN_CLK               RCC_APB2Periph_GPIOC  
#define DS18B20_DQ_OUT_PORT              GPIOC
#define DS18B20_DQ_OUT_CLK               RCC_APB2Periph_GPIOC


#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)  MEM_ADDR( BITBAND(addr, bitnum)  )
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008

static INT32U cpu_sr;
static u16 * DQ_index;
//IO��������
#if 0
void DS18B20_IO_IN()
{
	if(*DQ_index<8){
		GPIOC->CRL&=~(15<<*DQ_index);
		GPIOC->CRL|=8<<*DQ_index;
	}else{
		GPIOC->CRH&=~(8<<*DQ_index);
		GPIOC->CRH|=8<<*DQ_index;
	}
}
void DS18B20_IO_OUT()
{
	if(*DQ_index<8){
		GPIOC->CRL&=~(15<<*DQ_index);
		GPIOC->CRL|=3<<*DQ_index;
	}else{
		GPIOC->CRH&=~(15<<*DQ_index);
		GPIOC->CRH|=3<<*DQ_index;
	}
}
#else
void DS18B20_IO_IN()
{
}
void DS18B20_IO_OUT()
{
}
#endif

void Set_DS18B20_DQ_OUT()
{
	//GPIO_SetBits(DS18B20_DQ_OUT_PORT,0x0001<<*DQ_index);
	BIT_ADDR(GPIOC_ODR_Addr, *DQ_index)  = 1;
}
void Clr_DS18B20_DQ_OUT()
{
	//GPIO_ResetBits(DS18B20_DQ_OUT_PORT,0x0001<<*DQ_index);
	BIT_ADDR(GPIOC_ODR_Addr, *DQ_index)  = 0;
} 


u8 DS18B20_DQ_IN()
{
	//return GPIO_ReadInputDataBit(DS18B20_DQ_IN_PORT, 0x0001<<*DQ_index);
	return BIT_ADDR(GPIOC_IDR_Addr, *DQ_index);
}
// 'tick' values
#if 0
int A,B,C,D,E,F,G,H,I,J;

//-----------------------------------------------------------------------------
// Set the 1-Wire timing to 'standard' (standard=1) or 'overdrive' (standard=0).
//
void SetSpeed(int standard)
{
	switch(standard){
		case 0:
			A = 6 * 4;
			B = 64 * 4;
			C = 60 * 4;
			D = 10 * 4;
			E = 9 * 4;
			F = 55 * 4;
			G = 0;
			H = 480 * 4;
			I = 70 * 4;
			J = 410 * 4;
			break;
		case 1:
			A = 1.5 * 4;
			B = 7.5 * 4;
			C = 7.5 * 4;
			D = 2.5 * 4;
			E = 0.75 * 4;
			F = 7 * 4;
			G = 2.5 * 4;
			H = 70 * 4;
			I = 8.5 * 4;
			J = 40 * 4;
			break;
		case 2:
			A = 3.75 * 4;
			B = 8.5 * 4;
			C = 8.5 * 4;
			D = 3.0 * 4;
			E = 2.25 * 4;
			F = 7 * 4;
			G = 2.5 * 4;
			H = 95 * 4;
			I = 8.5 * 4;
			J = 70 * 4;
			break;
	}
}

void tickDelay_0(u32 nus) 
{ 
u32 ticks; 
u32 told,tnow,tcnt=0; 
u32 reload=SysTick->LOAD; //LOAD��ֵ        
ticks=nus*15;  //��Ҫ�Ľ�����      
tcnt=0; 
told=SysTick->VAL;         //�ս���ʱ�ļ�����ֵ 
while(1) 
{ 
	tnow=SysTick->VAL; 
	if(tnow!=told) 
	{      
		if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����. 
		else tcnt+=reload-tnow+told;      
		told=tnow; 
		if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�. 
	}   
};       
} 
void tickDelay(int nus) 
{ 
	int count = nus*3;
	while(count--);  
}

//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 1 if no presence detect was found,
// return 0 otherwise.
// (NOTE: Does not handle alarm presence from DS2404/DS1994)
//
u8 DS18B20_Rst(void)	
{
        u8 result;
        tickDelay(G);
        Clr_DS18B20_DQ_OUT(); // Drives DQ low
        tickDelay(H);
        Set_DS18B20_DQ_OUT(); // Releases the bus
        tickDelay(I);
        result = DS18B20_DQ_IN() ^ 0x01; // Sample for presence pulse from slave
        tickDelay(J); // Complete the reset sequence recovery
        return result; // Return sample presence pulse result
}

//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void DS18B20_Write_Bit(u8 dat)
{
        if (dat)
        {
                // Write '1' bit
                Clr_DS18B20_DQ_OUT(); // Drives DQ low
                tickDelay(A);
                Set_DS18B20_DQ_OUT(); // Releases the bus
                tickDelay(B); // Complete the time slot and 10us recovery
        }
        else
        {
                // Write '0' bit
                Clr_DS18B20_DQ_OUT(); // Drives DQ low
                tickDelay(C);
                Set_DS18B20_DQ_OUT(); // Releases the bus
                tickDelay(D);
        }
}

//-----------------------------------------------------------------------------
// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
//
u8 DS18B20_Read_Bit(void)
{
        u8 result;

        Clr_DS18B20_DQ_OUT(); // Drives DQ low
        tickDelay(A);
        Set_DS18B20_DQ_OUT(); // Releases the bus
        tickDelay(E);
        result = DS18B20_DQ_IN() & 0x01; // Sample the bit value from the slave
        tickDelay(F); // Complete the time slot and 10us recovery

        return result;
}

//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void DS18B20_Write_Byte(u8 dat) 
{
        u8 loop;

        // Loop to write each bit in the byte, LS-bit first
        for (loop = 0; loop < 8; loop++)
        {
                DS18B20_Write_Bit(dat & 0x01);

                // shift the data byte for the next bit
                dat >>= 1;
        }
}

//-----------------------------------------------------------------------------
// Read 1-Wire data byte and return it
//
u8 DS18B20_Read_Byte(void)

{
        u8  loop, result=0;

        for (loop = 0; loop < 8; loop++)
        {
                // shift the result to get it ready for the next bit
                result >>= 1;

                // if result is one, then set MS bit
                if (DS18B20_Read_Bit())
                        result |= 0x80;
        }
        return result;
}
void test_DQ_clk()
{
	OS_ENTER_CRITICAL();
	while(1)
	{
		BIT_ADDR(GPIOC_ODR_Addr, *DQ_index)  = 0;
		tickDelay(A);
		BIT_ADDR(GPIOC_ODR_Addr, *DQ_index)  = 1;
		tickDelay(B);
	}
	OS_EXIT_CRITICAL();	
}


#else
//��λDS18B20
u8 DS18B20_Rst(void)	   
{   
	u8 retry=0;

	DS18B20_IO_OUT(); //SET PA0 OUTPUT
	Clr_DS18B20_DQ_OUT(); //����DQ
	delay_us(750);    //����750us
	Set_DS18B20_DQ_OUT(); //DQ=1 
	delay_us(12);     //15US
	DS18B20_IO_IN();//SET PA0 INPUT	 
	while (DS18B20_DQ_IN()&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200) return 1;
	else retry=0;
	while (!DS18B20_DQ_IN()&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}


	
//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
	u8 data;
	DS18B20_IO_OUT();//SET PA0 OUTPUT
	Clr_DS18B20_DQ_OUT(); 
	delay_us(1);
	Set_DS18B20_DQ_OUT(); 
	DS18B20_IO_IN();//SET PA0 INPUT
	delay_us(10);
	if(DS18B20_DQ_IN())data=1;
	else data=0;	 
	delay_us(50);           
	return data;
}
	
	
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)    // read one byte
{        
	u8 i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
		j=DS18B20_Read_Bit();
		dat=(j<<7)|(dat>>1);
	}						    
	return dat;
}
void DS18B20_Write_Bit(u8 dat)
{
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
	if (dat) 
	{
		Clr_DS18B20_DQ_OUT();// Write 1
		delay_us(1);                            
		Set_DS18B20_DQ_OUT();
		delay_us(60);             
	}
	else 
	{
		Clr_DS18B20_DQ_OUT();// Write 0
		delay_us(60);             
		Set_DS18B20_DQ_OUT();
		delay_us(1);                          
	}
}	
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)     
{             
	u8 j;
	u8 testb;
	DS18B20_IO_OUT();//SET PA0 OUTPUT;
	for (j=1;j<=8;j++) 
		{
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) 
			{
			Clr_DS18B20_DQ_OUT();// Write 1
			delay_us(1);                            
			Set_DS18B20_DQ_OUT();
			delay_us(60);             
			}
		else 
			{
			Clr_DS18B20_DQ_OUT();// Write 0
			delay_us(60);             
			Set_DS18B20_DQ_OUT();
			delay_us(1);                          
			}
		}
}
#endif


void delay_us_0(u32 nus) 
{ 
    int count = nus*11;
	while(count--);  
} 

void delay_us(u32 nus) 
{ 
u32 ticks; 
u32 told,tnow,tcnt=0; 
u32 reload=SysTick->LOAD; //LOAD��ֵ        
ticks=nus*43;  //��Ҫ�Ľ�����      
tcnt=0; 
told=SysTick->VAL;         //�ս���ʱ�ļ�����ֵ 
while(1) 
{ 
	tnow=SysTick->VAL; 
	if(tnow!=told) 
	{      
		if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����. 
		else tcnt+=reload-tnow+told;      
		told=tnow; 
		if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�. 
	}   
};       
} 
//��ʱnms 
//nms:Ҫ��ʱ��ms�� 
void delay_ms(u32 nms) 
{ 
  
	OS_EXIT_CRITICAL();
	OSTimeDly(nms); //ucos��ʱ 
	OS_ENTER_CRITICAL();
	//delay_us(1000*nms);
} 

void selectDQ(u16 *Pin)
{
		DQ_index = Pin;
}

void test_DQ_clk()
{
	while(1){
		Clr_DS18B20_DQ_OUT();
		delay_us(2);
		Set_DS18B20_DQ_OUT();
		delay_us(12);
	}
}


//����ROM����
u8 Search_Rom(u8 ROM_ID[16][8])
{
	u8 r1=0,r2=0,m=0,n=0;
	u8 l=0;//ջ����
	u8 stack_node_pos[MAXNUM+1]={0};	//����������ֵλ�� ��ջ��ֵ�Ƚ�
	u8 node_pos=0; //��������ֵʱ��λ��
	u8 rom[64]={0};//һ�������е�λ��
	u8 s=0;
	u8 num_18b20;
	OS_ENTER_CRITICAL();
	l=0;
	num_18b20=0;   //����ǰ��0������Ŀ

	do
	{	
		DS18B20_Rst();
		DS18B20_Write_Byte(0xf0);

		for(m=0;m<8;m++)
		{	s=0;
			for(n=0;n<8;n++)
			{	
				s>>=1;
				r1=DS18B20_Read_Bit();//��ǰλ
				r2=DS18B20_Read_Bit();//����
				if(r1==0&&r2==1)
				{	
					 DS18B20_Write_Bit(0);
	    			 rom[(m*8+n)]=0;
				}
				else
				if(r1==1&&r2==0)
				{	
					s=s|0x80;
					DS18B20_Write_Bit(1);
					rom[(m*8+n)]=1;
				}
				else
				if(r1==0&&r2==0)  //�ж���ӻ������˲���ֵ
				{ 	   
				  
				  node_pos=m*8+n+1;	 //��¼��������ֵ��λ��
				  //����ֵ�����һ�β���ֵλ�ñȽ�
				  if(node_pos>stack_node_pos[l]) //��·�� �µĲ���ֵ
					{						
						DS18B20_Write_Bit(0);				   //ѡ����0
						rom[(m*8+n)]=0;				   //��¼								
						stack_node_pos[++l]=node_pos;  //��¼�µĲ���ֵλ��						
					}
					else if(node_pos<stack_node_pos[l])	//��·��
					{
						s=s|((rom[(m*8+n)]&0x01)<<7);	//���ͱ�����һ�ε�λ
						DS18B20_Write_Bit(rom[(m*8+n)]);
					}
					else if(node_pos==stack_node_pos[l]) //���һ�β���ֵ��λ��
					{   								 //���һ�ηֲ�ѡ�� ��1 ɾ����ջ
						s=s|0x80;
						DS18B20_Write_Bit(1);
						rom[(m*8+n)]=1;
						l=l-1;							 //��ջ��ɾ����ֵ
					}
				}
				else 
				{	//�޴ӻ����� �˳�����0
					OS_EXIT_CRITICAL();
					return 0;
				}
			}
			ROM_ID[num_18b20][m]=s;						 //ѭ��8�α���һ�ֽ�
		}
		num_18b20=num_18b20+1; 
	}while(stack_node_pos[l]!=0&&num_18b20<MAXNUM);	
	OS_EXIT_CRITICAL();	
   return 1;
}

//��ds18b20�õ��¶�ֵ-ROM��ʽ����ȡ�������ϵĶ��18B20
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
short DS18B20_Get_Temp_ROM(u8 rom[16][8],u8 index)
	{
	u8 temp;
	u8 TL,TH;
	short tem;
	OS_ENTER_CRITICAL();	
	DS18B20_Start ();                    // ds1820 start convert
	DS18B20_Rst();
	 
	DS18B20_Write_Byte(0x55);// ���� rom ƥ��
	for(temp=0;temp<8;temp++)  DS18B20_Write_Byte(rom[index][temp]); //����64 bit ID
	DS18B20_Write_Byte(0x44);
	DS18B20_Rst();
 
	DS18B20_Write_Byte(0x55);// ���� rom ƥ��
	for(temp=0;temp<8;temp++)  DS18B20_Write_Byte(rom[index][temp]); //����64 bit ID

	DS18B20_Write_Byte(0xbe);// convert	    
	TL=DS18B20_Read_Byte(); // LSB   
	TH=DS18B20_Read_Byte(); // MSB  
	
	if(TH>7)
		{
		TH=~TH;
		TL=~TL; 
		temp=0;//�¶�Ϊ��  
		}
	else temp=1;//�¶�Ϊ��	  	  
	tem=TH; //��ø߰�λ
	tem<<=8;    
	tem+=TL;//��õװ�λ
	tem=(float)tem*0.625;//ת��   
	OS_EXIT_CRITICAL();		
	if(temp)return tem; //�����¶�ֵ
	else return -tem;    
} 


//��ʼ�¶�ת��
void DS18B20_Start(void)// ds1820 start convert
{   						               
	DS18B20_Rst();	   

	DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0x44);// convert
} 
	
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init(void)
{
	return DS18B20_Rst();
}  

