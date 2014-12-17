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
**�������ƣ�void USART1_Config(void)
**�������ܣ�USART1�������ų�ʼ��
**�����������
**���ز�������
**�������ã��ڲ�����
**********************************************************************************/
static void USART1_Config(void)
{
   GPIO_InitTypeDef GPIO_InitStructure;
   USART_InitTypeDef USART_InitStructure;


   RCC_APB2PeriphClockCmd(USART1_RCC | RCC_APB2Periph_USART1, ENABLE);        //�򿪴���ʱ��
   
   
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

   //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);   //ʹ�ܴ���1�ж�

   USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);            //ʹ��USART1����DMA���� 
   USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);          //ʹ��USART1����DMA����
       
   USART_Cmd(USART1, ENABLE);                                                  //�������� 
}


/**********************************************************************************
**�������ƣ�void USART1_NVIC_Configuration(void)
**�������ܣ�USART1�����жϳ�ʼ��
**�����������
**���ز�������
**�������ã��ڲ�����
**********************************************************************************/
static void USART1_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
        
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                                      //���ڽ����жϴ�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = USART1_IRQn_Priority;  //�����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure);

}


/**********************************************************************************
**�������ƣ�void USART1_Init(void)
**�������ܣ�USART1���ڳ�ʼ��
**�����������
**���ز�������
**�������ã��ڲ�����
**********************************************************************************/
static void USART1_Init(void)
{
    USART1_Config();                    //USART1���Ź��ܳ�ʼ��

    USART1_NVIC_Configuration();        //USART1�жϳ�ʼ��
}

/**********************************************************************************
**�������ƣ�static void DMA1_USART1_TX_Configuration(void)
**�������ܣ�DMA1_USART1_TX��ʼ��
**�����������
**���ز�������
**�������ã��ⲿ����
**********************************************************************************/
static void DMA1_USART1_TX_Configuration(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);            //����DMAʱ�� 

        /*
         *  DMA���ã�
     *  ����DMAԴ���ڴ��ַ&�������ݼĴ�����ַ
     *  �����ڴ�-->����
     *  ÿ�δ���λ��8bit
     *  �����СDMA_BufferSize=SENDBUFF_SIZE
     *  ��ַ����ģʽ�������ַ�������ڴ��ַ����1
     *  DMAģʽ��һ�δ��䣬��ѭ��
     *  ���ȼ�����
         */    
    DMA_DeInit(DMA1_Channel4);                                         //����1��DMA����ͨ����ͨ��4
                                                                                                                                           
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);     //����DMAԴ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART1_SendBuff;           //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 //������ΪDMA��Ŀ�Ķ�
    DMA_InitStructure.DMA_BufferSize = USART1_SENDBUFF_SIZE;           //�����С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�����ַ������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ��ַ����1
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                //�������ݿ��8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                                //�ڴ����ݿ��8bit
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                   //DMA_Mode_Normal��ֻ����һ�Σ�, DMA_Mode_Circular ����ͣ�ش��ͣ�
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                //(DMA�������ȼ�Ϊ��)
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                           //���ڴ浽�ڴ�
    
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);

    DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);       //ʹ��DMAͨ��4��������ж�



}


/**********************************************************************************
**�������ƣ�static void DMA1_USART1_TX_NVIC_Configuration(void)
**�������ܣ�DMA�жϳ�ʼ��
**�����������
**���ز�������
**�������ã��ⲿ����
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
**�������ƣ�static void DMA1_USART1_TX_Init(void)
**�������ܣ�USART1���ڳ�ʼ��
**�����������
**���ز�������
**�������ã��ⲿ����
**********************************************************************************/
static void DMA1_USART1_TX_Init(void)
{
    DMA1_USART1_TX_Configuration();

    DMA1_USART1_TX_NVIC_Configuration();
}


