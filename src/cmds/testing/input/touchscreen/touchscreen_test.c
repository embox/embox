/**
 * @file
 * @brief Touchscreen test - draws touches, print position and other events
 *
 * @date   15.04.2020
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <stdio.h>
#include <drivers/input/input_dev.h>
#include <drivers/input/touchscreen.h>
#include <drivers/video/fb.h>

/* Cursor is 12 x 12 */
#define CURSOR_DEFAULT_SIZE 12

static const int ts_cursor_color[2] = { 0x000000, 0xff0000 };
static int16_t ts_x[2], ts_y[2];
static uint16_t ts_cursor_size[2];
static struct fb_info *fb;

static uint32_t color_convert(uint32_t in) {
	uint32_t out;
	pix_fmt_convert(&in, &out, 1, RGB888, fb->var.fmt);
	return out;
}

static int init_fb(void) {
	struct fb_fillrect rect;

	fb = fb_lookup(0);

	if (!fb) {
		fprintf(stderr, "Cannot open framebuffer\n");
		return -1;
	}

	rect.dx = rect.dy = 0;
	rect.width = fb->var.xres;
	rect.height = fb->var.yres;
	rect.color = color_convert(0xffffff);
	rect.rop = ROP_COPY;

	fb_fillrect(fb, &rect);

	return 0;
}

static int normalize_coord(int x, int a, int b) {
	if (x < a) {
		return a;
	} else if (x > b - 1) {
		return b - 1;
	} else {
		return x;
	}
}

static void draw_cursor(int i, int x, int y, int color) {
	struct fb_fillrect rect;

	rect.dx = x - (ts_cursor_size[i] + 1) / 2;
	rect.dy = y - (ts_cursor_size[i] + 1) / 2;
	rect.width = ts_cursor_size[i];
	rect.height = ts_cursor_size[i];
	rect.color = color_convert(color);
	rect.rop = ROP_COPY;

	fb_fillrect(fb, &rect);
}

static void handle_touch(int i, int x, int y, int pressure) {
	/* Clear cursor */
	draw_cursor(i, ts_x[i], ts_y[i], 0xffffff);

	/* Update new area because of new pressure. */
	ts_cursor_size[i] = pressure;

	/* Update cursor */
	ts_x[i] = normalize_coord(x, ts_cursor_size[i],
		fb->var.xres - ts_cursor_size[i]);
	ts_y[i] = normalize_coord(y, ts_cursor_size[i],
		fb->var.yres - ts_cursor_size[i]);
	draw_cursor(i, ts_x[i], ts_y[i], ts_cursor_color[i]);

	printf("finger%d = (x=%d, y=%d)\n", i, ts_x[i], ts_y[i]);
}

static int ts_handle(struct input_dev *ts) {
	struct input_event ev;
	int type;
	int pressure = CURSOR_DEFAULT_SIZE, new_x = 0, new_y = 0, touch = -1;

	while (0 <= input_dev_event(ts, &ev)) {
		type = ev.type & ~TS_EVENT_NEXT;

		switch (type) {
		case TS_TOUCH_1:
			touch = 0;
			new_x = (ev.value >> 16) & 0xffff;
			new_y = ev.value & 0xffff;
			break;
		case TS_TOUCH_2:
			touch = 1;
			new_x = (ev.value >> 16) & 0xffff;
			new_y = ev.value & 0xffff;
			break;
		case TS_TOUCH_1_RELEASED:
			printf("Finger 0 released\n");
			break;
		case TS_TOUCH_2_RELEASED:
			printf("Finger 1 released\n");
			break;
		case TS_TOUCH_PRESSURE:
			printf("Pressure = %d\n", ev.value);
			pressure = ev.value / 3;
			break;
		default:
			printf("event (type=%d, value=%d)\n", ev.type, ev.value);
			break;
		}

		if (ev.type & TS_EVENT_NEXT) {
			continue;
		}

		if (touch != -1 && pressure) {
			handle_touch(touch, new_x, new_y, pressure);
		}
		touch = -1;
	}

	return 0;
}

static void print_usage(const char *cmd) {
	printf("Usage: %s [-h] <ts>\n", cmd);
}

int main(int argc, char **argv) {
	int opt;
	struct input_dev *ts;

	if (argc < 2) {
		print_usage(argv[0]);
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			print_usage(argv[0]);
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	/* Set default cursor size. */
	memset(ts_x, 0, sizeof ts_x);
	memset(ts_y, 0, sizeof ts_y);
	ts_cursor_size[0] = ts_cursor_size[1] = CURSOR_DEFAULT_SIZE;

	if (!(ts = input_dev_lookup(argv[argc - 1]))) {
		fprintf(stderr, "Cannot find touchscreen \"%s\"\n", argv[argc - 1]);
		return -1;
	}

	if (0 > input_dev_open(ts, ts_handle)) {
		fprintf(stderr, "Failed open ts input device\n");
		return -1;
	}

	if (0 > init_fb()) {
		fprintf(stderr, "Framebuffer color filling error\n");
		input_dev_close(ts);
		return -1;
	}

	/* Testing for infinite time. */
	while (1) {
	}

	return 0;
}
