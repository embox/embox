#ifndef _UART_H_
#define _UART_H_


// output
void uart_putc(char ch);

BOOL uart_is_empty ();
// input
char uart_getc();


#endif // _UART_H_
