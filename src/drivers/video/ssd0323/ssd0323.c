/**
 * @file pl110.c
 * @brief Initial implementation for PL110 video controller
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 09.08.2017
 */

 #include <util/log.h>

#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include <hal/reg.h>
#include <drivers/common/memory.h>
#include <drivers/video/fb.h>

#include <framework/mod/options.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(ssd0323_init);

#define SSD0323_DISPLAY_WIDTH  OPTION_GET(NUMBER,display_width)
#define SSD0323_DISPLAY_HEIGHT OPTION_GET(NUMBER,display_height)

static int ssd0323_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

/* we use only 16 grey scale color */
static uint8_t ssd0323_fb[SSD0323_DISPLAY_WIDTH * SSD0323_DISPLAY_HEIGHT / 2]
                        __attribute__ ((aligned (0x4)));

static int ssd0323_get_var(struct fb_info *info,
		struct fb_var_screeninfo *var) {

	return 0;
}

static const struct fb_ops ssd0323_ops = {
	.fb_set_var   = ssd0323_set_var,
	.fb_get_var   = ssd0323_get_var,
};

static int ssd0323_init(void) {

	char *mmap_base = (void *) ssd0323_fb;

	fb_create(&ssd0323_ops, mmap_base, sizeof(ssd0323_fb));

	memset(mmap_base, 0, sizeof(ssd0323_fb));

	return 0;
}

