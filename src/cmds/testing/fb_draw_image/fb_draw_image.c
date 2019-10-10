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
#include <lib/png.h>

#include <lib/fb_draw.h>

static void print_usage(void) {
	printf(
		"Usage : fb_draw_image [FILE]\n"
		"If [FILE] format is supporting then draw it to the framebuffer\n"
	 );
}

static int draw_image(struct screen *screen_info, uint8_t *img, uint32_t height, uint32_t width, int num_of_channels) {
	long int img_pos = 0, scr_pos = 0;
	int x, y;
	printf("%d\n", screen_info->width);
	for (y = 0; y < height; y++) {

		for (x = 0; x < width; x++) {
			if (fb_draw_put_pix(img[img_pos], img[img_pos + 1], img[img_pos + 2], 32, screen_info, scr_pos + x) != 0) {
				return -1;
			}
			img_pos += num_of_channels;
		}
		scr_pos += screen_info->width;
	}
	return 0;
}

int main(int argc, char *argv[]) {
	int fd = 0;
	struct screen screen_info;
	uint8_t file_header[FILE_HEADER_LEN];
	struct bmp bmp_data;
	struct png png_data;

	if (argc != 2) {
		print_usage();
		return -1;
	}

	if (fb_draw_init_screen(&screen_info, 0) != 0) {
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

	/* work with file */
	switch (raw_get_file_format(file_header)) {
		case BMP_FILE:
			if (bmp_load(argv[1], &bmp_data) != 0) {
				printf("Error : can't load bmp file.\n");
				return -1;
			}

			if (bmp_data.width > screen_info.width || bmp_data.height > screen_info.height) {
				printf("Error : image > screen.\n");
			}

			if (draw_image(&screen_info, bmp_data.image, bmp_data.height, bmp_data.width, BMP_NUM_OF_CHANNELS) != 0) {
				bmp_unload(&bmp_data);
				return -1;
			}

			bmp_unload(&bmp_data);
			break;

		case PNG_FILE:
			if (png_load(argv[1], &png_data) != 0) {
				printf("Error : can't load png file.\n");
				return -1;
			}

			if (png_data.width > screen_info.width || png_data.height > screen_info.height) {
				printf("Error : image > screen.\n");
			}

			if (png_data.color == PNG_TRCL) {
				if (draw_image(&screen_info, png_data.image, png_data.height, png_data.width, PNG_TRCL) != 0) {
					png_unload(&png_data);
					return -1;
				}
			} else {
				if (draw_image(&screen_info, png_data.image, png_data.height, png_data.width, PNG_TRCLA) != 0) {
					png_unload(&png_data);
					return -1;
				}
			}

			png_unload(&png_data);
			break;

		default:
			printf("This format is not support.\n");
			return -1;
	}

	return 0;
}
