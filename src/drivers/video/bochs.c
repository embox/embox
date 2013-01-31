/**
 * @file
 *
 * @date 24.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/pci/pci.h>
#include <drivers/video/fb.h>
#include <drivers/video/vbe.h>
#include <errno.h>

PCI_DRIVER("bochs", bochs_init, PCI_VENDOR_ID_BOCHS, PCI_DEV_ID_BOCHS_VGA);

int bochs_check_var(struct fb_var_screeninfo *var, struct fb_info *info) {
	return 0;
}

int bochs_set_par(struct fb_info *info) {
	info->screen_size = info->var.xres * info->var.yres
			* (info->var.bits_per_pixel / 8);
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	vbe_write(VBE_DISPI_INDEX_XRES, info->var.xres);
	vbe_write(VBE_DISPI_INDEX_YRES, info->var.yres);
	vbe_write(VBE_DISPI_INDEX_BPP, info->var.bits_per_pixel);
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
	return 0;
}

static const struct fb_ops bochs_ops = {
	.fb_check_var = bochs_check_var,
	.fb_set_par = bochs_set_par
};

static const struct fb_fix_screeninfo bochs_fix_screeninfo = {
	.name = "Bochs framebuffer"
};

static const struct fb_var_screeninfo bochs_default_var_screeninfo = {
	.xres = 1024,
	.yres = 768,
	.bits_per_pixel = 16
};

static int bochs_init(struct pci_slot_dev *pci_dev) {
	int ret;
	struct fb_info *info;

	assert(pci_dev != NULL);

	info = fb_alloc();
	if (info == NULL) {
		return -ENOMEM;
	}

	memcpy(&info->fix, &bochs_fix_screeninfo, sizeof info->fix);
	memcpy(&info->var, &bochs_default_var_screeninfo, sizeof info->var);
	info->ops = &bochs_ops;
	info->screen_base = (void *)(pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK);

	ret = fb_register(info);
	if (ret != 0) {
		fb_release(info);
		return ret;
	}

	return 0;
}
