/**
 * @file
 * @brief lib for read and draw BMP
 *
 * @date Apr 18, 2019
 * @author Filipp Chubukov
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <lib/bmp.h>

#define HEADER_SIZE 54 /* size of header */
#define COLOR_SIZE 4 /* num of colors for one 32 pixel */
#define PALLETE_SIZE 256 /* default and max num of colors in pallete */

/**
 * in each bmp format, in some cases, when the width is not a multiple of four,
 * the garbage is appended, this macro helps to calculate the garbage size
 */
#define FIND_SKIP(a) ( ( 4 - ( (a) % 4 ) ) & 0x03)

struct color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

int check_bmp(int fd) {
	uint8_t signature[2];

	lseek(fd, 0, SEEK_SET);
	read(fd, signature, 2);
	if (memcmp(signature, "BM", sizeof(signature)) != 0) {
		printf("Invalid bmp signature\n");
		return -1;
	}
	return 0;
}

static void read_pallete(int fd, struct color pallete[], int colors) {
	uint8_t tmp[4];
	int i;

	lseek(fd, HEADER_SIZE - 1, SEEK_SET); /* switch pos in fd to the end of head */
	for (i = 0; i < colors; i++) {
		read(fd, tmp, 4);
		pallete[i].red = tmp[1];
		pallete[i].green = tmp[2];
		pallete[i].blue = tmp[3];
	}
}

int predict_img_size(int fd) {
	struct bmp_header bmp_head;

	if (check_bmp(fd) != 0) {
		return -1;
	}

	if (read(fd, &bmp_head, sizeof(struct bmp_header)) != sizeof(struct bmp_header)) {
		printf("Error while read file descriptor.\n");
		return -1;
	}

	return bmp_head.biWidth * bmp_head.biHeight * 3;
}

int get_image_bmp(int fd, struct bmp_header *bmp_head, uint8_t *image) {
	int j, i ,k, skip, pos = 0;
	int height, width;
	struct color pallete[PALLETE_SIZE];
	uint8_t buff[4];

	if (check_bmp(fd) != 0) {
		return -1;
	}

	if (read(fd, bmp_head, sizeof(struct bmp_header)) != sizeof(struct bmp_header)) {
		printf("Error while read file descriptor.\n");
		return -1;
	}

	height = bmp_head->biHeight;
	width = bmp_head->biWidth;

	switch (bmp_head->biBitCount) {
		case 32:
			lseek(fd, bmp_head->bfOffBits, SEEK_SET);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					read(fd, buff, 4);
					image[pos] = buff[2];
					image[pos + 1] = buff[1];
					image[pos + 2] = buff[0];
					pos += 3;
				}
			}
			break;

		case 24:
			lseek(fd, bmp_head->bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width * 3);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					read(fd, buff, 3);
					image[pos] = buff[2];
					image[pos + 1] = buff[1];
					image[pos + 2] = buff[0];
					pos += 3;
				}
				if (skip) {
					read(fd, buff, skip);
				}
			}
			break;

		/* 16bpp very unusual format and is not supported in most parsers */
		case 16:
			printf("Error : 16bit bmp not supported\n");
			return -1;

		case 8:
			read_pallete(fd, pallete, (bmp_head->bfOffBits - HEADER_SIZE) / COLOR_SIZE);
			lseek(fd, bmp_head->bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					read(fd, buff, 1);
					image[pos] = pallete[buff[0]].blue;
					image[pos + 1] = pallete[buff[0]].green;
					image[pos + 2] = pallete[buff[0]].red;
					pos += 3;
				}
				if (skip) {
					read(fd, buff, skip);
				}
			}
			break;

		case 4:
			read_pallete(fd, pallete, (bmp_head->bfOffBits - HEADER_SIZE) / COLOR_SIZE);
			lseek(fd, bmp_head->bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width / 2 + width % 2);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width / 2; j++) {
					read(fd, buff, 1);
					buff[1] = buff[0]>>4;
					buff[2] = buff[0] & 0x0f;
					image[pos + 5] = pallete[buff[1]].red;
					image[pos + 4] = pallete[buff[1]].green;
					image[pos + 3] = pallete[buff[1]].blue;
					image[pos + 2] = pallete[buff[2]].red;
					image[pos + 1] = pallete[buff[2]].green;
					image[pos] = pallete[buff[2]].blue;
					pos += 6;
				}
				if (width % 2) {
					read(fd, buff, 1);
					buff[1] = buff[0]>>4;
					image[pos + 2] = pallete[buff[1]].red;
					image[pos + 1] = pallete[buff[1]].green;
					image[pos] = pallete[buff[1]].blue;
					pos += 3;
				}
				if (skip) {
					read(fd, buff, skip);
				}
			}
			break;

		case 1:
			lseek(fd, bmp_head->bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width / 8 + (width % 8 ? 1 : 0));
			for (i = 0; i < height; i++) {
				for (j = 0; j < width / 8; j++) {
					read(fd, buff, 1);
					for (k = 0; k < 8; k++) {
						if (buff[0] & 0x80) {
							image[pos] = 255;
							image[pos + 1] = 255;
							image[pos + 2] = 255;
							image[pos + 3] = 255;
							pos += 3;
						} else {
							image[pos] = 0;
							image[pos + 1] = 0;
							image[pos + 2] = 0;
							pos += 3;
						}
						buff[0] = buff[0]<<1;
					}
				}

				if (width % 8) {
					read(fd, buff, 1);
					for (k = 0; k < width % 8; k++) {
						if (buff[0] & 0x80) {
							image[pos] = 255;
							image[pos + 1] = 255;
							image[pos + 2] = 255;
							pos += 3;
						} else {
							image[pos] = 0;
							image[pos + 1] = 0;
							image[pos + 2] = 0;
							pos += 3;
						}
						buff[0] = buff[0]<<1;
					}
				}

				if (skip) {
					read(fd, buff, skip);
				}
			}
			break;

		default:
			printf("invalid bitcount\n");
			return -1;
	}
	return 0;
}
