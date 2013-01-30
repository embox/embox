/**
 * @file
 *
 * @date 24.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/pci/pci.h>
#include <drivers/video/display.h>
#include <drivers/video/vbe.h>

PCI_DRIVER("bochs", bochs_init, PCI_VENDOR_ID_BOCHS, PCI_DEV_ID_BOCHS_VGA);

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

	display_init(&bochs_display, "Bochs display", &bochs_display_options,
			(void *)(pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK));

	return display_register(&bochs_display);
}
