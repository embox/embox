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
#include <drivers/video/vesa.h>
#include <errno.h>
#include <framework/mod/options.h>

#define MODOPS_DEFAULT_MODE OPTION_GET(NUMBER, default_mode)

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

static int bochs_init(struct pci_slot_dev *pci_dev) {
	int ret;
	struct fb_info *info;
	const struct vesa_mode_desc *desc;
	const struct fb_videomode *mode;

	assert(pci_dev != NULL);

	info = fb_alloc();
	if (info == NULL) {
		return -ENOMEM;
	}

	memcpy(&info->fix, &bochs_fix_screeninfo, sizeof info->fix);

	info->ops = &bochs_ops;
	info->screen_base = (void *)(pci_dev->bar[0] & ~0xf); /* FIXME */

	desc = vesa_mode_get_desc(MODOPS_DEFAULT_MODE);
	if (desc == NULL) {
		fb_release(info);
		return -EINVAL;
	}

	mode = fb_desc_to_videomode(desc->xres, desc->yres, desc->bpp);
	if (mode == NULL) {
		fb_release(info);
		return -EINVAL;
	}

	ret = fb_try_mode(&info->var, info, mode, desc->bpp);
	if (ret != 0) {
		fb_release(info);
		return ret;
	}

	ret = fb_register(info);
	if (ret != 0) {
		fb_release(info);
		return ret;
	}

	return 0;
}
