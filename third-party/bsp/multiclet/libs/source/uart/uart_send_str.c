#include "uart.h"

void uart_send_str(char *str,UART_TypeDef *UART)
{
	int i;
	for (i=0;str[i]!='\0';i++)
	{
		while(UART_FIFO_TX_FULL(UART) == 1);
		UART_SEND_BYTE(str[i], UART);
	}

}
