/**
 * @file
 * @brief
 *
 * @date Jun 21, 2017
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>

int main() {
	long int screensize = 0;
	uint8_t *fbp = 0;
	int x = 0, y = 0;
	long int location = 0;
	struct fb_info *fb_info;

	fb_info = fb_lookup(0);

	printf("%dx%d, %dbpp\n", fb_info->var.xres, fb_info->var.yres, fb_info->var.bits_per_pixel);

	/* Figure out the size of the screen in bytes */
	screensize = fb_info->var.xres * fb_info->var.yres * fb_info->var.bits_per_pixel / 8;

#if 1
	/* Map the device to memory */
	fbp = (uint8_t *) mmap_device_memory((void *)fb_info->screen_base, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
			(uint64_t)((uintptr_t)fb_info->screen_base));
	if ((int) fbp == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");
#endif
	x = 0;
	y = 0; /* Where we are going to put the pixel */

	/*struct fb_fillrect rect = {
			0,
			0,
			3,
			100,
			0x0FF0,
			ROP_COPY,
	};
	fb_fillrect(fb_info, &rect);
	*/
#if 1
	/* Figure out where in memory to put the pixel */
	for (y = 0; y < 300; y++)
		for (x = 0; x < 300; x++) {

			location = (x + fb_info->var.xoffset) * (fb_info->var.bits_per_pixel / 8)
					+ (y + fb_info->var.yoffset) * fb_info->var.xres * (fb_info->var.bits_per_pixel / 8);

			if (fb_info->var.bits_per_pixel == 32) {
				*(fbp + location) = 100; /* Some blue */
				*(fbp + location + 1) = 15 + (x - 100) / 2; /* A little green*/
				*(fbp + location + 2) = 200 - (y - 100) / 5; /* A lot of red */
				*(fbp + location + 3) = 0; /* No transparency */
			} else { /* assume 16bpp */
				int b = 10;
				int g = (x - 100) / 6; /* A little green */
				int r = 31 - (y - 100) / 16; /* A lot of red */
				unsigned short int t = r << 11 | g << 5 | b;
				*((unsigned short int*) (fbp + location)) = t;
			}
		}
#endif
	munmap(fbp, screensize);
	return 0;
}
