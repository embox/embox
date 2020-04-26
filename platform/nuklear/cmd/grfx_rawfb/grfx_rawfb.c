/*
 * MIT License
 *
 * Copyright (c) 2016-2017 Patrick Rudolph <siro@das-labor.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Based on x11/main.c.
 *
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <inttypes.h>
#include <sys/mman.h>

#include <drivers/video/fb.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIBSHM_IMPLEMENTATION
#define NK_RAWFB_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_SOFTWARE_FONT

#include "nuklear.h"
#include "nuklear_rawfb.h"

#define DTIME           20
#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
int main(int argc, char *argv[]) {
	struct rawfb_context *rawfb;
	void *fb_buf = NULL;
	unsigned char *tex_scratch;
	long int screensize = 0;
	uint8_t *fbp = 0;
	struct fb_info *fb_info;
	int bpp;
	uint32_t width = 0, height = 0;

	fb_info = fb_lookup(0);

	printf("%" PRId32 "x%" PRId32 ", %" PRId32 "bpp\n", fb_info->var.xres, fb_info->var.yres,
			fb_info->var.bits_per_pixel);

	bpp = fb_info->var.bits_per_pixel / 8;

	/* Figure out the size of the screen in bytes */
	screensize = fb_info->var.xres * fb_info->var.yres * bpp;

	/* Map the device to memory */
	fbp = (uint8_t *) mmap_device_memory((void *) fb_info->screen_base,
			screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
			(uint64_t) ((uintptr_t) fb_info->screen_base));
	if ((int) fbp == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");

	width = fb_info->var.xres;
	height = fb_info->var.yres;

	fb_buf = malloc(height * width * 4);
	if (!fb_buf) {
		fprintf(stderr, "Cannot allocate buffer for screen\n");
		exit(2);
	}
	tex_scratch = malloc(512 * 512);
	if (!tex_scratch) {
		fprintf(stderr, "Cannot allocate buffer for font\n");
		exit(2);
	}

	rawfb = nk_rawfb_init(fb_buf, tex_scratch, width, height, width * 4,
		PIXEL_LAYOUT_XRGB_8888);
	if (!rawfb) {
		fprintf(stderr, "Cannot allocate rawfb\n");
		exit(2);
	}

	while (1) {
		/* Input */

		/* GUI */
		if (nk_begin(&rawfb->ctx, "Demo", nk_rect(50, 50, 200, 200),
			NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|
			NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
			enum {EASY, HARD};
			static int op = EASY;
			static int property = 20;
			static float value = 0.6f;

			nk_layout_row_static(&rawfb->ctx, 30, 80, 1);
			if (nk_button_label(&rawfb->ctx, "button"))
				fprintf(stdout, "button pressed\n");
			nk_layout_row_dynamic(&rawfb->ctx, 30, 2);
			if (nk_option_label(&rawfb->ctx, "easy", op == EASY)) op = EASY;
			if (nk_option_label(&rawfb->ctx, "hard", op == HARD)) op = HARD;
			nk_layout_row_dynamic(&rawfb->ctx, 25, 1);
			nk_property_int(&rawfb->ctx, "Compression:", 0, &property, 100, 10, 1);

			nk_layout_row_begin(&rawfb->ctx, NK_STATIC, 30, 2);
			{
				nk_layout_row_push(&rawfb->ctx, 50);
				nk_label(&rawfb->ctx, "Volume:", NK_TEXT_LEFT);
				nk_layout_row_push(&rawfb->ctx, 110);
				nk_slider_float(&rawfb->ctx, 0, &value, 1.0f, 0.1f);
			}
			nk_layout_row_end(&rawfb->ctx);
		}
		nk_end(&rawfb->ctx);
		if (nk_window_is_closed(&rawfb->ctx, "Demo")) break;

		/* Draw framebuffer */
		nk_rawfb_render(rawfb, nk_rgb(30,30,30), 1);

		{
			int i, j;
			void *rb = fb_buf, *fb = fb_info->screen_base;

			for (i = 0; i < height; i++) {
				for (j = 0; j < width; j++) {
					pix_fmt_convert(rb, fb, 1, BGRA8888, fb_info->var.fmt);
					rb += 4;
					fb += bpp;
				}
			}
		}

		usleep(1000 * 100);
	}

	nk_rawfb_shutdown(rawfb);

	free(fb_buf);
	free(tex_scratch);

	return 0;
}
