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
#define MBOOT_VIDEO_SET OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_mode_set)
#define MBOOT_VWIDTH OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_width)
#define MBOOT_VHEIGHT OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_height)
#define MBOOT_VDEPTH OPTION_MODULE_GET(MBOOTMOD,NUMBER,video_depth)

#define VID OPTION_GET(NUMBER,vendor_id)
#define PID OPTION_GET(NUMBER,product_id)
#define BAR OPTION_GET(NUMBER,fb_bar)

PCI_DRIVER("generic fb", generic_init, VID, PID);

static int generic_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {
	return -ENOSYS;
}

static int generic_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {

	if (!MBOOT_VIDEO_SET) {
		return -ENOSYS;
	}

	memset(var, 0, sizeof(struct fb_var_screeninfo));

	var->xres_virtual = var->xres = MBOOT_VWIDTH;
	var->yres_virtual = var->yres = MBOOT_VWIDTH;
	var->bits_per_pixel = MBOOT_VDEPTH;
	var->fmt = RGBA8888;

	return 0;
}

static const struct fb_ops generic_ops = {
	.fb_get_var = generic_get_var,
	.fb_set_var = generic_set_var,
};

static int generic_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *) (pci_dev->bar[BAR] & ~0xf); /* FIXME */
	size_t mmap_len = binalign_bound(MBOOT_VWIDTH *
			MBOOT_VHEIGHT * MBOOT_VDEPTH / 8, PAGE_SIZE());
	struct fb_info *info;

	if (MAP_FAILED == mmap_device_memory(mmap_base, mmap_len,
	        PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED,
	        (uintptr_t)mmap_base)) {
		return -EIO;
	}

	info = fb_create(&generic_ops, mmap_base, mmap_len);
	if (info == NULL) {
		munmap(mmap_base, mmap_len);
		return -ENOMEM;
	}

	return 0;
}
