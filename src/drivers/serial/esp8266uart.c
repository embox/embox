/**
 * @file
 * @brief ESP8266's universal serial communication interface, uart mode
 *
 * @author  Ilya Nozhkin
 * @date    22.03.2017
 */

#include <hal/reg.h>
#include <drivers/diag.h>
#include <drivers/diag.h>

static int esp8266uart_diag_init(const struct diag *diag) {
	return 0;
}

static void esp8266uart_diag_putc(const struct diag *diag, char ch) {
}

static char esp8266uart_diag_getc(const struct diag *diag) {
	return 0;
}

static int esp8266uart_diag_kbhit(const struct diag *diag) {
	return 0;
}

DIAG_OPS_DECLARE(
		.init = esp8266uart_diag_init,
		.putc = esp8266uart_diag_putc,
		.getc = esp8266uart_diag_getc,
		.kbhit = esp8266uart_diag_kbhit,
);
