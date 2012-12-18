/*
 * @file
 *
 * @date Dec 14, 2012
 * @author: Anton Bondarev
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

//#include <types.h>

struct display;

typedef void (*display_set_pixel_ft)(struct display *display, unsigned x, unsigned y, unsigned c);
typedef void (*display_setup_ft)(struct display *display);

struct display {
	int mode;
	int width;
	int height;
	unsigned char *vga_regs;
	display_set_pixel_ft set_pixel;
};


extern void display_clear_screen(struct display *display);
extern void display_set_pixel(struct display *display, unsigned x, unsigned y, unsigned c);


#endif /* DISPLAY_H_ */
