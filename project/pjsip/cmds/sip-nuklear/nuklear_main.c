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
#include <fcntl.h>

#include <drivers/video/fb.h>
#include <drivers/input/input_dev.h>

#include "rawfb.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_XLIBSHM_IMPLEMENTATION
#define NK_RAWFB_IMPLEMENTATION
#define NK_INCLUDE_FONT_BAKING
//#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_SOFTWARE_FONT

#include "nuklear.h"
#include "demo/x11_rawfb/nuklear_rawfb.h"

#define STB_IMAGE_IMPLEMENTATION
#include "example/stb_image.h"

#include "main.h"
#include "nuklear_rawfb_with_file_font.h"

#include "nuklear.c"

#define BACKGROUND_COLOR 0xffa0a0a0

static int redraw_required = 1;

static inline int normalize_coord(int x, int a, int b) {
	if (x < a) {
		return a;
	} else if (x > b - 1) {
		return b - 1;
	} else {
		return x;
	}
}

void handle_mouse(int fd, struct fb_info *fb_info,
		struct rawfb_context *rawfb) {
	static int16_t mouse_x = 0, mouse_y = 0;
	struct input_event ev;

	while (1) {
		if (redraw_required) {
			break;
		}

		if (read(fd, &ev, sizeof ev) <= 0) {
			break;
		}

		mouse_x += (ev.value >> 16) & 0xffff;
		mouse_y -= ev.value & 0xffff;

		mouse_x = normalize_coord(mouse_x, 0, fb_info->var.xres);
		mouse_y = normalize_coord(mouse_y, 0, fb_info->var.yres);

		nk_input_motion(&rawfb->ctx, mouse_x, mouse_y);
		nk_input_button(&rawfb->ctx, NK_BUTTON_LEFT, mouse_x, mouse_y, ev.type & MOUSE_BUTTON_LEFT);
		nk_input_button(&rawfb->ctx, NK_BUTTON_RIGHT, mouse_x, mouse_y, ev.type & MOUSE_BUTTON_RIGHT);
		nk_input_button(&rawfb->ctx, NK_BUTTON_MIDDLE, mouse_x, mouse_y, ev.type & MOUSE_BUTTON_MIDDLE);

		redraw_required = 1;

		/* printf("pos = (%d, %d)\n", mouse_x, mouse_y); */
	}
}

void handle_touchscreen(int fd, struct fb_info *fb_info,
		struct rawfb_context *rawfb) {
	struct input_event ev;
	int type;
	static int x = 0, y = 0;

	while (1) {
		if (read(fd, &ev, sizeof ev) <= 0) {
			break;
		}

		type = ev.type & ~TS_EVENT_NEXT;

		switch (type) {
		case TS_TOUCH_1:
			x = normalize_coord((ev.value >> 16) & 0xffff, 0, fb_info->var.xres);
			y = normalize_coord(ev.value & 0xffff, 0, fb_info->var.yres);
			nk_input_button(&rawfb->ctx, NK_BUTTON_LEFT, x, y, 1);
			nk_input_motion(&rawfb->ctx, x, y);

			redraw_required = 1;
			break;
		case TS_TOUCH_1_RELEASED:
			nk_input_button(&rawfb->ctx, NK_BUTTON_LEFT, x, y, 0);

			redraw_required = 1;
			break;
		default:
			break;
		}
	}
}

void demo_nk_on_incoming_call(void) {
	redraw_required = 1;
}

/* ===============================================================
 *
 *                          DEMO
 *
 * ===============================================================*/
