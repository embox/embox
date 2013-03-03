/**
 * @file
 * @brief Implements diag and serial driver for skyeye
 *
 * @date 24.11.10
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <kernel/irq.h>
#include <hal/reg.h>
#include <drivers/at91sam7_us.h>

void uart_init(void) {

}

char uart_getc(void) {
	while (!uart_has_symbol()) {
	}
	return (char) REG_LOAD(0xfffd0018);
}

int uart_has_symbol(void) {
	return (AT91C_US_RXRDY & 0xfffd0014);
}

void uart_putc(char ch) {
	REG_STORE(0xfffd001c, (unsigned long) ch);
}
