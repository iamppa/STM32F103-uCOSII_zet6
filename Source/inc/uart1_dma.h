#ifndef __UART1_DMA_H__
#define __UART1_DMA_H__

#include "stm32f10x.h"
#include "ucos_ii.h"
#include "cabinet.h"


#define USART1_SENDBUFF_SIZE 28
#define USART1_RECVBUFF_SIZE 10

extern u8 * USART1_SendBuff;
extern u8 USART1_RecvBuff[USART1_RECVBUFF_SIZE];

extern u8 Flag_Uart_Send;

extern OS_EVENT *Sem_recv;
extern OS_EVENT *Sem_send;

static void USART1_Config(void);
static void USART1_NVIC_Configuration(void);
static void USART1_Init(void);
static void DMA1_USART1_TX_Configuration(void);
static void DMA1_USART1_TX_NVIC_Configuration(void);
static void DMA1_USART1_TX_Init(void);
static void DMA1_USART1_RX_Configuration(void);
static void DMA1_USART1_RX_NVIC_Configuration(void);
static void DMA1_USART1_RX_Init(void);
void DMA1_USART1_Init(void);
void DMA1_USART1_RX_reset(void);
	

#endif
