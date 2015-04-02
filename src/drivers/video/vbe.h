/**
 * @file
 * @brief
 *
 * @date 30.01.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIDEO_VBE_H_
#define DRIVERS_VIDEO_VBE_H_

#include <asm/io.h>
#include <stdint.h>

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

/* FIXME This could and probably should be read from GETCAPS register */
#define VBE_DISPI_MAX_XRES              1600
#define VBE_DISPI_MAX_YRES              1200
#define VBE_DISPI_MAX_BPP               32

static inline void vbe_write(uint16_t index, uint16_t value) {
	out16(index, VBE_DISPI_IOPORT_INDEX);
	out16(value, VBE_DISPI_IOPORT_DATA);
}

static inline unsigned short vbe_read(uint16_t index) {
	out16(index, VBE_DISPI_IOPORT_INDEX);
	return in16(VBE_DISPI_IOPORT_DATA);
}

#endif /* DRIVERS_VIDEO_VBE_H_ */
