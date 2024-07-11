/**
 * @file
 * @brief VideoCore Device Driver for Raspberry Pi
 *
 * @date 16.07.15
 * @author Michele Di Giorgio
 */

#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>

#include <drivers/common/memory.h>

#include <drivers/mailbox/bcm2835_mailbox.h>
#include <drivers/video/fb.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(raspi_init);

/*	set_x = 1024, set_y = 768, set_bpp = 16 */
#define CONFIG_FB_XRES OPTION_GET(NUMBER,fb_xres)
#define CONFIG_FB_YRES OPTION_GET(NUMBER,fb_yres)
#define CONFIG_FB_BPP OPTION_GET(NUMBER,fb_bpp)
#define CONFIG_FB_VC_BUS OPTION_GET(NUMBER,fb_vc_bus)

static int raspi_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	return 0;
}

static int raspi_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {
	var->xres = CONFIG_FB_XRES;
	var->yres = CONFIG_FB_YRES;
	var->bits_per_pixel = CONFIG_FB_BPP;
	return 0;
}

static struct fb_ops raspi_ops = {
	.fb_set_var = raspi_set_var,
	.fb_get_var = raspi_get_var,
};

static int raspi_init(void) {
	struct bcm2835_fb_info raspi_fb;
	struct fb_info *info;

	/* Set new frame buffer information */
	raspi_fb.width_p        = CONFIG_FB_XRES;
	raspi_fb.height_p       = CONFIG_FB_YRES;
	raspi_fb.bit_depth      = CONFIG_FB_BPP;
	raspi_fb.width_v        = CONFIG_FB_XRES;
	raspi_fb.height_v       = CONFIG_FB_YRES;
	raspi_fb.x              = 0;
	raspi_fb.y              = 0;
	raspi_fb.gpu_pitch      = 0;    // Filled in by the VC
	raspi_fb.gpu_pointer    = 0;    // Filled in by the VC

	bcm2835_mailbox_write(((uint32_t) &raspi_fb) + CONFIG_FB_VC_BUS, BCM2835_FRAMEBUFFER_CHANNEL);
	if (bcm2835_mailbox_read(BCM2835_FRAMEBUFFER_CHANNEL) != 0 ||
			!raspi_fb.gpu_pointer) {
		return -EAGAIN;
	}

	info = fb_create(&raspi_ops, (char *) raspi_fb.gpu_pointer, binalign_bound(raspi_fb.gpu_size, PAGE_SIZE()));
	if (info == NULL) {
		return -ENOMEM;
	}

	/* Get new base address to the frame buffer */
	info->screen_base = (char *) raspi_fb.gpu_pointer;
	info->screen_size = binalign_bound(raspi_fb.gpu_size, PAGE_SIZE());
	if (MAP_FAILED == mmap_device_memory(info->screen_base, info->screen_size,
	        PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED,
	        (uintptr_t)info->screen_base)) {
		fb_delete(info);
		return -EIO;
	}

	return 0;
}
