/*
 * bmp.c
 *
 *  Created on: Oct 3, 2012
 *      Author: user
 */

#include <xwnd/xwnd.h>
#include <xwnd/bmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <vesa.h>

#define XWND_BMP_MAGIC 0x00
#define XWND_BMP_PX_OFFSET 0x0A
#define XWND_BMP_WIDTH 0x12
#define XWND_BMP_HEIGHT 0x16
#define XWND_BMP_BPP 0x1C
#define XWND_BMP_BUF_SIZE 0x40

inline static unsigned int ui_min (unsigned a, unsigned b) {
	return (a<=b)?a:b;
}

inline static unsigned int bmp_4_lup (unsigned int x) {
	return (x / 4 + ((x % 4) ? 1 : 0)) * 4;
}

inline static int not_alignment (unsigned i, unsigned wd) {
	return (i % bmp_4_lup(wd)) < wd;
}

struct xwnd_bmp_image xwnd_load_bmp (const char * file) {
/* FIXME: make it suitable for different graphic modes*/
	FILE * f;
	char buf[XWND_BMP_BUF_SIZE];
	struct xwnd_bmp_image bmp;
	char a, b;
	unsigned int array_off, t_off, i, j;
	bmp.bpp = 0;

	bmp.err = XWND_BMP_OK;
	f = fopen (file, "r");
	if (!f) {
		bmp.err = XWND_BMP_FAIL_FILE;
		return bmp;
	}
	fread(&a, 0x01, 1, f);
	fread(&b, 0x01, 1, f);
	if ((a != 'B') || (b != 'M')) {
		bmp.err = XWND_BMP_FAIL_SIGN;
		fclose(f);
		return bmp;
	}
	else {
		printf ("%c%c\n", a, b);
	}
	fread(buf, 0x08, 1, f);
	fread(&array_off, 0x04, 1, f);
	fread(buf, 0x04, 1, f);
	fread(&(bmp.width), 0x04, 1, f);
	fread(&(bmp.height), 0x04, 1, f);
	fread(buf, 0x02, 1, f);
	fread(&(bmp.bpp), 0x02, 1, f);

	for (t_off = array_off - XWND_BMP_BPP - 2; t_off != 0; ) {
		fread(buf, ui_min(XWND_BMP_BUF_SIZE, t_off), 1, f);
		t_off -= ui_min(XWND_BMP_BUF_SIZE, t_off);
	}

	bmp.pxls = malloc (bmp.width * bmp.height);
	if (!bmp.pxls) {
		bmp.err = XWND_BMP_FAIL_MEM;
		fclose(f);
		return bmp;
	}

	if (bmp.bpp == 24) {
		for (i = 0; i < bmp.width * bmp.height; i++) {
			unsigned char x;
			fread (&x, 0x01, 1, f);
			fread (&x, 0x01, 1, f);
			fread (&x, 0x01, 1, f);
			bmp.pxls[i] = x?15:0;
		}
	}
	else if (bmp.bpp == 8) {
		for (i = 0, j = 0; i < (bmp_4_lup(bmp.width)) * bmp.height; i++) {
			unsigned char x;
			fread (&x, 0x01, 1, f);
			if (not_alignment(i, bmp.width)) {
				bmp.pxls[j] = x % 0x10;
				j++;
			}
		}
	}
	fclose(f);
	return bmp;
}

void xwnd_free_bmp (struct xwnd_bmp_image * img) {
	if (img->pxls)
		free (img->pxls);
}

void xwnd_draw_bmp_image (struct xwnd_bmp_image * img) {
	int x, y;
	for (x = 0; x < img->width; x++) {
		for (y = 0; y < img->height; y++) {
			vesa_put_pixel(x, img->height - y - 1, img->pxls[y*img->width+x]);
		}
	}

	xwnd_draw_vert_line  (0, 0, img->height, 2);
	xwnd_draw_horiz_line (0, 0, img->width, 2);
	xwnd_draw_vert_line  (img->width, 0, img->height, 2);
	xwnd_draw_horiz_line (0, img->height, img->width, 2);

}
