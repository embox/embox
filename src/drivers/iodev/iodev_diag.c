/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 */

#include <drivers/diag.h>
#include <asm/io.h>
#include <drivers/video/vga.h>
#include <drivers/iodev.h>
#include <drivers/video_term.h>
#include <string.h>

typedef struct vchar {
	char c;
	char a;
} __attribute__((packed)) vchar_t ;

/* The video memory address. */
#define VIDEO          0xB8000

struct diag_tty_data {
	char attr;
	vchar_t *video;
};

static void diag_tty_init(struct video_term *t) {
	/**
	 * mode 3h (80x25 text mode)
	 * 0x67 => 01100111
	 * +---+---+---+--+--+----+---+
	 * |7  |6  |5 4|3 |2 |1   |0  |
	 * |1  |1  |1 0|0 |1 |1   |0  |
	 * |VSP|HSP|   |CS|CS|ERAM|IOS|
	 * 7,6 - 480 lines
	 * 3,2 - 28,322 MHZ Clock
	 * 1 - Enable Ram
	 * 0 - Map 0x3d4 to 0x3b4
	 */
	out16(0x67, VGA_MISC_WRITE);
}

static void diag_tty_cursor(struct video_term *t, uint32_t x, uint32_t y) {
	unsigned int pos;

	pos = x + y * t->width;
	out16((pos & 0xff00) | 0x0e, VGA_CRTC_INDEX);
	out16((pos <<     8) | 0x0f, VGA_CRTC_INDEX);
}

static void diag_tty_putc(struct video_term *t, char ch, uint32_t x, uint32_t y) {
	struct diag_tty_data *data;

	data = (struct diag_tty_data *)t->data;
	data->video[x + y * t->width] = (vchar_t) { .c = ch, .a = data->attr };
}

static void diag_tty_clear(struct video_term *t, uint32_t x, uint32_t y,
		uint32_t width, uint32_t height) {
	uint32_t i, j;
	struct diag_tty_data *data;

	data = (struct diag_tty_data *)t->data;

	width = x + width > t->width ? t->width - x : width;
	height = y + height > t->height ? t->height - y : height;

	if ((width == 0) || (height == 0)) return;

	for (i = x; i < x + width; ++i) {
		for (j = y; j < y + height; ++j) {
			data->video[i + j * t->width] = (vchar_t) { .c = 0x20, .a = data->attr };
		}
	}
}

static void diag_tty_move(struct video_term *t, uint32_t sx, uint32_t sy,
		uint32_t width, uint32_t height, uint32_t dx, uint32_t dy) {
	uint32_t i;
	struct diag_tty_data *data;

	data = (struct diag_tty_data *)t->data;

	width = sx + width > t->width ? t->width - sx : width;
	width = dx + width > t->width ? t->width - dx : width;

	height = sy + height > t->height ? t->height - sy : height;
	height = dy + height > t->height ? t->height - dy : height;

	if ((width == 0) || (height == 0)) return;

	if (sy <= dy) {
		while (height-- != 0) {
			memmove(&data->video[dx + (dy + height) * t->width],
					&data->video[sx + (sy + height) * t->width],
					width * sizeof data->video[0]);
		}
	}
	else {
		for (i = 0; i < height; ++i) {
			memmove(&data->video[dx + (dy + i) * t->width],
					&data->video[sx + (sy + i) * t->width],
					width * sizeof data->video[0]);
		}
	}
}

static const struct vterm_ops diag_tty_ops = {
	.init = &diag_tty_init,
	.cursor = &diag_tty_cursor,
	.putc = &diag_tty_putc,
	.clear = &diag_tty_clear,
	.move = &diag_tty_move
};

static int iodev_diag_init(void) {
	static struct diag_tty_data data = {
		.attr = 0x7,
		.video = (vchar_t *)VIDEO
	};

	vterm_init(&diag_tty, 80, 24, &diag_tty_ops, &data);
	return 0;
}

const struct iodev_ops iodev_diag_ops_struct = {
	.init = &iodev_diag_init,
	.getc = &diag_getc,
	.putc = &diag_putc,
	.kbhit = &diag_kbhit
};

struct video_term diag_tty;
const struct iodev_ops *const iodev_diag_ops = &iodev_diag_ops_struct;
