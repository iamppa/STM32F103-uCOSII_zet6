#include "stm32f10x.h"
#include "uart_printf.h"


void UartInit (int USART_BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
   
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3 ,ENABLE);		  //		GPIO_FullRemap_USART3
	/* Configure USART3 Tx (PC10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/* Configure USART3 Rx (PC11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);



	USART_InitStructure.USART_BaudRate = USART_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(printf_uart, &USART_InitStructure);

	//USART_ITConfig(printf_uart, USART_IT_RXNE, ENABLE);
	USART_Cmd(printf_uart, ENABLE);
}

int fputc(int ch)

{
	USART_SendData(printf_uart, (unsigned char) ch);
	while (!(printf_uart->SR & USART_FLAG_TXE));
	return (ch);
}

int fgetc()
{
	while (USART_GetFlagStatus(printf_uart, USART_FLAG_RXNE) == RESET)
	{}
	return (char) USART_ReceiveData(printf_uart);
}



int GetKey (void) {

while (!(printf_uart->SR & USART_FLAG_RXNE));

return ((int)(printf_uart->DR & 0x1FF));

}
void uprintf(char * str){
	while(*str !='\0')
	{
		USART_SendData(printf_uart, (unsigned char) *str);
		while (!(printf_uart->SR & USART_FLAG_TXE));
		str++;
	}
	USART_SendData(printf_uart,0x0D);
	while (!(printf_uart->SR & USART_FLAG_TXE));
	USART_SendData(printf_uart,0x0A);
	while (!(printf_uart->SR & USART_FLAG_TXE));

}

