/**
 * @file
 *
 * @date 24.01.13
 * @author Ilia Vaprol
 */

#include <asm/io.h>
#include <embox/unit.h>
#include <types.h>

EMBOX_UNIT_INIT(bochs_init);

#define VBE_DISPI_IOPORT_INDEX      0x01CE
#define VBE_DISPI_IOPORT_DATA       0x01CF

#define VBE_DISPI_INDEX_ID          0x0
#define VBE_DISPI_INDEX_XRES        0x1
#define VBE_DISPI_INDEX_YRES        0x2
#define VBE_DISPI_INDEX_BPP         0x3
#define VBE_DISPI_INDEX_ENABLE      0x4
#define VBE_DISPI_INDEX_BANK        0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH  0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7
#define VBE_DISPI_INDEX_X_OFFSET    0x8
#define VBE_DISPI_INDEX_Y_OFFSET    0x9

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

static void vbe_set(uint16_t xres, uint16_t yres, uint16_t bpp) {
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	vbe_write(VBE_DISPI_INDEX_XRES, xres);
	vbe_write(VBE_DISPI_INDEX_YRES, yres);
	vbe_write(VBE_DISPI_INDEX_BPP, bpp);
	vbe_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);
}

#include <string.h>
static int bochs_init(void) {
	uint16_t *disp = (void *)0xFD000000;
	vbe_set(1280,1024,16);
    // Painting top half of the screen with white
    memset(disp,0x8f,1280*1024);
	return 0;
}
