/**
 * @file
 * @brief
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */

#ifndef DRIVERS_SERIAL_H_
#define DRIVERS_SERIAL_H_

extern int uart_init(void);

extern char uart_getc(void);

extern void uart_putc(char c);

extern int uart_has_symbol(void);

#endif /* SERIAL_H_ */
