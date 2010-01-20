/**
 * \file uart.h
 */

#ifndef _UART_H_
#define _UART_H_

#include "kernel/irq.h"

/*
 * Init UART subsystem.
 */
int uart_init(void);

/**
 * output: write character via uart.
 */
void uart_putc(char ch);

/**
 * input: read character via uart.
 */
char uart_getc(void);


int uart_set_irq_handler(IRQ_HANDLER pfunc);
int uart_remove_irq_handler(void);


#endif /* _UART_H_ */
