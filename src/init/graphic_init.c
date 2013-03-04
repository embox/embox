/**
 * @file
 *
 * @date Feb 4, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <assert.h>

#include <drivers/iodev.h>
#include <drivers/video/vesa_modes.h>
#include <drivers/video/fb.h>
#include <embox/unit.h>

#define VESA_MODE_NUMBER OPTION_GET(NUMBER,vesa_mode)
#define FB_NAME OPTION_STRING_GET(fb_name)
#define FB_INIT OPTION_GET(NUMBER,fb_init)

EMBOX_UNIT_INIT(graphic_init);

static int fb_init(void) {
	struct fb_info *info;
	const struct video_resbpp *resbpp;
	const struct fb_videomode *mode;
	int ret;

	info = fb_lookup(FB_NAME);
	if (info == NULL) {
		return -ENODEV;
	}

	assert(info->ops != NULL);
	assert(info->ops->fb_set_par != NULL);

	resbpp = video_resbpp_by_vesamode(VESA_MODE_NUMBER);
	mode = video_fbmode_by_resbpp(resbpp);
	if (mode == NULL) {
		return -EINVAL;
	}

	ret = fb_try_mode(&info->var, info, mode, resbpp->bpp);
	if (ret != 0) {
		return ret;
	}

	ret = info->ops->fb_set_par(info);
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static int graphic_init(void) {
	int ret;

	ret = FB_INIT ? fb_init() : 0;

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