int demo_nk_main(const char *input_dev_path) {
	struct rawfb_context *rawfb;
	struct rawfb_fb_info rfb;
	int i;
	void *fb_buf[2] = { NULL };
	unsigned char *tex_scratch;
	long int screensize = 0;
	uint8_t *fbp = 0;
	struct fb_info *fb_info;
	int bpp;
	uint32_t width = 0, height = 0;
	clock_t start_time, cur_time;
	int frames = 0;
	struct nk_image im_accept, im_decline;
	unsigned char *img_data_accept, *img_data_decline;
	int w, h, format;
	float font_sizes[] = RAWFB_FONT_SIZES;
	int input_fd, input_type;

	fb_info = fb_lookup(0);

	printf("%" PRId32 "x%" PRId32 ", %" PRId32 "bpp\n", fb_info->var.xres, fb_info->var.yres,
			fb_info->var.bits_per_pixel);

	bpp = fb_info->var.bits_per_pixel / 8;

	/* Figure out the size of the screen in bytes */
	screensize = fb_info->var.xres * fb_info->var.yres * bpp;

	/* Map the device to memory */
	fbp = (uint8_t *)mmap_device_memory((void *)fb_info->screen_base,
	    screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
	    (uintptr_t)fb_info->screen_base);
	if ((int) fbp == -1) {
		perror("Error: failed to map framebuffer device to memory");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");

	width = fb_info->var.xres;
	height = fb_info->var.yres;

	for (i = 0; i < 2; i++) {
		fb_buf[i] = memalign(64, height * width * 4);
		if (!fb_buf[i]) {
			fprintf(stderr, "Cannot allocate buffer for screen\n");
			goto out_free_fb_buf;
		}
	}

	tex_scratch = malloc(512 * 512);
	if (!tex_scratch) {
		fprintf(stderr, "Cannot allocate buffer for font\n");
		goto out_free_fb_buf;
	}

	rawfb = nk_rawfb_init_with_file_font(fb_buf[0], tex_scratch, width, height, width * 4,
		PIXEL_LAYOUT_XRGB_8888, "fonts/Roboto-Regular.ttf", font_sizes);
	if (!rawfb) {
		fprintf(stderr, "Cannot allocate rawfb\n");
		goto out_free_fb_and_tex;
	}

	input_fd = open(input_dev_path, O_RDONLY | O_NONBLOCK);
	if (input_fd == -1) {
		fprintf(stderr, "Cannot open input device \"%s\"\n", input_dev_path);
		goto out_free;
	}

	/* FIXME */
	if (!strcmp(input_dev_path, "/dev/ps-mouse")) {
		input_type = INPUT_DEV_MOUSE;
	} else {
		input_type = INPUT_DEV_TOUCHSCREEN;
	}

	img_data_accept = stbi_load("icons/phone-accept-80.png", &w, &h, &format, 0);
	im_accept = nk_image_ptr(img_data_accept);
	im_accept.region[0] = 0;
	im_accept.region[1] = 0;
	im_accept.region[2] = w;
	im_accept.region[3] = h;

	img_data_decline = stbi_load("icons/phone-decline-80.png", &w, &h, &format, 0);
	im_decline = nk_image_ptr(img_data_decline);
	im_decline.region[0] = 0;
	im_decline.region[1] = 0;
	im_decline.region[2] = w;
	im_decline.region[3] = h;

	start_time = clock();

	rfb.fb_info = fb_info;
	rfb.fb_buf[0] = fb_buf[0];
	rfb.fb_buf[1] = fb_buf[1];
	rfb.fb_buf_idx = 0;
	rfb.width = width;
	rfb.height = height;
	rfb.bpp = bpp;
	rfb.clear_color = BACKGROUND_COLOR;

	rawfb_init(&rfb);

	while (1) {
		/* Input */
		while (!redraw_required) {
			rawfb_swap_irq_set(0);
			nk_input_begin(&rawfb->ctx);
			{
				switch (input_type) {
				case INPUT_DEV_MOUSE:
					handle_mouse(input_fd, fb_info, rawfb);
					break;
				case INPUT_DEV_TOUCHSCREEN:
					handle_touchscreen(input_fd, fb_info, rawfb);
					break;
				default:
					/* Unreachable */
					break;
				}
			}
			nk_input_end(&rawfb->ctx);
			rawfb_swap_irq_set(1);

			if (call_info->state == CALL_INACTIVE) {
				sleep(0);
				redraw_required = 1;
			}
		}

		/* GUI */
		demo_nk_gui(&rawfb->ctx, &im_accept, &im_decline,
			input_type == INPUT_DEV_MOUSE, &redraw_required);

		nk_rawfb_render(rawfb, nk_rgb(0, 0, 0), 0);

		rawfb_swap_buffers(&rfb);

		//rawfb_clear_screen(&rfb);

		/* Swap nuklear to new FB. */
		nk_rawfb_resize_fb(rawfb, fb_buf[rfb.fb_buf_idx], width, height, width * 4,
			PIXEL_LAYOUT_XRGB_8888);

		frames++;

		cur_time = clock();

		if (cur_time - start_time > 1000) {
			if (0) {
				printf("FPS = %d\n", frames);
			}
			frames = 0;
			start_time = clock();
		}
	}

out_free:
	nk_rawfb_shutdown(rawfb);

out_free_fb_and_tex:
	free(tex_scratch);

out_free_fb_buf:
	for (i = 0; i < 2; i++) {
		if (fb_buf[i]) {
			free(fb_buf[i]);
		}
	}

	return 0;
}
