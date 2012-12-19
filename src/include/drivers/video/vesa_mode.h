/*
 * @file
 *
 * @date Dec 19, 2012
 * @author: Anton Bondarev
 */

#ifndef VESA_MODE_H_
#define VESA_MODE_H_

#include <util/array.h>
#include <drivers/video/vesa.h>

struct display;
struct vga_mode_description;

typedef void (*vga_set_pixel_ft)(struct display *display, int x, int y, int c);
typedef void (*vga_putc_ft)(struct display *display, int x, int y, int c);
typedef void (*vga_setup_ft)(struct vga_mode_description *vga_mode);

struct vga_mode_description {
	int mode;
	const unsigned char *regs;
	vga_set_pixel_ft set_pixel;
	vga_putc_ft putc;
	vga_setup_ft setup;
	int width;
	int height;
};

#define VGA_MODE_DEFINE(vga_mode,vga_regs,setp,put_char,setup_f,scr_width,scr_height)  \
	extern const struct vga_mode_description *__vga_mode_descriptions[];               \
	static const struct vga_mode_description vga_mode_##mode = {                       \
			.mode = vga_mode,                                                          \
			.regs = vga_regs,                                                          \
			.set_pixel = setp,                                                         \
			.putc = put_char,                                                     \
			.setup = setup_f,                                                     \
			.width = scr_width,                                                  \
			.height = scr_height                                                 \
	};                                                                           \
	ARRAY_SPREAD_ADD(__vga_mode_descriptions,&vga_mode_##mode);

const struct vga_mode_description *vga_mode_description_lookup(enum vesa_video_mode);



#endif /* VESA_MODE_H_ */
