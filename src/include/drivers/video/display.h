/*
 * @file
 *
 * @date 14.12.12
 * @author Anton Bondarev
 * @author Ilia Vaporl
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <types.h>

struct display;

#if 0
typedef void (*display_set_pixel_ft)(uint16_t pos_x, uint16_t pos_y, uint16_t color);
typedef void (*display_setup_ft)(struct display *display);
#endif

struct display {
	uint16_t width;
	uint16_t height;
	uint8_t mode;
	uint8_t *vga_regs;
	void (*setup)(struct display *displ, uint16_t width, uint16_t height, uint8_t depth);
	uint32_t (*get_pixel)(struct display *displ, uint16_t x, uint16_t y);
	void (*set_pixel)(struct display *displ, uint16_t x, uint16_t y, uint32_t color);
};



struct screen_info {
	int xres;
	int right_margin;
	int hsync_len;
	int left_margin;

	int yres;
	int lower_margin;
	int vsync_len;
	int upper_margin;

	int vmode;
	int bits_per_pixel;
};


extern void display_clear_screen(struct display *display);

extern void display_set_pixel(struct display *display, unsigned x, unsigned y, unsigned c);


#endif /* DISPLAY_H_ */
