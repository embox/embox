/*
 * @file
 *
 * @date Dec 19, 2012
 * @author: Anton Bondarev
 */

//http://linuxconsole.sourceforge.net/fbdev/HOWTO/3.html

//http://www.faqs.org/faqs/pc-hardware-faq/supervga-programming/
//http://wiki.osdev.org/VGA_Hardware#The_CRT_Controller
//http://www.osdever.net/FreeVGA/vga/crtcreg.htm

//CIRRUS_LOGIC_GD_5446

#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

#include <drivers/pci/pci.h>
#include <drivers/pci/pci_driver.h>
#include <drivers/pci/pci_id.h>

#include <drivers/video/cirrus_logic.h>
#include <drivers/video/vga.h>
#include <drivers/video/fb.h>

struct cirrus_chip_info {
	unsigned int *regbase;
	const struct fb_var_screeninfo *screen_info;
	int doubleVCLK;
	int multiplexing;
	char *screen_base;
};

static struct cirrus_chip_info cirrus_chip_info;

static void mdelay(int delay) {

}

void cirrus_chip_reset(struct cirrus_chip_info *cinfo) {
	/* disable flickerfixer */
	vga_wcrt(cinfo->regbase, CL_CRT51, 0x00);
	mdelay(100);
	/* mode */
	vga_wgfx(cinfo->regbase, CL_GR31, 0x00);
}

void udelay(int cnt) {
	volatile int tcnt = cnt * 1000;

	while(tcnt--) {}

}

/* write to hidden DAC registers */
static void WHDR(const struct cirrus_chip_info *cinfo, unsigned char val) {
	unsigned char dummy;

	/* now do the usual stuff to access the HDR */
	dummy = in8(VGA_PEL_MSK);
	udelay(200);
	dummy = in8(VGA_PEL_MSK);
	udelay(200);
	dummy = in8(VGA_PEL_MSK);
	udelay(200);
	dummy = in8(VGA_PEL_MSK);
	udelay(200);

	out8(val, VGA_PEL_MSK);
	udelay(200);

	dummy = dummy;
}

unsigned char RHDR(const struct cirrus_chip_info *cinfo) {
	unsigned char dummy;

	/* now do the usual stuff to access the HDR */
	dummy = in8(VGA_PEL_MSK);
	udelay(200);
	dummy = in8(VGA_PEL_MSK);
	udelay(200);
	dummy = in8(VGA_PEL_MSK);
	udelay(200);
	dummy = in8(VGA_PEL_MSK);
	udelay(200);

	dummy = in8(VGA_PEL_MSK);
	udelay(200);

	return dummy;

}

