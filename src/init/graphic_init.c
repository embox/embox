/**
 * @file
 *
 * @date Feb 4, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <assert.h>

#include <drivers/video/fb.h>
#include <drivers/video/fb_videomodes.h>

#include <embox/unit.h>

#include <arm/arm-m/kernel/devicetree.h>

EMBOX_UNIT_INIT(praphic_mode_init);

extern int platform_get_default_video_mode(int *x, int *y, int *bpp);

int graphic_init(void) {
	static int inited;
	struct fb_info *fbinfo;
	const struct fb_videomode *mode;
	struct fb_var_screeninfo var;
	int ret;
	int xres, yres, bpp;

	if (inited) {
		return 0;
	}
	inited = 1;

	if (0 == platform_get_default_video_mode(&xres, &yres, &bpp)) {
		return 0;
	}

	fbinfo = fb_lookup(0);
	if (fbinfo == NULL) {
		return -ENODEV;
	}

	mode = fb_find_videomode(xres, yres, bpp);
	if (mode == NULL) {
		return -EINVAL;
	}

	fb_videomode_to_var(&var, mode);
	var.bits_per_pixel = bpp;
	var.fmt = bpp == 16 ? BGR565 : RGBA8888;
	ret = fb_set_var(fbinfo, &var);
	if (ret != 0) {
		return -EIO;
	}

	return 0;

}

static int praphic_mode_init(void) {
	return graphic_init();
}
