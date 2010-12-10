/**
 * @file
 * @brief VGA framebuffer driver
 *
 * @date 10.11.10
 * @author Nikolay Korotky
 */
#include <drivers/vga.h>
#include <asm/regs.h>

/** Save the X position. */
static int xpos;

/** Save the Y position. */
static int ypos;

/** Point to the video memory. */
static volatile unsigned char *video;

void vga_clear_screen(void) {
	int i;

	video = (unsigned char *) VIDEO;

	for (i = 0; i < COLUMNS * LINES * 2; i++) {
		*(video + i) = 0;
	}

	xpos = 0;
	ypos = 0;
}

void vga_putc(int c) {
	if (c == '\n' || c == '\r') {
newline:
		xpos = 0;
		ypos++;
		if (ypos >= LINES) {
			ypos = 0;
		}
		return;
	}

	*(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

	xpos++;
	if (xpos >= COLUMNS) {
		goto newline;
	}
}

int vga_getc(void) {
        return 0;
}

void diag_putc(int c) {
	vga_putc(c);
}

int diag_getc(void) {
	return vga_getc();
}

/*TODO: temporary */
void uart_putc(int c) {
	vga_putc(c);
}

int uart_getc(void) {
	return vga_getc();
}

