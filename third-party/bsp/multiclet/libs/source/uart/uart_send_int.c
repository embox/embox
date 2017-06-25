#include "uart.h"

void uart_send_int(int str, UART_TypeDef *UART)
{
	int i;
	for(i=0;i<4;i++)
	{
        while(UART_FIFO_TX_FULL(UART) == 1);
		UART_SEND_BYTE(str, UART);
		str = str >> 8;
	}
}
