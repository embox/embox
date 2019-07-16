/**
  * @file bmp.h
  * @brief lib for read and draw BMP
  * @author Filipp Chubukov
  * @version
  * @date 03.07.2019
*/

#ifndef LIB_BMP_H_
#define LIB_BMP_H_

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

/**
  * @brief check BMP_head
  *
  * @param fd File descriptor
  *
  * @return -1 if not BMP and 0 if BMP
*/
extern int check_bmp(int fd);

/**
  * @brief predict size of buff for image
  *
  * @param fd File descriptor
  *
  * @return size of image or -1 if error
*/
extern int predict_img_size(int fd);

/**
  * @brief fill buffer with image
  *
  * @param fd File descriptor
  * @param image - buffer for image, big enough for image
  *
  * @information colors in image are reversed to BGR
  *
  * @return -1 if error and 0 if successed
*/
extern int get_image_bmp(int fd, struct bmp_header *bmp_head, uint8_t *image);

#endif /* LIB_BMP_H_ */
