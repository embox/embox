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

static char diag_vga_getc(void) {
	return keyboard_getc();
}

static void diag_vga_putc(char ch) {
	vga_putc(vga_console_diag(), ch);
}

static int diag_vga_kbhit(void) {
	return keyboard_has_symbol();
}

static const struct diag_ops diag_vga_ops = {
	.getc = &diag_vga_getc,
	.putc = &diag_vga_putc,
	.kbhit = &diag_vga_kbhit
};

void diag_init(void) {
	diag_common_set_ops(&diag_vga_ops);
	vga_console_init(vga_console_diag(), 80, 25);
	keyboard_init();
}
