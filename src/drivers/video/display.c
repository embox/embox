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

#if 0
/*****************************************************************************
*****************************************************************************/
void display_draw(struct display *display) {
	unsigned x, y;

/* clear screen */
	for(y = 0; y < display->height; y++) {
		for(x = 0; x < display->width; x++) {
			display->set_pixel(x, y, 0);
		}
/* draw 2-color X */
	for(y = 0; y < g_ht; y++)
	{
		g_write_pixel((g_wd - g_ht) / 2 + y, y, 1);
		g_write_pixel((g_ht + g_wd) / 2 - y, y, 2);
	}
}
#endif

void display_set_pixel(struct display *display, unsigned x, unsigned y, unsigned c) {
	display->set_pixel(display, x, y, c);
}
