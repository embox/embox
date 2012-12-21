/*
 * @file
 *
 * @date Dec 19, 2012
 * @author: Anton Bondarev
 */


//CIRRUS_LOGIC_GD_5446

//http://www.faqs.org/faqs/pc-hardware-faq/supervga-programming/

#include <drivers/pci/pci_driver.h>
#include <drivers/pci/pci_id.h>

#include <drivers/video/vga.h>
#include <drivers/video/vesa.h>

void setup_resolution(void) {
	/* unlock register VGA_CRTC_H_TOTAL..CRT7 */
	vga_wcrt(regbase, VGA_CRTC_V_SYNC_END, 0x20);	/* previously: 0x00) */
	/* mode control: VGA_CRTC_START_HI enable, ROTATE(?), 16bit
	* address wrap, no compat. */
	vga_wcrt(regbase, VGA_CRTC_MODE_CONTROL, 0xc3);

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
	dev_dbg(info->device, "CRT9: %d\n", tmp);
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

}


static int setup_bpp16(void) {
	/* Extended Sequencer Mode: 256c col. mode */
	vga_wseq(regbase, CL_SEQR7,	cinfo->doubleVCLK ? 0xa3 : 0xa7);
	/* mode register: 256 color mode */
	vga_wgfx(regbase, VGA_GFX_MODE, 64);

	WHDR(cinfo, cinfo->doubleVCLK ? 0xe1 : 0xc1);

	return 0;
}

/******************************************************
 * 24 bpp
 */
static int setup_bpp24(void) {
	/* Extended Sequencer Mode: 256c col. mode */
	vga_wseq(regbase, CL_SEQR7, 0xa5);


	/* mode register: 256 color mode */
	vga_wgfx(regbase, VGA_GFX_MODE, 64);
	/* hidden dac reg: 8-8-8 mode (24 or 32) */
	WHDR(cinfo, 0xc5);

	return 0;
}


static int setup_bits_per_pixel(int bpp) {
	return 0;
}

static int cirrus_init(struct pci_slot_dev *pci_dev) {
	return 0;
}

static void cirrus_vga_setup(struct vga_mode_description *vga_mode) {
	setup_bits_per_pixel(24);
}

VGA_MODE_DEFINE(0x114, NULL, NULL, NULL, cirrus_vga_setup, 800, 600);
VGA_MODE_DEFINE(0x115, NULL, NULL, NULL, cirrus_vga_setup, 800, 600);


PCI_DRIVER("cl_gd5446", cirrus_init, PCI_VENDOR_ID_CIRRUS, PCI_DEV_ID_CIRRUS_5446);
