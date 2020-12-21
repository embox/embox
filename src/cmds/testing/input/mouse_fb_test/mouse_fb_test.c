/**
 * @file
 * @brief Mouse test - draws mouse, print position and other events
 *
 * @date 08.04.2020
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <drivers/input/input_dev.h>
#include <drivers/video/fb.h>

#define CURSOR_HEIGHT 12
#define CURSOR_WIDTH  6

static int16_t mouse_x, mouse_y;
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

static void draw_cursor(int x, int y, int color) {
	struct fb_fillrect rect;

	rect.dx = x - CURSOR_WIDTH / 2;
	rect.dy = y - CURSOR_HEIGHT / 2;
	rect.width = CURSOR_WIDTH;
	rect.height = CURSOR_HEIGHT;
	rect.color = color_convert(color);
	rect.rop = ROP_COPY;

	fb_fillrect(fb, &rect);
}

static int mouse_handle(int fd) {
	struct input_event ev;

	while (1) {
		if (read(fd, &ev, sizeof ev) <= 0) {
			continue;
		}

		draw_cursor(mouse_x, mouse_y, 0xffffff);

		mouse_x += (ev.value >> 16) & 0xffff;
		mouse_y -= ev.value & 0xffff;

		mouse_x = normalize_coord(mouse_x, CURSOR_WIDTH,
			fb->var.xres - CURSOR_WIDTH);
		mouse_y = normalize_coord(mouse_y, CURSOR_HEIGHT,
			fb->var.yres - CURSOR_HEIGHT);

		draw_cursor(mouse_x, mouse_y, 0x000000);

		printf("pos = (%d, %d), buttons = %c %c %c\n",
			mouse_x, mouse_y,
			ev.type & MOUSE_BUTTON_LEFT   ? 'L' : ' ',
			ev.type & MOUSE_BUTTON_MIDDLE ? 'M' : ' ',
			ev.type & MOUSE_BUTTON_RIGHT  ? 'R' : ' '
		);
	}

	return 0;
}

static void print_usage(const char *cmd) {
	printf("Usage: %s [-h] <mouse>\n", cmd);
}

int main(int argc, char **argv) {
	int opt;
	int fd;

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

	fd = open(argv[argc - 1], O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "Cannot find mouse \"%s\"\n", argv[argc - 1]);
		return -1;
	}

	if (0 > init_fb()) {
		fprintf(stderr, "Framebuffer color filling error\n");
		close(fd);
		return -1;
	}

	/* Testing for infinite time. */
	mouse_handle(fd);

	return 0;
}
