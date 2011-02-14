/**
 * @file
 * @brief Implements diag and serial driver for skyeye
 *
 * @date 24.11.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <kernel/diag.h>
#include <kernel/irq.h>
#include <hal/reg.h>
#include <drivers/at91sam7_us.h>

int uart_init(void) {
	diag_init();
	return 0;
}

void uart_putc(char ch) {
	diag_putc(ch);
}

char uart_getc(void) {
	return diag_getc();
}

int uart_set_irq_handler(irq_handler_t pfunc) {
	return 0;
}

int uart_remove_irq_handler(void) {
	return 0;
}

/*implement diag interface*/
void diag_init(void) {
}

char diag_getc(void) {
	while (!diag_has_symbol()) {
	}
	return (char) REG_LOAD(0xfffd0018);
}

int diag_has_symbol(void) {
	return (AT91C_US_RXRDY & 0xfffd0014);
}

void diag_putc(char ch) {
	REG_STORE(0xfffd001c, (unsigned long) ch);
}
