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

#include <lib/fb_draw.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main() {
	struct screen screen_info;
	int x, y, scr_pos = 0;
	uint8_t r, g, b;

	if (fb_draw_init_screen(&screen_info, 0) != 0) {
		return -1;
	}

	for (y = 0; y < screen_info.height; y++) {

		for (x = 0; x < screen_info.width; x++) {
			r = MIN(0xFF, (1 + x + y) / ((screen_info.width + screen_info.height) / 0xFF));
			g = MIN(0xFF, (x + 1) / (screen_info.width / 0xFF));
			b = MIN(0xFF, (1 + screen_info.height - y) / (screen_info.height / 0xFF));

			if (fb_draw_put_pix(r, g, b, 32, &screen_info, scr_pos + x) != 0) {
				return -1;
			}
		}
		scr_pos += screen_info.width;
	}

	return 0;
}
