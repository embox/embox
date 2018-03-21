/**
 * @file
 *
 * @date Mar 21, 2018
 * @author Anton Bondarev
 */

#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>

#include <drivers/video/fb.h>

#include <util/binalign.h>
#include <mem/page.h>

#include <framework/mod/options.h>


PCI_DRIVER("lynxfb", lynxfb_init, PCI_VENDOR_ID_SMI, PCI_DEV_ID_LYNX_SE);

static int lynxfb_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {
	return 0;
}

static int lynxfb_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {

	memset(var, 0, sizeof(struct fb_var_screeninfo));

	return 0;
}

static const struct fb_ops lynxfb_ops = {
	.fb_set_var = lynxfb_set_var,
	.fb_get_var = lynxfb_get_var,
};

static int lynxfb_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *)(pci_dev->bar[0] & ~0xf); /* FIXME */
	size_t mmap_len = binalign_bound(2048 * 2048 * 32 / 8, PAGE_SIZE());
	struct fb_info *info;

	if (MAP_FAILED == mmap_device_memory(mmap_base,
				mmap_len,
			       	PROT_READ|PROT_WRITE|PROT_NOCACHE,
				MAP_FIXED,
				(unsigned long) mmap_base)) {
		return -EIO;
	}

	info = fb_create(&lynxfb_ops, mmap_base, mmap_len);
	if (info == NULL) {
		munmap(mmap_base, mmap_len);
		return -ENOMEM;
	}

	return 0;
}
