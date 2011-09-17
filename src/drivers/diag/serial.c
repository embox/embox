/**
 * @file
 * @brief diag interface for serial port
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */

#include <kernel/diag.h>
#include <drivers/serial.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(diag_serial_init);

void diag_init(void) {
	uart_init();
}

int diag_has_symbol(void) {
	return uart_has_symbol();
}

char diag_getc(void) {
	return uart_getc();

}

void diag_putc(char ch) {
	uart_putc(ch);
}

static int diag_serial_init(void) {
	diag_init();
	return 0;
}
