/**
 * @file
 *
 * @date Feb 4, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <assert.h>

#include <drivers/video/vesa.h>
#include <drivers/video/fb.h>

#include <embox/unit.h>

#define VESA_MODE_NUMBER OPTION_GET(NUMBER,vesa_mode)
#define FB_NAME OPTION_STRING_GET(fb_name)

EMBOX_UNIT_INIT(graphic_init);
extern const struct fb_videomode *fb_desc_to_videomode(int x, int y, int depth);
static int graphic_init(void) {
	struct fb_info *info;
	struct vesa_mode_desc *desc;
	const struct fb_videomode *mode;

	info = fb_lookup(FB_NAME);
	if (info == NULL) {
		return -ENODEV;
	}

	assert(info->ops != NULL);
	assert(info->ops->fb_set_par != NULL);

	desc = vesa_mode_get_desc(VESA_MODE_NUMBER);
	mode = fb_desc_to_videomode(desc->xres, desc->yres, desc->bpp);

	fb_videomode_to_var(&info->var, mode);

	return info->ops->fb_set_par(info);
}
