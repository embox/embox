#include "uart.h"

void uart_init(UART_TypeDef *UART, UART_InitTypeDef* UART_InitStruct)
{
	int control;
	//Check parameters and set default value
	if((UART_InitStruct->Mode) != 0 && (UART_InitStruct->Mode) != 1 && (UART_InitStruct->Mode) != 2 && (UART_InitStruct->Mode) != 3) UART_InitStruct->Mode = 3;
	if((UART_InitStruct->Irq_get_byte) != 0 && (UART_InitStruct->Irq_get_byte) != 1) UART_InitStruct->Irq_get_byte = 0;
	if((UART_InitStruct->Irq_send_byte) != 0 && (UART_InitStruct->Irq_send_byte) != 1) UART_InitStruct->Irq_send_byte = 0;
	if((UART_InitStruct->TypeParity) != 0 && (UART_InitStruct->TypeParity) != 1) UART_InitStruct->TypeParity = 0;
	if((UART_InitStruct->Parity) != 0 && (UART_InitStruct->Parity) != 1) UART_InitStruct->Parity = 0;
	if((UART_InitStruct->FlowControl) != 0 && (UART_InitStruct->FlowControl) != 1) UART_InitStruct->FlowControl = 0;
	if((UART_InitStruct->Loop) != 0 && (UART_InitStruct->Loop) != 1) UART_InitStruct->Loop = 0;
	if((UART_InitStruct->Irq_fifo_t) != 0 && (UART_InitStruct->Irq_fifo_t) != 1) UART_InitStruct->Irq_fifo_t = 0;
	if((UART_InitStruct->Irq_fifo_r) != 0 && (UART_InitStruct->Irq_fifo_r) != 1) UART_InitStruct->Irq_fifo_r = 0;
	if((UART_InitStruct->Irq_get_break) != 0 && (UART_InitStruct->Irq_get_break) != 1) UART_InitStruct->Irq_get_break = 0;
	if((UART_InitStruct->Irq_wait_r) != 0 && (UART_InitStruct->Irq_wait_r) != 1) UART_InitStruct->Irq_wait_r = 0;
	if((UART_InitStruct->Irq_shift_empty) != 0 && (UART_InitStruct->Irq_shift_empty) != 1) UART_InitStruct->Irq_shift_empty = 0;

	control = (UART_InitStruct->Mode)|(UART_InitStruct->Irq_get_byte << 2)|(UART_InitStruct->Irq_send_byte << 3)|
	(UART_InitStruct->TypeParity << 4)|(UART_InitStruct->Parity << 5)|(UART_InitStruct->FlowControl << 6)|(UART_InitStruct->Loop << 7)|
	(UART_InitStruct->Irq_fifo_t << 9)|(UART_InitStruct->Irq_fifo_r << 10)|(UART_InitStruct->Irq_get_break << 12)|
	(UART_InitStruct->Irq_wait_r << 13)|(UART_InitStruct->Irq_shift_empty << 14); //make control reg

	UART->BDR = (UART_InitStruct->System_freq) / (UART_InitStruct->BaudRate * 8 - 1); //38400 bps e.g. //set baudrate
	UART->CR = control; //set control register
}
