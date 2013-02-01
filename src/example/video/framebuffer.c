/**
 * @file
 * @brief
 *
 * @date 01.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/video/fb.h>
#include <framework/example/self.h>

EMBOX_EXAMPLE(run);

static int run(int argc, char **argv) {
	int color1 = 0xF0,
		color2 = 0x0F,
		color3 = 0x70,
		color4 = 0x07;
	struct fb_info *info = fb_lookup("fb0");
	assert(info != NULL);
	assert(info->ops != NULL);
	assert(info->ops->fb_set_par != NULL);
	info->ops->fb_set_par(info);
	fb_memset(info->screen_base, color1, info->screen_size / 4);
	fb_memset((info->screen_base + info->screen_size / 4),
			color2, info->screen_size / 4);
	fb_memset((info->screen_base + info->screen_size / 2),
			color3, info->screen_size / 4);
	fb_memset((info->screen_base + 3 * info->screen_size / 4),
			color4, info->screen_size / 4);
	return 0;
}
