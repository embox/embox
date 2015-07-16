/**
 * @file
 *
 * @date Feb 4, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <assert.h>

#include <drivers/video/vesa_modes.h>
#include <drivers/video/fb.h>
#include <drivers/video/fb_videomodes.h>
#include <embox/unit.h>

#include <module/embox/arch/x86/boot/multiboot.h>

#define VESA_MODE_NUMBER OPTION_GET(NUMBER,vesa_mode)
#define FB_INIT OPTION_GET(NUMBER,fb_init)

#define MBOOTMOD embox__arch__x86__boot__multiboot

#define SET_X   OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
#define SET_Y   OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)
#define SET_BPP OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth)

EMBOX_UNIT_INIT(mode_init);

static int mode_init(void) {
	struct fb_info *fbinfo;
	const struct fb_videomode *mode;
	const struct video_resbpp resbpp = {
		.x = SET_X,
		.y = SET_Y,
		.bpp = SET_BPP,
	};
	struct fb_var_screeninfo var;
	int ret;

	fbinfo = fb_lookup(0);
	if (fbinfo == NULL) {
		return -ENODEV;
	}

	mode = video_fbmode_by_resbpp(&resbpp);
	if (mode == NULL) {
		return -EINVAL;
	}

	fb_videomode_to_var(&var, mode);
	var.bits_per_pixel = SET_BPP;

	ret = fb_set_var(fbinfo, &var);
	if (ret != 0) {
		return -EIO;
	}

	return 0;

}
