#ifndef _UART_H_
#define _UART_H_

int uart_init();
// output
void uart_putc(char ch);

// input
char uart_getc();

#ifdef IRQ_HANDLER
int uart_set_irq_handler(IRQ_HANDLER pfunc);
int uart_remove_irq_handler(IRQ_HANDLER pfunc);
#endif
#endif // _UART_H_
