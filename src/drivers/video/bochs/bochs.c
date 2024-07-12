/**
 * @file
 *
 * @date 24.01.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>

#include <drivers/video/fb.h>
#include <drivers/video/vbe.h>

#include <util/log.h>
#include <util/binalign.h>
#include <framework/mod/options.h>
#include <mem/page.h>

PCI_DRIVER("bochs", bochs_init, PCI_VENDOR_ID_BOCHS, PCI_DEV_ID_BOCHS_VGA);

static int bochs_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {
	if (var->xres > VBE_DISPI_MAX_XRES
			|| var->yres > VBE_DISPI_MAX_YRES
			|| var->bits_per_pixel > VBE_DISPI_MAX_BPP) {
		return -EINVAL;
	}

	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	vbe_write(VBE_DISPI_INDEX_XRES, var->xres);
	vbe_write(VBE_DISPI_INDEX_YRES, var->yres);
	vbe_write(VBE_DISPI_INDEX_BPP, var->bits_per_pixel);
	vbe_write(VBE_DISPI_INDEX_VIRT_WIDTH, var->xres_virtual);
	vbe_write(VBE_DISPI_INDEX_VIRT_HEIGHT, var->yres_virtual);
	vbe_write(VBE_DISPI_INDEX_X_OFFSET, var->xoffset);
	vbe_write(VBE_DISPI_INDEX_Y_OFFSET, var->yoffset);
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

	return 0;
}

static int bochs_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {

	memset(var, 0, sizeof(struct fb_var_screeninfo));

	var->xres           = vbe_read(VBE_DISPI_INDEX_XRES);
	var->yres           = vbe_read(VBE_DISPI_INDEX_YRES);
	var->bits_per_pixel = vbe_read(VBE_DISPI_INDEX_BPP);
	var->xres_virtual   = vbe_read(VBE_DISPI_INDEX_VIRT_WIDTH);
	var->yres_virtual   = vbe_read(VBE_DISPI_INDEX_VIRT_HEIGHT);
	var->xoffset        = vbe_read(VBE_DISPI_INDEX_X_OFFSET);
	var->yoffset        = vbe_read(VBE_DISPI_INDEX_Y_OFFSET);

	switch (var->bits_per_pixel) {
	case 16:
		var->fmt = BGR565;
		break;
	case 32:
		var->fmt = RGBA8888;
		break;
	default:
		log_error("Unsupported bpp = %d\n", var->bits_per_pixel);
		return -1;
	}

	return 0;
}

static const struct fb_ops bochs_ops = {
	.fb_set_var = bochs_set_var,
	.fb_get_var = bochs_get_var,
};

static int bochs_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *)(pci_dev->bar[0] & ~0xf); /* FIXME */
	size_t mmap_len = binalign_bound(VBE_DISPI_MAX_XRES
			* VBE_DISPI_MAX_YRES
			* VBE_DISPI_MAX_BPP / 8, PAGE_SIZE());
	struct fb_info *info;

	if (MAP_FAILED == mmap_device_memory(mmap_base, mmap_len,
	        PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED,
	        (uintptr_t)mmap_base)) {
		return -EIO;
	}

	info = fb_create(&bochs_ops, mmap_base, mmap_len);
	if (info == NULL) {
		munmap(mmap_base, mmap_len);
		return -ENOMEM;
	}

	return 0;
}
