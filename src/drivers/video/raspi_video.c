/**
 * @file
 * @brief VideoCore Device Driver for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/video/raspi_fb.h>
#include <drivers/video/fb.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(raspi_init);

struct raspi_fb_info *raspi_fb;

/*static int raspi_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {
	if (var->xres > RASPI_FB_MAX_RES
			|| var->yres > RASPI_FB_MAX_RES
			|| var->bits_per_pixel > RASPI_FB_MAX_BPP)
		return -EINVAL;

	raspi_fb->width_p = var->xres;
	raspi_fb->height_p = var->yres;
	raspi_fb->bit_depth = var->bits_per_pixel;
	raspi_fb->width_v = var->xres_virtual;
	raspi_fb->height_v = var->yres_virtual;
	raspi_fb->x = var->xoffset;
	raspi_fb->y = var->yoffset;

	raspi_fb->gpu_pitch = 0;
	raspi_fb->gpu_pointer = 0;
	raspi_fb->gpu_size = 0;

	mailbox_write(((uint32_t) & raspi_fb)+0x40000000, BCM2835_FRAMEBUFFER_CHANNEL);
	if (mailbox_read(BCM2835_FRAMEBUFFER_CHANNEL) != 0 || !raspi_fb->gpu_pointer)
		return -EAGAIN;

	return 0;
}*/

static int raspi_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {
	memset(var, 0, sizeof(struct fb_var_screeninfo));

	var->xres			= raspi_fb->width_p;
	var->yres			= raspi_fb->height_p;
	var->bits_per_pixel	= raspi_fb->bit_depth;
	var->xres_virtual	= raspi_fb->width_v;
	var->yres_virtual	= raspi_fb->height_v;
	var->xoffset		= raspi_fb->x;
	var->yoffset		= raspi_fb->y;

	return 0;
}

static struct fb_ops raspi_ops = {
	//.fb_set_var = raspi_set_var,
	.fb_set_var = NULL,
	.fb_get_var = raspi_get_var,
};
	

static int raspi_init(void) {
	char *mmap_base;
	size_t mmap_len;
	struct fb_info *info;


	raspi_fb = init_raspi_fb(SCREEN_WIDTH, SCREEN_HEIGHT, BIT_DEPTH);

	mmap_base = (char *) raspi_fb->gpu_pointer;
	mmap_len = binalign_bound(raspi_fb->gpu_size, PAGE_SIZE());
	if (MAP_FAILED == mmap_device_memory(mmap_base,
				mmap_len,
					PROT_READ|PROT_WRITE|PROT_NOCACHE,
				MAP_FIXED,
				(unsigned long) mmap_base)) {
		return -EIO;
	}

	info = fb_create(&raspi_ops, mmap_base, mmap_len);
	if (info == NULL) {
		munmap(mmap_base, mmap_len);
		return -ENOMEM;
	}

	return 0;
}