static void chipset_init(struct cirrus_chip_info *cinfo) {
	/* plane mask: nothing */
	vga_wseq(cinfo->regbase, VGA_SEQ_PLANE_MASK, 0xff);
	/* character map select: doesn't even matter in gx mode */
	vga_wseq(cinfo->regbase, VGA_SEQ_CHARACTER_MAP, 0x00);
	/* memory mode: chain4, ext. memory */
	vga_wseq(cinfo->regbase, VGA_SEQ_MEMORY_MODE, 0x0a);

	/* controller-internal base address of video memory */
/* enable extention mode ? */
//	if (bi->init_sr07)
//		vga_wseq(cinfo->regbase, CL_SEQR7, bi->sr07);

	/*  vga_wseq(cinfo->regbase, CL_SEQR8, 0x00); */
	/* EEPROM control: shouldn't be necessary to write to this at all.. */

	/* graphics cursor X position (incomplete; position gives rem. 3 bits */
	vga_wseq(cinfo->regbase, CL_SEQR10, 0x00);
	/* graphics cursor Y position (..."... ) */
	vga_wseq(cinfo->regbase, CL_SEQR11, 0x00);
	/* graphics cursor attributes */
	vga_wseq(cinfo->regbase, CL_SEQR12, 0x00);
	/* graphics cursor pattern address */
	vga_wseq(cinfo->regbase, CL_SEQR13, 0x00);

	/* Screen A preset row scan: none */
	vga_wcrt(cinfo->regbase, VGA_CRTC_PRESET_ROW, 0x00);
	/* Text cursor start: disable text cursor */
	vga_wcrt(cinfo->regbase, VGA_CRTC_CURSOR_START, 0x20);
	/* Text cursor end: - */
	vga_wcrt(cinfo->regbase, VGA_CRTC_CURSOR_END, 0x00);
	/* text cursor location high: 0 */
	vga_wcrt(cinfo->regbase, VGA_CRTC_CURSOR_HI, 0x00);
	/* text cursor location low: 0 */
	vga_wcrt(cinfo->regbase, VGA_CRTC_CURSOR_LO, 0x00);

	/* Underline Row scanline: - */
	vga_wcrt(cinfo->regbase, VGA_CRTC_UNDERLINE, 0x00);


	/* Set/Reset registes: - */
	vga_wgfx(cinfo->regbase, VGA_GFX_SR_VALUE, 0x00);
	/* Set/Reset enable: - */
	vga_wgfx(cinfo->regbase, VGA_GFX_SR_ENABLE, 0x00);
	/* Color Compare: - */
	vga_wgfx(cinfo->regbase, VGA_GFX_COMPARE_VALUE, 0x00);
	/* Data Rotate: - */
	vga_wgfx(cinfo->regbase, VGA_GFX_DATA_ROTATE, 0x00);
	/* Read Map Select: - */
	vga_wgfx(cinfo->regbase, VGA_GFX_PLANE_READ, 0x00);
	/* Mode: conf. for 16/4/2 color mode, no odd/even, read/write mode 0 */
	vga_wgfx(cinfo->regbase, VGA_GFX_MODE, 0x00);
	/* Miscellaneous: memory map base address, graphics mode */
	vga_wgfx(cinfo->regbase, VGA_GFX_MISC, 0x01);
	/* Color Don't care: involve all planes */
	vga_wgfx(cinfo->regbase, VGA_GFX_COMPARE_MASK, 0x0f);
	/* Bit Mask: no mask at all */
	vga_wgfx(cinfo->regbase, VGA_GFX_BIT_MASK, 0xff);


	/* Graphics controller mode extensions: finer granularity,
	 * 8byte data latches
	 */
	vga_wgfx(cinfo->regbase, CL_GRB, 0x28);

	vga_wgfx(cinfo->regbase, CL_GRC, 0xff);	/* Color Key compare: - */
	vga_wgfx(cinfo->regbase, CL_GRD, 0x00);	/* Color Key compare mask: - */
	vga_wgfx(cinfo->regbase, CL_GRE, 0x00);	/* Miscellaneous control: - */
	/* Background color byte 1: - */
	/*  vga_wgfx (cinfo->regbase, CL_GR10, 0x00); */
	/*  vga_wgfx (cinfo->regbase, CL_GR11, 0x00); */

	/* Attribute Controller palette registers: "identity mapping" */
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE0, 0x00);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE1, 0x01);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE2, 0x02);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE3, 0x03);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE4, 0x04);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE5, 0x05);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE6, 0x06);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE7, 0x07);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE8, 0x08);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTE9, 0x09);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTEA, 0x0a);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTEB, 0x0b);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTEC, 0x0c);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTED, 0x0d);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTEE, 0x0e);
	vga_wattr(cinfo->regbase, VGA_ATC_PALETTEF, 0x0f);

	/* Attribute Controller mode: graphics mode */
	vga_wattr(cinfo->regbase, VGA_ATC_MODE, 0x01);
	/* Overscan color reg.: reg. 0 */
	vga_wattr(cinfo->regbase, VGA_ATC_OVERSCAN, 0x00);
	/* Color Plane enable: Enable all 4 planes */
	vga_wattr(cinfo->regbase, VGA_ATC_PLANE_ENABLE, 0x0f);
	/* Color Select: - */
	vga_wattr(cinfo->regbase, VGA_ATC_COLOR_PAGE, 0x00);

