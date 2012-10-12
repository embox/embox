/*
 * bmp.h
 *
 *  Created on: Oct 3, 2012
 *      Author: user
 */

#ifndef BMP_H_
#define BMP_H_

enum xwnd_bmp_err {XWND_BMP_OK, XWND_BMP_FAIL, XWND_BMP_FAIL_FILE, XWND_BMP_FAIL_MEM, XWND_BMP_FAIL_SIGN};

struct xwnd_bmp_image {
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	enum xwnd_bmp_err err;
	char * pxls;
};

struct xwnd_bmp_image xwnd_load_bmp (const char * file);
void xwnd_free_bmp (struct xwnd_bmp_image * img);
void xwnd_draw_bmp_image (struct xwnd_bmp_image * img);

#endif /* BMP_H_ */
