#include "ds18b20.h"
#include "ucos_ii.h"
//#include "SysTickDelay.h"					 		   
//Mini STM32开发板
//DS18B20 驱动函数 
//正点原子@ALIENTEK
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
//IO方向设置
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
u32 reload=SysTick->LOAD; //LOAD的值        
ticks=nus*15;  //需要的节拍数      
tcnt=0; 
told=SysTick->VAL;         //刚进入时的计数器值 
while(1) 
{ 
	tnow=SysTick->VAL; 
	if(tnow!=told) 
	{      
		if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了. 
		else tcnt+=reload-tnow+told;      
		told=tnow; 
		if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出. 
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
//复位DS18B20
u8 DS18B20_Rst(void)	   
{   
	u8 retry=0;

	DS18B20_IO_OUT(); //SET PA0 OUTPUT
	Clr_DS18B20_DQ_OUT(); //拉低DQ
	delay_us(750);    //拉低750us
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


	
//从DS18B20读取一个位
//返回值：1/0
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
	
	
//从DS18B20读取一个字节
//返回值：读到的数据
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
//写一个字节到DS18B20
//dat：要写入的字节
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
u32 reload=SysTick->LOAD; //LOAD的值        
ticks=nus*43;  //需要的节拍数      
tcnt=0; 
told=SysTick->VAL;         //刚进入时的计数器值 
while(1) 
{ 
	tnow=SysTick->VAL; 
	if(tnow!=told) 
	{      
		if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了. 
		else tcnt+=reload-tnow+told;      
		told=tnow; 
		if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出. 
	}   
};       
} 
//延时nms 
//nms:要延时的ms数 
void delay_ms(u32 nms) 
{ 
  
	OS_EXIT_CRITICAL();
	OSTimeDly(nms); //ucos延时 
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


//搜索ROM函数
u8 Search_Rom(u8 ROM_ID[16][8])
{
	u8 r1=0,r2=0,m=0,n=0;
	u8 l=0;//栈长度
	u8 stack_node_pos[MAXNUM+1]={0};	//保存最后差异值位置 与栈顶值比较
	u8 node_pos=0; //产生差异值时的位置
	u8 rom[64]={0};//一次搜索中的位数
	u8 s=0;
	u8 num_18b20;
	OS_ENTER_CRITICAL();
	l=0;
	num_18b20=0;   //搜索前清0器件数目

	do
	{	
		DS18B20_Rst();
		DS18B20_Write_Byte(0xf0);

		for(m=0;m<8;m++)
		{	s=0;
			for(n=0;n<8;n++)
			{	
				s>>=1;
				r1=DS18B20_Read_Bit();//当前位
				r2=DS18B20_Read_Bit();//补码
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
				if(r1==0&&r2==0)  //有多个从机产生了差异值
				{ 	   
				  
				  node_pos=m*8+n+1;	 //记录产生差异值的位置
				  //差异值与最后一次差异值位置比较
				  if(node_pos>stack_node_pos[l]) //新路径 新的差异值
					{						
						DS18B20_Write_Bit(0);				   //选择发送0
						rom[(m*8+n)]=0;				   //记录								
						stack_node_pos[++l]=node_pos;  //记录新的差异值位置						
					}
					else if(node_pos<stack_node_pos[l])	//老路径
					{
						s=s|((rom[(m*8+n)]&0x01)<<7);	//发送保存上一次的位
						DS18B20_Write_Bit(rom[(m*8+n)]);
					}
					else if(node_pos==stack_node_pos[l]) //最后一次差异值的位置
					{   								 //最后一次分叉选择 发1 删除改栈
						s=s|0x80;
						DS18B20_Write_Bit(1);
						rom[(m*8+n)]=1;
						l=l-1;							 //从栈里删除该值
					}
				}
				else 
				{	//无从机连接 退出返回0
					OS_EXIT_CRITICAL();
					return 0;
				}
			}
			ROM_ID[num_18b20][m]=s;						 //循环8次保存一字节
		}
		num_18b20=num_18b20+1; 
	}while(stack_node_pos[l]!=0&&num_18b20<MAXNUM);	
	OS_EXIT_CRITICAL();	
   return 1;
}

//从ds18b20得到温度值-ROM方式，读取单总线上的多个18B20
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp_ROM(u8 rom[16][8],u8 index)
	{
	u8 temp;
	u8 TL,TH;
	short tem;
	OS_ENTER_CRITICAL();	
	DS18B20_Start ();                    // ds1820 start convert
	DS18B20_Rst();
	 
	DS18B20_Write_Byte(0x55);// 发送 rom 匹配
	for(temp=0;temp<8;temp++)  DS18B20_Write_Byte(rom[index][temp]); //发送64 bit ID
	DS18B20_Write_Byte(0x44);
	DS18B20_Rst();
 
	DS18B20_Write_Byte(0x55);// 发送 rom 匹配
	for(temp=0;temp<8;temp++)  DS18B20_Write_Byte(rom[index][temp]); //发送64 bit ID

	DS18B20_Write_Byte(0xbe);// convert	    
	TL=DS18B20_Read_Byte(); // LSB   
	TH=DS18B20_Read_Byte(); // MSB  
	
	if(TH>7)
		{
		TH=~TH;
		TL=~TL; 
		temp=0;//温度为负  
		}
	else temp=1;//温度为正	  	  
	tem=TH; //获得高八位
	tem<<=8;    
	tem+=TL;//获得底八位
	tem=(float)tem*0.625;//转换   
	OS_EXIT_CRITICAL();		
	if(temp)return tem; //返回温度值
	else return -tem;    
} 


//开始温度转换
void DS18B20_Start(void)// ds1820 start convert
{   						               
	DS18B20_Rst();	   

	DS18B20_Write_Byte(0xcc);// skip rom
	DS18B20_Write_Byte(0x44);// convert
} 
	
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init(void)
{
	return DS18B20_Rst();
}  

