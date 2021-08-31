/**
 * @file
 *
 * @date Feb 4, 2013
 * @author: Anton Bondarev
 */

#include <stddef.h>

#include <drivers/video/fb.h>
#include <drivers/video/fb_videomodes.h>

const struct fb_videomode *fb_find_videomode(int xres, int yres, int bpp) {
	int i;
	const struct fb_videomode *vmode;
	int size;

	vmode = fb_get_videomode_table(NULL);
	size = fb_get_videomode_table_size(NULL);

	for (i = 0; i < size; i ++) {
		if (xres == vmode[i].xres && yres == vmode[i].yres) {
			return &vmode[i];
		}
	}
	return NULL;
}

void fb_videomode_to_var(struct fb_var_screeninfo *var,
		const struct fb_videomode *mode) {
	var->xres = mode->xres;
	var->yres = mode->yres;
	var->xres_virtual = mode->xres;
	var->yres_virtual = mode->yres;
	var->xoffset = 0;
	var->yoffset = 0;
	var->pixclock = mode->pixclock;
	var->left_margin = mode->left_margin;
	var->right_margin = mode->right_margin;
	var->upper_margin = mode->upper_margin;
	var->lower_margin = mode->lower_margin;
	var->hsync_len = mode->hsync_len;
	var->vsync_len = mode->vsync_len;
	var->sync = mode->sync;
	var->vmode = mode->vmode & FB_VMODE_MASK;
}

void fb_var_to_videomode(struct fb_videomode *mode,
		const struct fb_var_screeninfo *var) { }
