#include "uart1_dma.h"


#define USART1_TX GPIO_Pin_9
#define USART1_RX GPIO_Pin_10
#define USART1_PORT GPIOA
#define USART1_RCC RCC_APB2Periph_GPIOA

#define BOOTRATE 115200
#define USART1_IRQn_Priority 0

u8 * USART1_SendBuff = 0;
u8 USART1_RecvBuff[USART1_RECVBUFF_SIZE];

u8 Flag_Uart_Send = 0;


OS_EVENT *Sem_recv;
OS_EVENT *Sem_send;


/**********************************************************************************
**函数名称：void USART1_Config(void)
**函数功能：USART1串口引脚初始化
**输入参数：无
**返回参数：无
**函数调用：内部调用
**********************************************************************************/
static void USART1_Config(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;


   RCC_APB2PeriphClockCmd(USART1_RCC | RCC_APB2Periph_USART1, ENABLE);        //打开串口时钟
   
   
   /* USART1 GPIO config */
   /* Configure USART1 Tx (PA.9) as alternate function push-pull */
   GPIO_InitStructure.GPIO_Pin   = USART1_TX;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_Init(USART1_PORT, &GPIO_InitStructure); 
   
   /* Configure USART1 Rx (PA.10) as input floating */
   GPIO_InitStructure.GPIO_Pin  = USART1_RX;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
   GPIO_Init(USART1_PORT, &GPIO_InitStructure);
   
   /* USART1 mode config */
   USART_InitStructure.USART_BaudRate   = BOOTRATE;
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;
   USART_InitStructure.USART_StopBits   = USART_StopBits_1;
   USART_InitStructure.USART_Parity     = USART_Parity_No ;
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
   USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
   
   USART_Init(USART1, &USART_InitStructure); 

   //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   //使能串口1中断

   USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);            //使能USART1发送DMA请求 
   USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);          //使能USART1接收DMA请求
       
   USART_Cmd(USART1, ENABLE);                                                  //启动串口 
}


/**********************************************************************************
**函数名称：void USART1_NVIC_Configuration(void)
**函数功能：USART1串口中断初始化
**输入参数：无
**返回参数：无
**函数调用：内部调用
**********************************************************************************/
static void USART1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
        
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                                      //串口接收中断打开
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART1_IRQn_Priority;  //设置中断优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

}


/**********************************************************************************
**函数名称：void USART1_Init(void)
**函数功能：USART1串口初始化
**输入参数：无
**返回参数：无
**函数调用：内部调用
**********************************************************************************/
static void USART1_Init(void)
{
    USART1_Config();                    //USART1引脚功能初始化

    USART1_NVIC_Configuration();        //USART1中断初始化
}

/**********************************************************************************
**函数名称：static void DMA1_USART1_TX_Configuration(void)
**函数功能：DMA1_USART1_TX初始化
**输入参数：无
**返回参数：无
**函数调用：外部调用
**********************************************************************************/
static void DMA1_USART1_TX_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);            //开启DMA时钟 

        /*
         *  DMA设置：
     *  设置DMA源：内存地址&串口数据寄存器地址
     *  方向：内存-->外设
     *  每次传输位：8bit
     *  传输大小DMA_BufferSize=SENDBUFF_SIZE
     *  地址自增模式：外设地址不增，内存地址自增1
     *  DMA模式：一次传输，非循环
     *  优先级：高
         */    
    DMA_DeInit(DMA1_Channel4);                                         //串口1的DMA传输通道是通道4
                                                                                                                                           
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);     //设置DMA源
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART1_SendBuff;           //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 //外设作为DMA的目的端
    DMA_InitStructure.DMA_BufferSize = USART1_SENDBUFF_SIZE;           //传输大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //内存地址自增1
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                //外设数据宽度8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                                //内存数据宽度8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                   //DMA_Mode_Normal（只传送一次）, DMA_Mode_Circular （不停地传送）
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                //(DMA传送优先级为高)
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                           //非内存到内存
    
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);       //使能DMA通道4传输完成中断



}


/**********************************************************************************
**函数名称：static void DMA1_USART1_TX_NVIC_Configuration(void)
**函数功能：DMA中断初始化
**输入参数：无
**返回参数：无
**函数调用：外部调用
**********************************************************************************/
static void DMA1_USART1_TX_NVIC_Configuration(void)
{
   NVIC_InitTypeDef NVIC_InitStructure;
   
   NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;  
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  
   NVIC_Init(&NVIC_InitStructure);

}


/**********************************************************************************
**函数名称：static void DMA1_USART1_TX_Init(void)
**函数功能：USART1串口初始化
**输入参数：无
**返回参数：无
**函数调用：外部调用
**********************************************************************************/
static void DMA1_USART1_TX_Init(void)
{
    DMA1_USART1_TX_Configuration();

    DMA1_USART1_TX_NVIC_Configuration();
}


