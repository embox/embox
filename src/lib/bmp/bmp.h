/**
  * @file bmp.h
  * @brief lib for read and draw BMP
  * @author Filipp Chubukov
  * @version
  * @date 03.07.2019
*/

#ifndef LIB_BMP_H_
#define LIB_BMP_H_

#define BMP_NUM_OF_CHANNELS 3

struct bmp {
	uint32_t width;
	uint32_t height;
	uint32_t bits_per_pixel;
	uint32_t image_size;

	uint8_t *image; /* BGR pixels from bmp file */
};

/**
  * @brief fill buffer with image
  *
  * @param *file_name link to the bmp file
  * @param *bmp_data - buffer for bmp info and data
  *
  * @return -1 if error and 0 if successed
*/
extern int bmp_load(char *file_name, struct bmp *bmp_data);

/**
  * @brief free memory
  *
  * @param *bmp_data - pointer to the bmp data
  *
*/
extern void bmp_unload(struct bmp *bmp_data);

#endif /* LIB_BMP_H_ */
