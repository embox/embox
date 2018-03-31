/**
 * @file
 *
 * @date Mar 21, 2018
 * @author Anton Bondarev
 */
#include <util/log.h>

#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>

#include <drivers/video/fb.h>

#include <util/binalign.h>
#include <mem/page.h>

#include <framework/mod/options.h>

#include "lynxfb.h"
#include "lynxfb_hw750.h"


PCI_DRIVER("lynxfb", lynxfb_init, PCI_VENDOR_ID_SMI, PCI_DEV_ID_LYNX_SE);


static struct sm750_share lynxfb_share;


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

static void lynxfb_hw_init(struct pci_slot_dev *pci_dev) {
#if 0
	int g_specId;
	size_t spec_offset;

	switch (pci_dev->device) {
	case PCI_DEV_ID_LYNX_EXP:
	case PCI_DEV_ID_LYNX_SE:
		g_specId = LYNXFB_SPC_SM750;
		/* though offset of share in sm750_share is 0,
		 *we use this marcro as the same */
		spec_offset = offsetof(struct sm750_share, share);
		break;
	default:
		break;
	}
#endif

	lynxfb_share.share.devid = pci_dev->device;
	lynxfb_share.share.revid = pci_dev->rev;

	lynxfb_hw750_setup(&lynxfb_share);

}

static int lynxfb_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *)(pci_dev->bar[0] & ~0xf); /* FIXME */
	size_t mmap_len = binalign_bound(2048 * 2048 * 32 / 8, PAGE_SIZE());
	struct fb_info *info;

	lynxfb_hw_init(pci_dev);

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
