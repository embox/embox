/**
 * @file
 * @brief
 *
 * @date 04.02.13
 * @author Ilia Vaprol
 */

#ifndef DRIVERS_VIDEO_FONT_H_
#define DRIVERS_VIDEO_FONT_H_

#include <stdint.h>

#define VGA8x8_IDX      0
#define VGA8x16_IDX     1

struct font_desc {
	uint8_t idx;
	const char *name;
	uint32_t width;
	uint32_t height;
	const char *data;
};

extern const struct font_desc font_vga_8x8, font_vga_8x16;

#endif /* DRIVERS_VIDEO_FONT_H_ */
