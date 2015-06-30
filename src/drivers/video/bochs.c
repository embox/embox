/**
 * @file
 *
 * @date 24.01.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
#include <sys/mman.h>
#include <util/binalign.h>
#include <mem/page.h>
#include <drivers/pci/pci.h>
#include <drivers/video/fb.h>
#include <drivers/video/vbe.h>
#include <drivers/video/vesa_modes.h>
#include <framework/mod/options.h>
#include <drivers/pci/pci_driver.h>

PCI_DRIVER("bochs", bochs_init, PCI_VENDOR_ID_BOCHS, PCI_DEV_ID_BOCHS_VGA);

static int bochs_check_var(struct fb_var_screeninfo *var, struct fb_info *info) {
	return 0;
}

static int bochs_set_par(struct fb_info *info) {
	info->screen_size = info->var.xres * info->var.yres
			* (info->var.bits_per_pixel / 8);
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	vbe_write(VBE_DISPI_INDEX_XRES, info->var.xres);
	vbe_write(VBE_DISPI_INDEX_YRES, info->var.yres);
	vbe_write(VBE_DISPI_INDEX_BPP, info->var.bits_per_pixel);
	vbe_write(VBE_DISPI_INDEX_VIRT_WIDTH, info->var.xres_virtual);
	vbe_write(VBE_DISPI_INDEX_VIRT_HEIGHT, info->var.yres_virtual);
	vbe_write(VBE_DISPI_INDEX_X_OFFSET, info->var.xoffset);
	vbe_write(VBE_DISPI_INDEX_Y_OFFSET, info->var.yoffset);
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

	return 0;
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

	var->xres           = vbe_read(VBE_DISPI_INDEX_XRES);
	var->yres           = vbe_read(VBE_DISPI_INDEX_YRES);
	var->bits_per_pixel = vbe_read(VBE_DISPI_INDEX_BPP);
	var->xres_virtual   = vbe_read(VBE_DISPI_INDEX_VIRT_WIDTH);
	var->yres_virtual   = vbe_read(VBE_DISPI_INDEX_VIRT_HEIGHT);
	var->xoffset        = vbe_read(VBE_DISPI_INDEX_X_OFFSET);
	var->yoffset        = vbe_read(VBE_DISPI_INDEX_Y_OFFSET);
}

static int bochs_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *)(pci_dev->bar[0] & ~0xf); /* FIXME */
	size_t mmap_len = binalign_bound(VBE_DISPI_MAX_XRES 
			* VBE_DISPI_MAX_YRES 
			* VBE_DISPI_MAX_BPP / 8, PAGE_SIZE());
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
