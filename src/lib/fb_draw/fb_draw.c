/**
 * @file
 * @brief lib for drawing in framebuffer
 * @author Filipp Chubukov
 * @version
 * @date 22.08.2019
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <lib/fb_draw.h>
#include <drivers/video/fb.h>

/*table for convert 5bit color to 8bit color */
static const uint8_t TABLE_5BIT[32] = {0, 8, 16, 25, 33, 41, 49, 58,
									66, 74, 82, 90, 99, 107, 115, 123,
									132, 140, 148, 156, 165, 173, 181, 189,
									197, 206, 214, 222, 230, 239, 247, 255};

/*table for convert 6bit color to 8bit color */
static const uint8_t TABLE_6BIT[64] = {0, 4, 8, 12, 16, 20, 24, 28,
									32, 36, 40, 45, 49, 53, 57, 61,
									65, 69, 73, 77, 81, 85, 89, 93,
									97, 101, 105, 109, 113, 117, 121, 125,
									130, 134, 138, 142, 146, 150, 154, 158,
									162, 166, 170, 174, 178, 182, 186, 190,
									194, 198, 202, 206, 210, 215, 219, 223,
									227, 231, 235, 239, 243, 247, 251, 255};

int fb_draw_init_screen(struct screen *screen_info, int screen_id) {
	struct fb_info *fb_info;

	fb_info = fb_lookup(screen_id);

	if (fb_info == NULL) {
		printf("Error : can't init screen.\n");
		return -1;
	}

	screen_info->fbp = (uint8_t *) fb_info->screen_base;
	screen_info->height = fb_info->var.yres;
	screen_info->width = fb_info->var.xres;
	screen_info->bpp = fb_info->var.bits_per_pixel;

	return 0;
}

static uint32_t get_32bpp_pix(uint8_t r, uint8_t g, uint8_t b, int bpp) {
	switch (bpp) {
		case 16:
			r = TABLE_5BIT[r];
			g = TABLE_6BIT[g];
			b = TABLE_5BIT[b];
			return (((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF)) | 0xff000000;

		case 32:
			return (((b & 0xFF) << 16) | ((g & 0xFF) << 8) | (r & 0xFF)) | 0xff000000;
	}
	printf("Unsupported color format\n");
	return -1;
}

static uint16_t get_16bpp_pix(uint8_t r, uint8_t g, uint8_t b, int bpp) {
	switch (bpp) {
		case 16:
			return (((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F));

		case 32:
			r = r / 8;
			g = g / 4;
			b = b / 8;
			return (((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F));
	}
	printf("Unsupported color format\n");
	return -1;
}

int fb_draw_put_pix(uint8_t r, uint8_t g, uint8_t b, int color_bpp, struct screen *screen_info, long int scr_pos) {
	switch (screen_info->bpp) {
		case 32:
			((uint32_t *)(screen_info->fbp))[scr_pos] = get_32bpp_pix(r, g, b, color_bpp);
			return 0;

		case 16:
			((uint16_t *)(screen_info->fbp))[scr_pos] = get_16bpp_pix(r, g, b, color_bpp);
			return 0;
	}
	printf("Unsupported screen format\n");
	return -1;
}
