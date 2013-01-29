/*
 * @file
 *
 * @date 14.12.12
 * @author Anton Bondarev
 * @author Ilia Vaporl
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <util/list.h>

#define DISPLAY_MODE_DEPTH8  0x00
#define DISPLAY_MODE_DEPTH16 0x01
#define DISPLAY_MODE_DEPTH32 0x02

struct display;

struct display_options {
	void (*setup)(struct display *displ, unsigned int width,
			unsigned int height, unsigned int mode);

	unsigned int (*get_pixel)(struct display *displ, unsigned int x,
			unsigned int y);

	void (*set_pixel)(struct display *displ, unsigned int x, unsigned int y,
			unsigned int color);
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

struct display {
	const char *name;
	struct list_link lnk;
	const struct display_options *ops;
#if 0
	struct screen_info info;
#endif

	void (*set_pixel)(struct display *displ, unsigned short x,
			unsigned short y, unsigned color); /* TODO remove this (it's for vga_setup_mode) */

	unsigned int width;
	unsigned int height;
	unsigned int mode;
	void *vga_regs;
};

extern int display_register(struct display *displ);
extern void display_unregister(struct display *displ);
extern struct display * display_get_default(void);

extern void display_clear_screen(struct display *displ);

extern void display_set_pixel(struct display *displ, unsigned int x,
		unsigned int y, unsigned int color);
extern unsigned int display_get_pixel(struct display *displ, unsigned int x,
		unsigned int y);

#endif /* DISPLAY_H_ */
