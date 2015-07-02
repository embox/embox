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

static int generic_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {
	return -ENOSYS;
}

static int generic_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {

	memset(var, 0, sizeof(struct fb_var_screeninfo));

	if (!(OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_mode_set))) {
		return -ENOSYS;
	}

	var->xres           = OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width);
	var->yres           = OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height);
	var->bits_per_pixel = OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth);

	var->xres_virtual = var->xres;
	var->yres_virtual = var->yres;

	return 0;
}

static const struct fb_ops generic_ops = {
	.fb_get_var = generic_get_var,
	.fb_set_var = generic_set_var,
};

static int generic_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *) (pci_dev->bar[BAR] & ~0xf); /* FIXME */
	size_t mmap_len = binalign_bound(OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
			* OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)
			* OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth) / 8, PAGE_SIZE());
	struct fb_info *info;

	if (MAP_FAILED == mmap_device_memory(mmap_base,
				mmap_len,
			       	PROT_READ|PROT_WRITE|PROT_NOCACHE,
				MAP_FIXED,
				(unsigned long) mmap_base)) {
		return -EIO;
	}

	info = fb_create(&generic_ops, mmap_base, mmap_len);
	if (info == NULL) {
		munmap(mmap_base, mmap_len);
		return -ENOMEM;
	}

	return 0;
}
