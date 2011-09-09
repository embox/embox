/**
 * @file
 * @brief diag interface for framebuffer device & keyboard
 *
 * @date 09.09.11
 * @author Anton Kozlov
 */

#include <kernel/diag.h>
#include <drivers/keyboard.h>
#include <drivers/vga.h>

void diag_init(void) {
	vga_console_init(vga_console_diag(), 80, 25);
	keyboard_init();
}

void diag_putc(char c) {
	vga_putc(vga_console_diag(), c);
}

char diag_getc(void) {
//	blink_cursor(con.x, con.y);
	return keyboard_getc();
}

int diag_has_symbol(void) {
	return keyboard_has_symbol();
}