//	WGen(cinfo, VGA_PEL_MSK, 0xff);	/* Pixel mask: no mask */

	/* BLT Start/status: Blitter reset */
	vga_wgfx(cinfo->regbase, CL_GR31, 0x04);
	/* - " -	   : "end-of-reset" */
	vga_wgfx(cinfo->regbase, CL_GR31, 0x00);

	/* misc... */
	WHDR(cinfo, 0);	/* Hidden DAC register: - */

}

static void setup_resolution(struct cirrus_chip_info *cinfo) {
	int vdispend, vsyncstart, vsyncend, vtotal;
	int hdispend, hsyncstart, hsyncend, htotal;
	int tmp;
	unsigned int *regbase = cinfo->regbase;
	const struct fb_var_screeninfo *var = cinfo->screen_info;

	hsyncstart = var->xres + var->right_margin;
	hsyncend = hsyncstart + var->hsync_len;
	htotal = (hsyncend + var->left_margin) / 8;
	hdispend = var->xres / 8;
	hsyncstart = hsyncstart / 8;
	hsyncend = hsyncend / 8;

	vdispend = var->yres;
	vsyncstart = vdispend + var->lower_margin;
	vsyncend = vsyncstart + var->vsync_len;
	vtotal = vsyncend + var->upper_margin;


	vdispend -= 1;
	vsyncstart -= 1;
	vsyncend -= 1;
	vtotal -= 2;

	if (cinfo->multiplexing) {
		htotal /= 2;
		hsyncstart /= 2;
		hsyncend /= 2;
		hdispend /= 2;
	}

	htotal -= 5;
	hdispend -= 1;
	hsyncstart += 1;
	hsyncend += 1;


	/* unlock register VGA_CRTC_H_TOTAL..CRT7 */
	vga_wcrt(regbase, VGA_CRTC_V_SYNC_END, 0x20);	/* previously: 0x00) */
	/* mode control: VGA_CRTC_START_HI enable, ROTATE(?), 16bit
	* address wrap, no compat. */
	vga_wcrt(regbase, VGA_CRTC_H_TOTAL, htotal);

	vga_wcrt(regbase, VGA_CRTC_H_DISP_END, hdispend);

	vga_wcrt(regbase, VGA_CRTC_H_BLANK_START, var->xres / 8);

	/* setup bit 7 to Compatible Read reg 0x10 and 0x11 */
	vga_wcrt(regbase, VGA_CRTC_H_BLANK_END,((htotal + 5) % 32) | 0x80);

	vga_wcrt(regbase, VGA_CRTC_H_SYNC_START, hsyncstart);

	tmp = hsyncend % 32;
	if ((htotal + 5) & 32) {
		tmp |= 0x80; /* setup bit 7 */
	}
	vga_wcrt(regbase, VGA_CRTC_H_SYNC_END, tmp);

	vga_wcrt(regbase, VGA_CRTC_V_TOTAL, vtotal & 0xff);

	tmp = 16;		/* LineCompare bit #9 */
	if (vtotal & 256)
		tmp |= 1;
	if (vdispend & 256)
		tmp |= 2;
	if (vsyncstart & 256)
		tmp |= 4;
	if ((vdispend + 1) & 256)
		tmp |= 8;
	if (vtotal & 512)
		tmp |= 32;
	if (vdispend & 512)
		tmp |= 64;
	if (vsyncstart & 512)
		tmp |= 128;
	vga_wcrt(regbase, VGA_CRTC_OVERFLOW, tmp);


	tmp = 0x40;		/* LineCompare bit #8 */
	if ((vdispend + 1) & 512)
		tmp |= 0x20;
	if (var->vmode & FB_VMODE_DOUBLE)
		tmp |= 0x80;

	vga_wcrt(regbase, VGA_CRTC_MAX_SCAN, tmp);

	vga_wcrt(regbase, VGA_CRTC_V_SYNC_START, vsyncstart & 0xff);

	/* bit 6 is Refresh Cycle Control
	 * bit 5 is Disable Vertical Interrupt
	 */
	vga_wcrt(regbase, VGA_CRTC_V_SYNC_END, (vsyncend % 16) | 0x40 | 0x20);

	vga_wcrt(regbase, VGA_CRTC_V_DISP_END, vdispend & 0xff);

	vga_wcrt(regbase, VGA_CRTC_V_BLANK_START, (vdispend + 1) & 0xff);

	vga_wcrt(regbase, VGA_CRTC_V_BLANK_END, vtotal & 0xff);

	vga_wcrt(regbase, VGA_CRTC_LINE_COMPARE, 0xff);

	tmp = var->xres >> 2;
	vga_wcrt(regbase, VGA_CRTC_OFFSET, tmp & 0xff);
	tmp >>= 8;

	/* ### add 0x40 for text modes with > 30 MHz pixclock */
	/* ext. display controls: ext.adr. wrap */
	/* add extended bit of OFFSET */
	vga_wcrt(cinfo->regbase, CL_CRT1B, 0x02 | (tmp ? 0x10 : 0) );

	vga_wcrt(regbase, VGA_CRTC_MODE_CONTROL, 0xc3);

	memset(cinfo->screen_base, 0, var->xres * var->yres);

}

