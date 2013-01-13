/*
 * @file
 *
 * @date Dec 14, 2012
 * @author: Anton Bondarev
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_


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



struct screen_info {
	int xres;
	int yres;
	int right_margin;
	int hsync_len;
	int left_margin;
	int lower_margin;
	int vsync_len;
	int upper_margin;
	int vmode;
	int bits_per_pixel;
};


extern void display_clear_screen(struct display *display);
extern void display_set_pixel(struct display *display, unsigned x, unsigned y, unsigned c);


#endif /* DISPLAY_H_ */
