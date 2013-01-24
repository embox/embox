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


static struct display display_bochs; /* TODO create in init and register this display */


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

static void bochs_setup(struct display *displ, uint16_t width,
		uint16_t height, uint8_t depth) {
	assert(displ != NULL);

	displ->width = width;
	displ->height = height;
	displ->mode = depth;

	bochs_set_resolution(displ->width, displ->height, displ->mode);
}

static void bochs_set_pixel(struct display *displ, uint16_t x, uint16_t y,
		uint32_t color) {
	size_t pixel_offset;

	assert(displ != NULL);
	assert(displ->vga_regs != NULL);
	assert(displ->mode == 16); /* TODO for now */

	pixel_offset = (x * displ->width + y) * (displ->mode / 8);
	*(uint16_t *)(displ->vga_regs + pixel_offset) = color;
}

static uint32_t bochs_get_pixel(struct display *displ, uint16_t x, uint16_t y) {
	size_t pixel_offset;

	assert(displ != NULL);
	assert(displ->vga_regs != NULL);

	pixel_offset = (x * displ->width + y) * (displ->mode / 8);
	return *(uint16_t *)(displ->vga_regs + pixel_offset);
}

struct display * get_bochs_display(void) {
	return &display_bochs;
}

static int bochs_init(struct pci_slot_dev *pci_dev) {
	assert(pci_dev != NULL);

	display_bochs.mode = 0;
	display_bochs.width = 0;
	display_bochs.height = 0;
	display_bochs.vga_regs = (uint8_t *)(pci_dev->bar[0] & PCI_BASE_ADDR_IO_MASK);
	display_bochs.get_pixel = bochs_get_pixel,
	display_bochs.set_pixel = bochs_set_pixel,
	display_bochs.setup = bochs_setup;
	return 0;
}
