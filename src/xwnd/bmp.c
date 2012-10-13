/**
 * @file
 * @brief XWnd library for *.bmp files loading and output implementation
 *
 * @date Oct 3, 2012
 * @author Alexandr Chernakov
 */

#include <xwnd/xwnd.h>
#include <xwnd/bmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <vesa.h>

inline static unsigned int bmp_4_lup (unsigned int x) {
	return (x / 4 + ((x % 4) ? 1 : 0)) * 4;
}

inline static int not_alignment (unsigned i, unsigned wd) {
	return (i % bmp_4_lup(wd)) < wd;
}

static int xwnd_bmp_check_signature (FILE * f) {
	char a, b;
	fseek(f, XWND_BMP_MAGIC, SEEK_SET);
	fread(&a, 0x01, 1, f);
	fread(&b, 0x01, 1, f);
	if ((a != 'B') || (b != 'M')) {
		return 0;
	}
	return 1;
}

static unsigned int xwnd_bmp_get_array_offseet (FILE * f) {
	unsigned int array_off;
	fseek(f, XWND_BMP_PX_OFFSET, SEEK_SET);
	fread(&array_off, 0x04, 1, f);
	return array_off;
}

static void xwnd_bmp_load_header (FILE * f, struct xwnd_bmp_image * bmp) {
	fseek(f, XWND_BMP_WIDTH, SEEK_SET);
	fread(&(bmp->width), 0x04, 1, f);
	fseek(f, XWND_BMP_HEIGHT, SEEK_SET);
	fread(&(bmp->height), 0x04, 1, f);
	fseek(f, XWND_BMP_BPP, SEEK_SET);
	fread(&(bmp->bpp), 0x02, 1, f);
}

struct xwnd_bmp_image xwnd_bmp_load (const char * file) {
/* FIXME: make it suitable for different graphic modes*/
	FILE * f;
	struct xwnd_bmp_image bmp;
	unsigned int array_off, i, j;
	bmp.bpp = 0;

	bmp.err = XWND_BMP_OK;
	f = fopen (file, "r");
	if (!f) {
		bmp.err = XWND_BMP_FAIL_FILE;
		return bmp;
	}
	if (!xwnd_bmp_check_signature(f)) {
		bmp.err = XWND_BMP_FAIL_SIGN;
		return bmp;
	}

	array_off = xwnd_bmp_get_array_offseet(f);
	xwnd_bmp_load_header(f, &bmp);

	fseek(f, array_off, SEEK_SET);

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

void xwnd_bmp_unload (struct xwnd_bmp_image * img) {
	if (img->pxls)
		free (img->pxls);
}

void xwnd_bmp_draw (struct xwnd_bmp_image * img) {
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