/**********************************************************************************
**函数名称：static void DMA1_USART1_RX_Configuration(void)
**函数功能：DMA1_RX初始化
**输入参数：无
**返回参数：无
**函数调用：外部调用
**********************************************************************************/
static void DMA1_USART1_RX_Configuration(void)
{        
        DMA_InitTypeDef DMA_InitStructure;

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        DMA_DeInit(DMA1_Channel5);

        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;           //DMA外设基地址
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART1_RecvBuff;                    //DMA内存基地址
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                             //外设作为数据传输的来源
        DMA_InitStructure.DMA_BufferSize = USART1_RECVBUFF_SIZE;                            //DMA缓存大小
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //外设地址寄存器不变
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                             //内存地址寄存器递增
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度8bit 
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;            //内存数据宽度8bit
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                            //循环模式
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;                                //优先级：高
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                                //非内存到内存

        DMA_Init(DMA1_Channel5, &DMA_InitStructure);                                                   //初始化DMA

        DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE); //使能DMA通道5传输完成中断

}


/**************************************************************
** 函数名称 :DMA1_USART1_RX_NVIC_Configuration(void)
** 功能说明 :NVIC初始化
** 输入参数 :无
** 输出参数 :无
** 返回参数 :无
** 注    意 :无
***************************************************************/
static void DMA1_USART1_RX_NVIC_Configuration(void)
{
        NVIC_InitTypeDef NVIC_InitStructure;

        NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
        NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

        NVIC_Init(&NVIC_InitStructure);        
}


/**********************************************************************************
**函数名称：void DMA1_USART1_RX_Init(void)
**函数功能：USART1串口初始化
**输入参数：无
**返回参数：无
**函数调用：外部调用
**********************************************************************************/
static void DMA1_USART1_RX_Init(void)
{
        DMA1_USART1_RX_Configuration();

        DMA1_USART1_RX_NVIC_Configuration();
}


/**********************************************************************************
**函数名称：static void DMA1_USART1_Init(void)
**函数功能：DMA1串口初始化
**输入参数：无
**返回参数：无
**函数调用：外部调用
**********************************************************************************/
void DMA1_USART1_Init(void)
{
	USART1_Init();
	DMA1_USART1_TX_Init();
	DMA1_USART1_RX_Init();
}


void DMA1_USART1_RX_reset(void)
{
	DMA_Cmd(DMA1_Channel5,DISABLE);
	USART1_Init();
	DMA_ClearFlag(DMA1_FLAG_TC5);
	USART_ClearFlag(USART1,USART_FLAG_TC);
	//DMA_ClearFlag(DMA1_FLAG_GL5);
	//DMA_ClearFlag(DMA1_FLAG_HT5);
	//DMA_ClearFlag(DMA1_FLAG_TE5);
	DMA_SetCurrDataCounter(DMA1_Channel5,USART1_RECVBUFF_SIZE);
}

/**********************************************************************************
**函数名称：void USART1_Send_Buf(u8 *buf , unsigned int len)
**函数功能：通过DMA发送串口数据
**输入参数：data -- 发送数据
**返回参数：0--发送不成功
**          1--发送成功
**函数调用：外部调用
**********************************************************************************/
void USART1_Send_Buf(u8 *buf , unsigned int len)
{
        DMA_InitTypeDef DMA_InitStructure;

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);            //开启DMA时钟 

        /*
         *  DMA设置：
     *  设置DMA源：内存地址&串口数据寄存器地址
     *  方向：内存-->外设
     *  每次传输位：8bit
     *  传输大小DMA_BufferSize=SENDBUFF_SIZE
     *  地址自增模式：外设地址不增，内存地址自增1
     *  DMA模式：一次传输，非循环
     *  优先级：高
         */    
    DMA_DeInit(DMA1_Channel4);                                         //串口1的DMA传输通道是通道4
                                                                                                                                           
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);     //设置DMA源
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buf;                               //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 //外设作为DMA的目的端
    DMA_InitStructure.DMA_BufferSize = len;                            //传输大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设地址不增加
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //内存地址自增1
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                //外设数据宽度8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                                //内存数据宽度8bit
    DMA_InitStructure.DMA_Mode =  DMA_Mode_Normal;                     //DMA_Mode_Normal（只传送一次）, DMA_Mode_Circular （不停地传送）
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                //(DMA传送优先级为高)
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                           //非内存到内存
    
        DMA_Init(DMA1_Channel4, &DMA_InitStructure);                  

        DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);       //使能DMA通道4传输完成中断 
                                      
        DMA_Cmd(DMA1_Channel4, ENABLE);
         
}
