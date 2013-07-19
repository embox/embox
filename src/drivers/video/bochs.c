/**
 * @file
 *
 * @date 24.01.13
 * @author Ilia Vaprol
 */

#include <errno.h>
#include <string.h>
#include <assert.h>
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

static const struct fb_fix_screeninfo bochs_fix_screeninfo = {
	.name = "Bochs framebuffer"
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
	int ret;
	struct fb_info *info;

	assert(pci_dev != NULL);

	info = fb_alloc();
	if (info == NULL) {
		return -ENOMEM;
	}

	memcpy(&info->fix, &bochs_fix_screeninfo, sizeof info->fix);
	fill_var(&info->var);

	info->ops = &bochs_ops;
	info->screen_base = (void *)(pci_dev->bar[0] & ~0xf); /* FIXME */

	ret = fb_register(info);
	if (ret != 0) {
		fb_release(info);
		return ret;
	}

	return 0;
}
