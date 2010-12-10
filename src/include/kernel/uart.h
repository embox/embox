/**
 * @file
 *
 * @date 07.07.10
 * @author Fedor Burdun
 */

#ifndef UART_H_
#define UART_H_

extern int uart_init(void);

extern void uart_putc(char ch);

extern char uart_getc(void);

/*
int uart_set_irq_handler(irq_handler_t pfunc);

int uart_remove_irq_handler(void) ;
*/

#endif /* UART_H_ */