/**********************************************************************************
**�������ƣ�static void DMA1_USART1_RX_Configuration(void)
**�������ܣ�DMA1_RX��ʼ��
**�����������
**���ز�������
**�������ã��ⲿ����
**********************************************************************************/
static void DMA1_USART1_RX_Configuration(void)
{        
        DMA_InitTypeDef DMA_InitStructure;

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

        DMA_DeInit(DMA1_Channel5);

        DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;           //DMA�������ַ
        DMA_InitStructure.DMA_MemoryBaseAddr = (u32)USART1_RecvBuff;                    //DMA�ڴ����ַ
        DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                                             //������Ϊ���ݴ������Դ
        DMA_InitStructure.DMA_BufferSize = USART1_RECVBUFF_SIZE;                            //DMA�����С
        DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;            //�����ַ�Ĵ�������
        DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                             //�ڴ��ַ�Ĵ�������
        DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//�������ݿ��8bit 
        DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;            //�ڴ����ݿ��8bit
        DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                            //ѭ��ģʽ
        DMA_InitStructure.DMA_Priority = DMA_Priority_High;                                //���ȼ�����
        DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                                //���ڴ浽�ڴ�

        DMA_Init(DMA1_Channel5, &DMA_InitStructure);                                                   //��ʼ��DMA

        DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE); //ʹ��DMAͨ��5��������ж�

}


/**************************************************************
** �������� :DMA1_USART1_RX_NVIC_Configuration(void)
** ����˵�� :NVIC��ʼ��
** ������� :��
** ������� :��
** ���ز��� :��
** ע    �� :��
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
**�������ƣ�void DMA1_USART1_RX_Init(void)
**�������ܣ�USART1���ڳ�ʼ��
**�����������
**���ز�������
**�������ã��ⲿ����
**********************************************************************************/
static void DMA1_USART1_RX_Init(void)
{
        DMA1_USART1_RX_Configuration();

        DMA1_USART1_RX_NVIC_Configuration();
}


/**********************************************************************************
**�������ƣ�static void DMA1_USART1_Init(void)
**�������ܣ�DMA1���ڳ�ʼ��
**�����������
**���ز�������
**�������ã��ⲿ����
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
**�������ƣ�void USART1_Send_Buf(u8 *buf , unsigned int len)
**�������ܣ�ͨ��DMA���ʹ�������
**���������data -- ��������
**���ز�����0--���Ͳ��ɹ�
**          1--���ͳɹ�
**�������ã��ⲿ����
**********************************************************************************/
void USART1_Send_Buf(u8 *buf , unsigned int len)
{
        DMA_InitTypeDef DMA_InitStructure;

        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);            //����DMAʱ�� 

        /*
         *  DMA���ã�
     *  ����DMAԴ���ڴ��ַ&�������ݼĴ�����ַ
     *  �����ڴ�-->����
     *  ÿ�δ���λ��8bit
     *  �����СDMA_BufferSize=SENDBUFF_SIZE
     *  ��ַ����ģʽ�������ַ�������ڴ��ַ����1
     *  DMAģʽ��һ�δ��䣬��ѭ��
     *  ���ȼ�����
         */    
    DMA_DeInit(DMA1_Channel4);                                         //����1��DMA����ͨ����ͨ��4
                                                                                                                                           
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART1->DR);     //����DMAԴ
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)buf;                               //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 //������ΪDMA��Ŀ�Ķ�
    DMA_InitStructure.DMA_BufferSize = len;                            //�����С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�����ַ������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ��ַ����1
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;                //�������ݿ��8bit
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;                                //�ڴ����ݿ��8bit
    DMA_InitStructure.DMA_Mode =  DMA_Mode_Normal;                     //DMA_Mode_Normal��ֻ����һ�Σ�, DMA_Mode_Circular ����ͣ�ش��ͣ�
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;                //(DMA�������ȼ�Ϊ��)
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                                           //���ڴ浽�ڴ�
    
        DMA_Init(DMA1_Channel4, &DMA_InitStructure);                  

        DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);       //ʹ��DMAͨ��4��������ж� 
                                      
        DMA_Cmd(DMA1_Channel4, ENABLE);
         
}
