#include "uart.h"

void uart_send_char(unsigned char byte, UART_TypeDef *UART)
{
        while(UART_FIFO_TX_FULL(UART) == 1);
		UART_SEND_BYTE(byte, UART);
}
