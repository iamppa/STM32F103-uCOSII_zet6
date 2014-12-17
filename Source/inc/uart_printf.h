#ifndef _UART_PRINTF_H_
#define _UART_PRINTF_H_
#include "ucos_ii.h"

#define printf_uart USART3

void UartInit(int USART_BaudRate);
int fgetc();
int fputc(int ch);

void uprintf(char * str);


#endif
