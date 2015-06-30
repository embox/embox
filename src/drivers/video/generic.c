/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.03.2013
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <drivers/pci/pci.h>
#include <drivers/video/fb.h>
#include <drivers/video/vesa_modes.h>
#include <drivers/pci/pci_driver.h>
#include <framework/mod/options.h>
#include <mem/page.h>
#include <util/binalign.h>

#include <module/embox/arch/x86/boot/multiboot.h>

#define MBOOTMOD embox__arch__x86__boot__multiboot

#define VID OPTION_GET(NUMBER,vendor_id)
#define PID OPTION_GET(NUMBER,product_id)
#define BAR OPTION_GET(NUMBER,fb_bar)

PCI_DRIVER("generic fb", generic_init, VID, PID);

static int bochs_check_var(struct fb_var_screeninfo *var, struct fb_info *info) {
	return 0;
}

static int bochs_set_par(struct fb_info *info) {
	return -EINVAL;
}

static const struct fb_ops bochs_ops = {
	.fb_check_var = bochs_check_var,
	.fb_set_par = bochs_set_par,
	.fb_copyarea = fb_copyarea,
	.fb_fillrect = fb_fillrect,
	.fb_imageblit = fb_imageblit,
	.fb_cursor = fb_cursor
};

static void fill_var(struct fb_var_screeninfo *var) {

	memset(var, 0, sizeof(struct fb_var_screeninfo));

	if (!(OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_mode_set))) {
		return;
	}

	var->xres           = OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width);
	var->yres           = OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height);
	var->bits_per_pixel = OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth);

	var->xres_virtual = var->xres;
	var->yres_virtual = var->yres;

}

static int generic_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *) (pci_dev->bar[BAR] & ~0xf); /* FIXME */
	size_t mmap_len = binalign_bound(OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
			* OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)
			* OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth) / 8, PAGE_SIZE());
	struct fb_info *info;

	assert(pci_dev != NULL);

	info = fb_create(&bochs_ops, mmap_base, mmap_len);
	if (info == NULL) {
		return -ENOMEM;
	}

	fill_var(&info->var);

	if (MAP_FAILED == mmap_device_memory(mmap_base,
				mmap_len,
			       	PROT_READ|PROT_WRITE|PROT_NOCACHE,
				MAP_FIXED,
				(unsigned long) mmap_base)) {
		fb_delete(info);
		return -EIO;
	}

	return 0;
}
