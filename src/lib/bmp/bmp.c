/**
 * @file
 * @brief XWnd library for *.bmp files loading and output implementation
 *
 * @date Oct 3, 2012
 * @author Alexandr Chernakov
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <drivers/vesa.h>
#include <xwnd/xwnd.h>
#include <xwnd/draw_helpers.h>
#include <xwnd/bmp.h>

static enum xwnd_bmp_err xwnd_bmp_errno;

inline static unsigned int bmp_4_lup(unsigned int x) {
	return(x / 4 +((x % 4) ? 1 : 0)) * 4;
}

inline static int not_alignment(unsigned i, unsigned wd) {
	return(i % bmp_4_lup(wd)) < wd;
}

static int xwnd_bmp_check_signature(FILE * f) {
	char a, b;
	fseek(f, XWND_BMP_MAGIC, SEEK_SET);
	fread(&a, 0x01, 1, f);
	fread(&b, 0x01, 1, f);
	if((a != 'B') ||(b != 'M')) {
		return 0;
	}
	return 1;
}

static unsigned int xwnd_bmp_get_array_offseet(FILE * f) {
	unsigned int array_off;
	fseek(f, XWND_BMP_PX_OFFSET, SEEK_SET);
	fread(&array_off, 0x04, 1, f);
	return array_off;
}

static void xwnd_bmp_load_header(FILE * f, struct xwnd_bmp_image * bmp) {
	fseek(f, XWND_BMP_WIDTH, SEEK_SET);
	fread(&(bmp->width), 0x04, 1, f);
	fseek(f, XWND_BMP_HEIGHT, SEEK_SET);
	fread(&(bmp->height), 0x04, 1, f);
	fseek(f, XWND_BMP_BPP, SEEK_SET);
	fread(&(bmp->bpp), 0x02, 1, f);
}

static void xwnd_bmp_load_abortion (struct xwnd_bmp_image * bmp, FILE * f, enum xwnd_bmp_err err) {
	xwnd_bmp_errno = err;
	xwnd_bmp_unload(bmp);
	if (f) {
		fclose(f);
	}
}

static void xwnd_bmp_rd_24_bitmap(FILE * f, struct xwnd_bmp_image * bmp) {
	unsigned int i, j;
	unsigned char x;
	for(i = 0; i < bmp->width * bmp->height; i++) {
		fread(&x, 0x01, 1, f);
		fread(&x, 0x01, 1, f);
		fread(&x, 0x01, 1, f);
		if(not_alignment(i, bmp->width)) {
			bmp->pxls[j] = x?15:0;
			j++;
		}
	}

}

static void xwnd_bmp_rd_8_bitmap(FILE * f, struct xwnd_bmp_image * bmp) {
	unsigned int i, j;
	unsigned char x;
	for(i = 0, j = 0; i <(bmp_4_lup(bmp->width)) * bmp->height; i++) {
		fread(&x, 0x01, 1, f);
		if(not_alignment(i, bmp->width)) {
			bmp->pxls[j] = x % 0x10;
			j++;
		}
	}
}

struct xwnd_bmp_image * xwnd_bmp_load(const char * file) {
/* FIXME: make it suitable for different graphic modes*/
	FILE * f = NULL;
	struct xwnd_bmp_image * bmp = NULL;
	unsigned int array_off;

	bmp = malloc(sizeof(struct xwnd_bmp_image));
	if(!bmp) {
		xwnd_bmp_load_abortion(bmp, f, XWND_BMP_FAIL_MEM);
		return NULL;
	}

	bmp->pxls = NULL;
	bmp->bpp = 0;
	xwnd_bmp_errno = XWND_BMP_OK;

	f = fopen(file, "r");
	if(!f) {
		xwnd_bmp_load_abortion(bmp, f, XWND_BMP_FAIL_FILE);
		return NULL;
	}

	if(!xwnd_bmp_check_signature(f)) {
		xwnd_bmp_load_abortion(bmp, f, XWND_BMP_FAIL_SIGN);
		return NULL;
	}

	array_off = xwnd_bmp_get_array_offseet(f);
	xwnd_bmp_load_header(f, bmp);

	fseek(f, array_off, SEEK_SET);

	bmp->pxls = malloc(bmp->width * bmp->height);
	if(!bmp->pxls) {
		xwnd_bmp_load_abortion(bmp, f, XWND_BMP_FAIL_MEM);
		return NULL;
	}
	/*ToDo: Get rid of switch, add normal collor mapping*/
	switch (bmp->bpp) {
	case 24:
		xwnd_bmp_rd_24_bitmap(f, bmp);
		break;
	case 8:
		xwnd_bmp_rd_8_bitmap(f, bmp);
		break;
	default:
		xwnd_bmp_load_abortion(bmp, f, XWND_BMP_FAIL_UNSUPPORTED);
		return NULL;
	}

	fclose(f);
	return bmp;
}

void xwnd_bmp_unload(struct xwnd_bmp_image * img) {
	if(img) {
		if(img->pxls)
			free(img->pxls);
		free(img->pxls);
	}
}

void xwnd_bmp_draw(struct xwnd_bmp_image * img) {
	int x, y;
	for(x = 0; x < img->width; x++) {
		for(y = 0; y < img->height; y++) {
			vesa_put_pixel(x, img->height - y - 1, img->pxls[y*img->width+x]);
		}
	}

#if 0
	xwnd_draw_vert_line(0, 0, img->height, 2);
	xwnd_draw_horiz_line(0, 0, img->width, 2);
	xwnd_draw_vert_line(img->width, 0, img->height, 2);
	xwnd_draw_horiz_line(0, img->height, img->width, 2);
#endif

}

void xwnd_bmp_output(const struct xwnd_window * wnd, struct xwnd_bmp_image * img) {
	int x, y;
	for(x = 0; x < img->width; x++) {
		for(y = 0; y < img->height; y++) {
			xwnd_draw_pixel(wnd, x, img->height - y - 1, img->pxls[y*img->width+x]);
		}
	}
}

enum xwnd_bmp_err xwnd_bmp_get_errno (void) {
	return xwnd_bmp_errno;
}
