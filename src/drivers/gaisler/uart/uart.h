/**
 * \file uart.h
 */

#ifndef _UART_H_
#define _UART_H_

typedef struct _UART_STRUCT {
	volatile UINT32 data; //0x70
	volatile UINT32 status;
	volatile UINT32 ctrl;
	volatile UINT32 scaler;
} UART_STRUCT;

int uart_init();

/**
 * output: write character via uart.
 */
void uart_putc(char ch);

/**
 * input: read character via uart.
 */
char uart_getc();

#ifdef IRQ_HANDLER
int uart_set_irq_handler(IRQ_HANDLER pfunc);
int uart_remove_irq_handler(IRQ_HANDLER pfunc);
#endif

#endif /* _UART_H_ */
