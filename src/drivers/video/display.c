/**
 * @file
 *
 * @date Dec 14, 2012
 * @author: Anton Bondarev
 */
#include <types.h>
#include <drivers/video/display.h>

void display_clear_screen(struct display *display) {
	unsigned x, y;

	if(display->set_pixel == NULL) {
		return;
	}
	/* clear screen */
	for(y = 0; y < display->height; y++) {
		for(x = 0; x < display->width; x++) {
			display->set_pixel(display, x, y, 0);
		}
	}
}

void display_set_pixel(struct display *display, unsigned x, unsigned y, unsigned c) {
	display->set_pixel(display, x, y, c);
}
