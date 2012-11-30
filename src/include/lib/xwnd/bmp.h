/**
 * @file
 * @brief XWnd library for *.bmp files loading and output
 *
 * @date Oct 3, 2012
 * @author Alexandr Chernakov
 */

#ifndef XWND_BMP_H_
#define XWND_BMP_H_

#include <stdint.h>
#include <lib/xwnd/draw_helpers.h>

#define XWND_BMP_MAGIC 0x00
#define XWND_BMP_PX_OFFSET 0x0A
#define XWND_BMP_WIDTH 0x12
#define XWND_BMP_HEIGHT 0x16
#define XWND_BMP_BPP 0x1C
#define XWND_BMP_BUF_SIZE 0x40

enum xwnd_bmp_err {XWND_BMP_OK, XWND_BMP_FAIL, XWND_BMP_FAIL_FILE, XWND_BMP_FAIL_MEM,
	XWND_BMP_FAIL_SIGN,	XWND_BMP_FAIL_UNSUPPORTED};

struct xwnd_bmp_image {
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
	char * pxls;
};

struct xwnd_bmp_pixel_components {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

union xwnd_bmp_pixel {
	struct xwnd_bmp_pixel_components comps;
	uint8_t px_256;
};

extern struct xwnd_bmp_image * xwnd_bmp_load (const char * file);
extern void xwnd_bmp_unload (struct xwnd_bmp_image * img);
extern void xwnd_bmp_draw (struct xwnd_bmp_image * img);
extern void xwnd_bmp_output(const struct xwnd_window * wnd, struct xwnd_bmp_image * img);
extern enum xwnd_bmp_err xwnd_bmp_get_errno (void);

#endif /* XWND_BMP_H_ */
