/**
  * @file png.h
  * @brief lib for check, decode and draw PNG
  * @author Filipp Chubukov
  * @version
  * @date 17.06.2019
*/
#ifndef LIB_PNG_H_
#define LIB_PNG_H_

#define PNG_PLT 1 /* pallete */
#define PNG_GRSA 2 /* greyscale + alpha */
#define PNG_TRCL 3 /* truecolor */
#define PNG_TRCLA 4 /* truecolor + alpha */

struct png {
	uint32_t width;
	uint32_t height;
	uint32_t color;
	uint32_t image_size;

	uint8_t *image; /* RGBA pixels */
};

/**
  * @brief fill buffer with image
  *
  * @param *file_name link to the png file
  * @param *png_data - buffer for png info and data
  *
  * @return 0 if successed or error
*/
extern int png_load(char *file_name, struct png *png_data);

/**
  * @brief free memory
  *
  * @param *png_data - pointer to the png data
  *
*/
extern void png_unload(struct png *png_data);

#endif /* LIB_PNG_H_ */
