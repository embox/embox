/**
 * @file
 *
 * @date 24.01.13
 * @author Ilia Vaprol
 */

#include <asm/io.h>
#include <assert.h>
#include <drivers/pci/pci.h>
#include <drivers/video/display.h>
#include <types.h>
#include <string.h>
#include <util/list.h>

PCI_DRIVER("bochs", bochs_init, PCI_VENDOR_ID_BOCHS, PCI_DEV_ID_BOCHS_VGA);

#define VBE_DISPI_IOPORT_INDEX      0x01CE
#define VBE_DISPI_IOPORT_DATA       0x01CF

#define VBE_DISPI_INDEX_ID          0x00
#define VBE_DISPI_INDEX_XRES        0x01
#define VBE_DISPI_INDEX_YRES        0x02
#define VBE_DISPI_INDEX_BPP         0x03
#define VBE_DISPI_INDEX_ENABLE      0x04
#define VBE_DISPI_INDEX_BANK        0x05
#define VBE_DISPI_INDEX_VIRT_WIDTH  0x06
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x07
#define VBE_DISPI_INDEX_X_OFFSET    0x08
#define VBE_DISPI_INDEX_Y_OFFSET    0x09

#define VBE_DISPI_DISABLED          0x00
#define VBE_DISPI_ENABLED           0x01
#define VBE_DISPI_GETCAPS           0x02
#define VBE_DISPI_8BIT_DAC          0x20
#define VBE_DISPI_LFB_ENABLED       0x40
#define VBE_DISPI_NOCLEARMEM        0x80

static void vbe_write(uint16_t index, uint16_t value) {
	out16(index, VBE_DISPI_IOPORT_INDEX);
	out16(value, VBE_DISPI_IOPORT_DATA);
}

static void bochs_set_resolution(uint16_t xres, uint16_t yres, uint16_t bpp) {
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	vbe_write(VBE_DISPI_INDEX_XRES, xres);
	vbe_write(VBE_DISPI_INDEX_YRES, yres);
	vbe_write(VBE_DISPI_INDEX_BPP, bpp);
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

static void bochs_setup(struct display *displ, unsigned int width,
		unsigned int height, unsigned int mode) {
	assert(displ != NULL);

	displ->width = width;
	displ->height = height;
	displ->mode = mode;

	bochs_set_resolution(displ->width, displ->height,
			displ->mode & DISPLAY_MODE_DEPTH16 ? 16
				: displ->mode & DISPLAY_MODE_DEPTH32 ? 32 : 8); /* depth is 8 by default */
}

static void bochs_set_pixel(struct display *displ, unsigned int x, unsigned int y,
		unsigned int color) {
	size_t pixel_offset;

	assert(displ != NULL);
	assert(displ->vga_regs != NULL);

	pixel_offset = x + y * displ->width;
	if (displ->mode & DISPLAY_MODE_DEPTH16) {
		*((uint16_t *)displ->vga_regs + pixel_offset) = color;
	}
	else if (displ->mode & DISPLAY_MODE_DEPTH32) {
		*((uint32_t *)displ->vga_regs + pixel_offset) = color;
	}
	else {
		*((uint8_t *)displ->vga_regs + pixel_offset) = color;
	}
}

static unsigned int bochs_get_pixel(struct display *displ, unsigned int x,
		unsigned int y) {
	size_t pixel_offset;

	assert(displ != NULL);
	assert(displ->vga_regs != NULL);

	pixel_offset = x + y * displ->width;
	return displ->mode & DISPLAY_MODE_DEPTH16
			? *((uint16_t *)displ->vga_regs + pixel_offset)
			: displ->mode & DISPLAY_MODE_DEPTH32
				? *((uint32_t *)displ->vga_regs + pixel_offset)
				: *((uint8_t *)displ->vga_regs + pixel_offset);
}

static const struct display_options bochs_display_options = {
	.setup = &bochs_setup,
	.get_pixel = &bochs_get_pixel,
	.set_pixel = &bochs_set_pixel
};

static int bochs_init(struct pci_slot_dev *pci_dev) {
	static struct display bochs_display;

	assert(pci_dev != NULL);

	memset(&bochs_display, 0, sizeof(struct display));

	bochs_display.name = "Bochs display";
	list_link_init(&bochs_display.lnk);
	bochs_display.ops = &bochs_display_options;
	bochs_display.vga_regs = (unsigned char *)(pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK);

	return display_register(&bochs_display);
}
