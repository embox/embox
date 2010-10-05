/**
 * @file
 *
 * @date 07.07.10
 * @author Fedor Burdun
 */

#ifndef __UART_H_

int uart_init(void);

void uart_putc(char ch);

char uart_getc(void);

/*
int uart_set_irq_handler(irq_handler_t pfunc);

int uart_remove_irq_handler(void) ;
*/

#endif /* __UART_H_ */
