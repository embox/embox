/**
 * @file
 * @brief diag interface for framebuffer device & keyboard
 *
 * @date 09.09.11
 * @author Anton Kozlov
 */

#include <drivers/diag.h>
#include <drivers/keyboard.h>
#include <drivers/vga_console.h>

char diag_getc(void) {
	return keyboard_getc();
}

void diag_putc(char ch) {
	vga_putc(vga_console_diag(), ch);
}

int diag_kbhit(void) {
	return keyboard_has_symbol();
}

void diag_init(void) {
	vga_console_init(vga_console_diag(), 80, 25);
	keyboard_init();
}
