/**
 * @file
 * @brief Simple test to draw frame via embox/fb interface
 *
 * @date Jun 21, 2017
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <util/math.h>

#include <drivers/video/fb.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main() {
	uint8_t *fbp = 0;
	int x, y, width, height;
	int bytes_per_pixel;
	long int idx = 0;
	struct fb_info *fb_info;

	fb_info = fb_lookup(0);

	printf("%" PRId32 "x%" PRId32 ", %" PRId32 "bpp\n",fb_info->var.xres, fb_info->var.yres, fb_info->var.bits_per_pixel);

	/* Figure out the size of the screen in bytes */
	bytes_per_pixel = fb_info->var.bits_per_pixel / 8;

	/* Map the device to memory */
	fbp = (uint8_t *) fb_info->screen_base;

	width = fb_info->var.xres;
	height = fb_info->var.yres;
	idx = fb_info->var.xoffset + fb_info->var.xres * fb_info->var.yoffset;

	for (y = 0; y < height; y++) {
		idx += fb_info->var.xres;

		for (x = 0; x < width; x++) {
			if (bytes_per_pixel == 4) {
				int b = MIN(0xFF, (1 + x + y) / ((width + height) / 0xFF));
				int g = MIN(0xFF, (x + 1) / (width / 0xFF));
				int r = MIN(0xFF, (1 + height - y) / (height / 0xFF));
				uint32_t t = ((r & 0xFF) << 16) |
						((g & 0xFF) << 8) | (b & 0xFF);

				((uint32_t *)fbp)[idx + x] = t | 0xff000000;
			} else { /* assume RGB565 */
				int b = MIN(0x1F, (1 + x + y) / ((width + height) / 0x1F));
				int g = MIN(0x3F, (x + 1) / (width / 0x3F));
				int r = MIN(0x1F, (1 + height - y) / (height / 0x1F));
				uint16_t t = ((r & 0x1F) << 11) |
						((g & 0x3F) << 5) | (b & 0x1F);
				((uint16_t *)fbp)[idx + x] = t;
			}
		}
	}

	return 0;
}
