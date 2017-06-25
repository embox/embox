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

#include <drivers/mailbox/bcm2835_mailbox.h>
#include <drivers/video/fb.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(raspi_init);

static int raspi_set_var(struct fb_info *info,
		struct fb_var_screeninfo const *var) {
	struct bcm2835_fb_info raspi_fb;

	/* Validate Inputs */
	if (var->xres > BCM2835_FB_MAX_RES
			|| var->yres > BCM2835_FB_MAX_RES
			|| var->bits_per_pixel > BCM2835_FB_MAX_BPP) {
		return -EINVAL;
	}

	if (info == NULL) {
		return -EFAULT;
	}
	
	/* Unmap old frame buffer if any */
	if (info->screen_base != NULL) {
		munmap(info->screen_base, info->screen_size);
	}

	/* Set new frame buffer information */
	raspi_fb.width_p        = var->xres;
	raspi_fb.height_p       = var->yres;
	raspi_fb.bit_depth      = var->bits_per_pixel;
	raspi_fb.width_v        = var->xres_virtual;
	raspi_fb.height_v       = var->yres_virtual;
	raspi_fb.x              = var->xoffset;
	raspi_fb.y              = var->yoffset;
	raspi_fb.gpu_pitch      = 0;    // Filled in by the VC
	raspi_fb.gpu_pointer    = 0;    // Filled in by the VC

	/**
	 * Send the address of the frame buffer + 0x40000000 to the mailbox
	 *
	 * By adding 0x40000000, we tell the GPU not to use its cache for these
	 * writes, which ensures we will be able to see the change
	 */
	bcm2835_mailbox_write(((uint32_t) &raspi_fb) + 0x40000000, BCM2835_FRAMEBUFFER_CHANNEL);
	if (bcm2835_mailbox_read(BCM2835_FRAMEBUFFER_CHANNEL) != 0 ||
			!raspi_fb.gpu_pointer) {
		return -EAGAIN;
	}

	/* Get new base address to the frame buffer */
	info->screen_base = (char *) raspi_fb.gpu_pointer;
	info->screen_size = binalign_bound(raspi_fb.gpu_size, PAGE_SIZE());
	if (MAP_FAILED == mmap_device_memory(info->screen_base,
				info->screen_size,
					PROT_READ|PROT_WRITE|PROT_NOCACHE,
				MAP_FIXED,
				(unsigned long) info->screen_base)) {
		return -EIO;
	}

	return 0;
}

static int raspi_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {
	return -ENOTSUP;
}

static struct fb_ops raspi_ops = {
	.fb_set_var = raspi_set_var,
	.fb_get_var = raspi_get_var,
};

static int raspi_init(void) {
	struct fb_info *info;

	info = fb_create(&raspi_ops, NULL, 0);
	if (info == NULL) {
		return -ENOMEM;
	}

	return 0;
}
