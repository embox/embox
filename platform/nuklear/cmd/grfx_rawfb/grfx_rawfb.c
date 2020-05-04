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
#include <drivers/input/input_dev.h>

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

static inline int normalize_coord(int x, int a, int b) {
	if (x < a) {
		return a;
	} else if (x > b - 1) {
		return b - 1;
	} else {
		return x;
	}
}

static void handle_mouse(struct input_dev *mouse, struct fb_info *fb_info,
		struct rawfb_context *rawfb) {
	static int16_t mouse_x = 0, mouse_y = 0;
	struct input_event ev;

	while (0 <= input_dev_event(mouse, &ev)) {
		mouse_x += (ev.value >> 16) & 0xffff;
		mouse_y -= ev.value & 0xffff;

		mouse_x = normalize_coord(mouse_x, 0, fb_info->var.xres);
		mouse_y = normalize_coord(mouse_y, 0, fb_info->var.yres);

		nk_input_motion(&rawfb->ctx, mouse_x, mouse_y);
		nk_input_button(&rawfb->ctx, NK_BUTTON_LEFT, mouse_x, mouse_y, ev.type & 0x1);
		nk_input_button(&rawfb->ctx, NK_BUTTON_RIGHT, mouse_x, mouse_y, ev.type & 0x2);
		nk_input_button(&rawfb->ctx, NK_BUTTON_MIDDLE, mouse_x, mouse_y, ev.type & 0x4);

		/* printf("pos = (%d, %d)\n", mouse_x, mouse_y); */
	}
}

static void handle_touchscreen(struct input_dev *ts, struct fb_info *fb_info,
		struct rawfb_context *rawfb) {
	struct input_event ev;
	int type;
	static int x = 0, y = 0;

	while (0 <= input_dev_event(ts, &ev)) {
		type = ev.type & ~TS_EVENT_NEXT;

		switch (type) {
		case TS_TOUCH_1:
			x = normalize_coord((ev.value >> 16) & 0xffff, 0, fb_info->var.xres);
			y = normalize_coord(ev.value & 0xffff, 0, fb_info->var.yres);
			nk_input_button(&rawfb->ctx, NK_BUTTON_LEFT, x, y, 1);
			nk_input_motion(&rawfb->ctx, x, y);
			break;
		case TS_TOUCH_1_RELEASED:
			nk_input_button(&rawfb->ctx, NK_BUTTON_LEFT, x, y, 0);
			break;
		default:
			break;
		}

	}
}

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
	struct input_dev *mouse;

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

	/* Input device - mouse. */
	if (!(mouse = input_dev_lookup(argv[argc - 1]))) {
		fprintf(stderr, "Cannot find mouse \"%s\"\n", argv[argc - 1]);
		exit(1);
	}
	if (0 > input_dev_open(mouse, NULL)) {
		fprintf(stderr, "Failed open mouse input device\n");
		exit(1);
	}

	while (1) {
		/* Input */
		nk_input_begin(&rawfb->ctx);
		{
			switch (mouse->type) {
			case INPUT_DEV_MOUSE:
				handle_mouse(mouse, fb_info, rawfb);
				break;
			case INPUT_DEV_TOUCHSCREEN:
				handle_touchscreen(mouse, fb_info, rawfb);
				break;
			default:
				/* Unreachable */
				break;
			}
		}
		nk_input_end(&rawfb->ctx);

		/* GUI */
		if (nk_begin(&rawfb->ctx, "Demo", nk_rect(50, 50, 200, 200),
			NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|
			NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
			enum {EASY, HARD};
			static int op = EASY;
			static int property = 20;
			static float value = 0.6f;

			if (mouse->type == INPUT_DEV_TOUCHSCREEN) {
				/* Do not show cursor when using touchscreen */
				nk_style_hide_cursor(&rawfb->ctx);
			}

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

		if (fb_info->var.fmt != BGRA8888) {
			pix_fmt_convert(fb_buf, fb_info->screen_base, width * height,
							BGRA8888, fb_info->var.fmt);
		} else {
			memcpy(fb_info->screen_base, fb_buf, width * height * bpp);
		}
	}

	nk_rawfb_shutdown(rawfb);

	free(fb_buf);
	free(tex_scratch);

	return 0;
}