static int cirrus_setup_bpp16(struct cirrus_chip_info *cinfo) {
	unsigned int *regbase = cinfo->regbase;

	/* Extended Sequencer Mode: 256c col. mode */
	vga_wseq(regbase, CL_SEQR7, cinfo->doubleVCLK ? 0xa3 : 0xa7);
	/* mode register: 256 color mode */
	vga_wgfx(regbase, VGA_GFX_MODE, 64);

	WHDR(cinfo, cinfo->doubleVCLK ? 0xe1 : 0xc1);

	return 0;
}

static int cirrus_setup_bpp24(struct cirrus_chip_info *cinfo) {
	unsigned int *regbase = cinfo->regbase;

	/* Extended Sequencer Mode: 256c col. mode */
	vga_wseq(regbase, CL_SEQR7, 0xa5);

	/* mode register: 256 color mode */
	vga_wgfx(regbase, VGA_GFX_MODE, 64);
	/* hidden dac reg: 8-8-8 mode (24 or 32) */
	WHDR(cinfo, 0xc5);

	return 0;
}

static int cirrus_setup_bits_per_pixel(struct cirrus_chip_info *cinfo) {
	switch(cinfo->screen_info->bits_per_pixel) {
	case 24:
		return cirrus_setup_bpp24(cinfo);
	case 16:
		return cirrus_setup_bpp16(cinfo);
	}
	return 0;
}

static int cl_set_var(struct fb_info *info, const struct fb_var_screeninfo *var) {
	cirrus_chip_info.doubleVCLK = 0;
	cirrus_chip_info.multiplexing = 0;
	cirrus_chip_info.screen_info = var;
	cirrus_chip_info.screen_base = info->screen_base;

	chipset_init(&cirrus_chip_info);

	setup_resolution(&cirrus_chip_info);

	cirrus_setup_bits_per_pixel(&cirrus_chip_info);

	vga_display_enable(1);

	return 0;
}

static int cl_get_var(struct fb_info *info, struct fb_var_screeninfo *var) {

	var->xres = 1280;
	var->yres = 1024;
	var->bits_per_pixel = 16;
	var->fmt = RGB565;
	return 0;
}

static const struct fb_ops cl_ops = {
	.fb_get_var = cl_get_var,
	.fb_set_var = cl_set_var,
};

static int cirrus_init(struct pci_slot_dev *pci_dev) {
	char *mmap_base = (char *)(pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK); /* FIXME */
	size_t mmap_len = 1024 * 1280 * 16 / 8;
	struct fb_info *info;

	if (MAP_FAILED == mmap_device_memory(mmap_base, mmap_len,
	        PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED,
	        (uintptr_t)mmap_base)) {
		return -EIO;
	}

	info = fb_create(&cl_ops, mmap_base, mmap_len);
	if (info == NULL) {
		munmap(mmap_base, mmap_len);
		return -ENOMEM;
	}

	return 0;
}

PCI_DRIVER("cl_gd5446", cirrus_init, PCI_VENDOR_ID_CIRRUS, PCI_DEV_ID_CIRRUS_5446);
