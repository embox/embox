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
#include <fs/file_format.h>

#include <lib/bmp.h>

#define HEADER_SIZE 54 /* size of header */
#define COLOR_SIZE 4 /* num of colors for one 32 pixel */
#define PALLETE_SIZE 256 /* default and max num of colors in pallete */

/**
 * in each bmp format, in some cases, when the width is not a multiple of four,
 * the garbage is appended, this macro helps to calculate the garbage size
 */
#define FIND_SKIP(a) ( ( 4 - ( (a) % 4 ) ) & 0x03)

void bmp_unload(struct bmp *bmp_data) {
	free(bmp_data->image);
}

struct bmp_header {
	uint32_t bfSize;	      /* Size of file */
	uint32_t bfReserved;      /* == "00" */
	uint32_t bfOffBits;	      /* Indicate to start of image bytes */
	uint32_t biSize;	      /* Size of header */
	uint32_t biWidth;		      /* Width of image in pixels */
	uint32_t biHeight;		      /*Height of image in pixels */
	uint16_t biPlanes;	      /* == 1 */
	uint16_t biBitCount;		      /* bits per pixel */
	uint32_t biCompression;	      /* type of compression(we are not using) */
	uint32_t biSizeImage;     /* size of image, == 0 if no compression */
	uint32_t biXPelsPerMeter; /* X resolution */
	uint32_t biYPelsPerMeter; /* Y resolution */
	uint32_t biClrUsed;	      /* Number of used from color table colors */
	uint32_t biClrImportant;  /* Number of important colors - min for drawing */
};

struct color {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

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

int bmp_load(char *file_name, struct bmp *bmp_data) {
	int j, i , k, tmp, skip, pos = 0;
	int height, width;
	struct color pallete[PALLETE_SIZE];
	struct bmp_header bmp_head;
	uint8_t signature[2], buff[4];

	int fd = open(file_name, O_RDONLY);
	if (fd == -1) {
		printf("Error : can't open file.\n");
		return -1;
	}

	if (read(fd, signature, sizeof(signature)) != sizeof(signature)) {
		printf("Error : can't read signature of file.\n");
		close(fd);
		return -1;
	}

	if (raw_get_file_format(signature) != BMP_FILE) {
		printf("Error : it's not BMP file\n");
		close(fd);
		return -1;
	}

	if (read(fd, &bmp_head, sizeof(struct bmp_header)) != sizeof(struct bmp_header)) {
		printf("Error : can't read header of bmp file\n");
		close(fd);
		return -1;
	}

	bmp_data->bits_per_pixel = bmp_head.biBitCount;
	bmp_data->width = (width = bmp_head.biWidth);
	bmp_data->height = (height = bmp_head.biHeight);
	bmp_data->image_size = width * height * 3;

	bmp_data->image = (uint8_t*)malloc(bmp_data->image_size * sizeof(uint8_t));

	switch (bmp_head.biBitCount) {
		case 32:
			lseek(fd, bmp_head.bfOffBits, SEEK_SET);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					read(fd, buff, 4);
					bmp_data->image[pos] = buff[2];
					bmp_data->image[pos + 1] = buff[1];
					bmp_data->image[pos + 2] = buff[0];
					pos += 3;
				}
			}
			break;

		case 24:
			lseek(fd, bmp_head.bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width * 3);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					read(fd, buff, 3);
					bmp_data->image[pos] = buff[2];
					bmp_data->image[pos + 1] = buff[1];
					bmp_data->image[pos + 2] = buff[0];
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
			read_pallete(fd, pallete, (bmp_head.bfOffBits - HEADER_SIZE) / COLOR_SIZE);
			lseek(fd, bmp_head.bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					read(fd, buff, 1);
					bmp_data->image[pos] = pallete[buff[0]].blue;
					bmp_data->image[pos + 1] = pallete[buff[0]].green;
					bmp_data->image[pos + 2] = pallete[buff[0]].red;
					pos += 3;
				}
				if (skip) {
					read(fd, buff, skip);
				}
			}
			break;

		case 4:
			read_pallete(fd, pallete, (bmp_head.bfOffBits - HEADER_SIZE) / COLOR_SIZE);
			lseek(fd, bmp_head.bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width / 2 + width % 2);
			for (i = 0; i < height; i++) {
				for (j = 0; j < width / 2; j++) {
					read(fd, buff, 1);
					buff[1] = buff[0]>>4;
					buff[2] = buff[0] & 0x0f;
					bmp_data->image[pos + 5] = pallete[buff[1]].red;
					bmp_data->image[pos + 4] = pallete[buff[1]].green;
					bmp_data->image[pos + 3] = pallete[buff[1]].blue;
					bmp_data->image[pos + 2] = pallete[buff[2]].red;
					bmp_data->image[pos + 1] = pallete[buff[2]].green;
					bmp_data->image[pos] = pallete[buff[2]].blue;
					pos += 6;
				}
				if (width % 2) {
					read(fd, buff, 1);
					buff[1] = buff[0]>>4;
					bmp_data->image[pos + 2] = pallete[buff[1]].red;
					bmp_data->image[pos + 1] = pallete[buff[1]].green;
					bmp_data->image[pos] = pallete[buff[1]].blue;
					pos += 3;
				}
				if (skip) {
					read(fd, buff, skip);
				}
			}
			break;

		case 1:
			lseek(fd, bmp_head.bfOffBits, SEEK_SET);
			skip = FIND_SKIP(width / 8 + (width % 8 ? 1 : 0));
			for (i = 0; i < height; i++) {
				for (j = 0; j < width / 8; j++) {
					read(fd, buff, 1);
					for (k = 0; k < 8; k++) {
						if (buff[0] & 0x80) {
							bmp_data->image[pos] = 255;
							bmp_data->image[pos + 1] = 255;
							bmp_data->image[pos + 2] = 255;
							bmp_data->image[pos + 3] = 255;
							pos += 3;
						} else {
							bmp_data->image[pos] = 0;
							bmp_data->image[pos + 1] = 0;
							bmp_data->image[pos + 2] = 0;
							pos += 3;
						}
						buff[0] = buff[0]<<1;
					}
				}

				if (width % 8) {
					read(fd, buff, 1);
					for (k = 0; k < width % 8; k++) {
						if (buff[0] & 0x80) {
							bmp_data->image[pos] = 255;
							bmp_data->image[pos + 1] = 255;
							bmp_data->image[pos + 2] = 255;
							pos += 3;
						} else {
							bmp_data->image[pos] = 0;
							bmp_data->image[pos + 1] = 0;
							bmp_data->image[pos + 2] = 0;
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
			close(fd);
			return -1;
	}

	/* reverse pixels in image */
	for (i = 0; i < height / 2; i++) {
		for (j = 0; j < width * 3; j++) {
			tmp = bmp_data->image[width * 3 * (height - 1 - i) + j];
			bmp_data->image[width * 3 * (height - 1 - i) + j] = bmp_data->image[width * 3 * i + j];
			bmp_data->image[width * 3 * i + j] = tmp;
		}
	}

	close(fd);
	return 0;
}
