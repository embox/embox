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
	struct lynxfb_par *par;
	struct lynxfb_crtc *crtc;
	struct lynxfb_output *output;

	int ret;
	unsigned int line_length;

	if (!info)
		return -EINVAL;

	ret = 0;
	crtc = &par->crtc;
	output = &par->output;

	/* fix structure is not so FIX ... */
	line_length = var->xres_virtual * var->bits_per_pixel / 8;
	line_length = ALIGN(line_length, crtc->line_pad);

	log_debug("fix->line_length = %d", line_length);

	/*
	 * var->red,green,blue,transp are need to be set by driver
	 * and these data should be set before setcolreg routine
	 */
	switch (var->bits_per_pixel) {
	case 16:
		//var->fmt = BGR565;
		break;
	case 24:
	case 32:
		//var->fmt = BGRA8888;
		break;
	case 8:
	default:
		ret = -EINVAL;
		break;
	}
	//var->yres = var->xres = -1;


	if (ret) {
		log_error("pixel bpp format not satisfied.");
		return ret;
	}
	ret = hw_sm750_crtc_setMode(crtc, var, line_length);
	if (!ret)
		ret = hw_sm750_output_setMode(output, var);
	return ret;

}

static int lynxfb_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {

	memset(var, 0, sizeof(struct fb_var_screeninfo));
	var->bits_per_pixel = 16;
	var->fmt = BGR565;
	var->xres = 1024;
	var->yres = 768;


	return 0;
}

static const struct fb_ops lynxfb_ops = {
	.fb_set_var = lynxfb_set_var,
	.fb_get_var = lynxfb_get_var,
};

static void lynxfb_hw_init(struct pci_slot_dev *pci_dev) {
	struct lynx_share *share = &lynxfb_share.share;

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

	share->devid = pci_dev->device;
	share->revid = pci_dev->rev;
	log_debug("devid = %x, revid = %x", share->devid, share->revid);

	lynxfb_hw750_setup(&lynxfb_share);

	lynxfb_hw750_map(&lynxfb_share, pci_dev);

	lynxfb_hw750_inithw(&lynxfb_share);

	memset(share->pvMem, 0x0, share->vidmem_size);
}

static int lynxfb_init(struct pci_slot_dev *pci_dev) {
	struct fb_info *info;
	struct lynx_share *share = &lynxfb_share.share;

	lynxfb_hw_init(pci_dev);

	info = fb_create(&lynxfb_ops, (char *)share->pvMem, share->vidmem_size);
	if (info == NULL) {
		munmap(share->pvMem, share->vidmem_size);
		munmap(share->pvReg, share->vidreg_size);
		return -ENOMEM;
	}

	return 0;
}
