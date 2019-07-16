/**
 * @file
 * @brief app for draw images to the framebuffer
 *
 * @date July 14, 2019
 * @author Filipp Chubukov
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fs/file_format.h>
#include <lib/bmp.h>

#include <drivers/video/fb.h>

static void print_usage(void) {
	printf(
		"Usage : fb_draw_image [FILE]\n"
		"If [FILE] format is supporting then draw it to the framebuffer\n"
	 );
}

static int get_fbp(uint8_t *fbp, struct fb_info *fb_info, uint8_t *img, uint32_t width, uint32_t height) {
	int x, y, pos = 0;
	long int idx = fb_info->var.xoffset + fb_info->var.xres * fb_info->var.yoffset;
	int bytes_per_pixel = fb_info->var.bits_per_pixel / 8;

	for (y = 0; y < height; y++) {
		idx += fb_info->var.xres;

		for (x = 0; x < width; x++) {
			if (bytes_per_pixel == 4) {
				int b = img[pos];
				int g = img[pos + 1];
				int r = img[pos + 2];
				pos += 3;
				uint32_t t = ((r & 0xFF) << 16) |
						((g & 0xFF) << 8) | (b & 0xFF);

				((uint32_t *)fbp)[idx + x] = t | 0xff000000;
			} else {
				int b = img[pos + 2];
				int g = img[pos + 1];
				int r = img[pos];
				pos += 3;
				uint16_t t = ((r & 0x1F) << 11) |
						((g & 0x3F) << 5) | (b & 0x1F);
				((uint16_t *)fbp)[idx + x] = t;
			}
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int fd = 0;
	struct fb_info *fb_info;
	uint8_t *fbp = 0;
	uint8_t file_header[FILE_HEADER_LEN];
	struct bmp bmp_data;

	if (argc != 2) {
		print_usage();
		return -1;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		printf("Error : can't open file\n");
		return -1;
	}

	/* read header of file */
	if (read(fd, file_header, FILE_HEADER_LEN) != FILE_HEADER_LEN) {
		printf("Error while getting file header\n");
		close(fd);
		return -1;
	}
	close(fd);

	/* work with framebuffer */
	fb_info = fb_lookup(0);

	printf("%" PRId32 "x%" PRId32 ", %" PRId32 "bpp\n", fb_info->var.xres, fb_info->var.yres, fb_info->var.bits_per_pixel);

	/* Map the device to memory */
	fbp = (uint8_t *) fb_info->screen_base;

	/* work with file */
	switch (raw_get_file_format(file_header)) {
		case BMP_FILE:
			if (bmp_load(argv[1], &bmp_data) != 0) {
				printf("Error : can't load bmp file.\n");
				return -1;
			}

			get_fbp(fbp, fb_info, bmp_data.image, bmp_data.width, bmp_data.height);

			bmp_unload(&bmp_data);
			break;

		default:
			printf("This format is not support.\n");
			return -1;
	}

	return 0;
}
