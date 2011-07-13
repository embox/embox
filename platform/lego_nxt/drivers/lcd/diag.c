/**
 * @file
 * @brief Diag LCD output
 *
 * @date 7.06.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <drivers/nxt/lcd.h>
#include <kernel/diag.h>

void diag_init(void) {
	lcd_init();
	display_clear_screen();
}

char diag_getc(void) {
	/*getc cant be implemented */
	return '!';
}

void diag_putc(char ch) {
	display_char(ch);
}

int diag_has_symbol(void) {
	/* as no symbol can be obtained... */
#define DEBUG_GETC
	/*should be disabled in release for consistency */
#ifdef DEBUG_GETC
	return true;
#else
	return false;
#endif
}
