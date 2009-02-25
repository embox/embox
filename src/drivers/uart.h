#ifndef _UART_H_
#define _UART_H_

int uart_init();
// output
void uart_putc(char ch);

// input
char uart_getc();


#endif // _UART_H_
