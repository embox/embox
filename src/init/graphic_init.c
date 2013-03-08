/**
 * @file
 *
 * @date Feb 4, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <assert.h>

#include <drivers/iodev.h>
#include <drivers/video/vesa.h>
#include <drivers/video/fb.h>
#include <embox/unit.h>

#define VESA_MODE_NUMBER OPTION_GET(NUMBER,vesa_mode)
#define FB_NAME OPTION_STRING_GET(fb_name)

EMBOX_UNIT_INIT(graphic_init);

static int graphic_init(void) {
	int ret;
	struct fb_info *info;
	const struct vesa_mode_desc *desc;
	const struct fb_videomode *mode;

	info = fb_lookup(FB_NAME);
	if (info == NULL) {
		return -ENODEV;
	}

	assert(info->ops != NULL);
	assert(info->ops->fb_set_par != NULL);

	desc = vesa_mode_get_desc(VESA_MODE_NUMBER);
	if (desc == NULL) {
		return -EINVAL;
	}

	mode = fb_desc_to_videomode(desc->xres, desc->yres, desc->bpp);
	if (mode == NULL) {
		return -EINVAL;
	}

	ret = fb_try_mode(&info->var, info, mode, desc->bpp);
	if (ret != 0) {
		return ret;
	}

	ret = info->ops->fb_set_par(info);
	if (ret != 0) {
		return ret;
	}

	ret = iodev_setup_video();
	if (ret != 0) {
		return ret;
	}

	ret = iodev_init();
	if (ret != 0) {
		return ret;
	}

	return 0;
}
