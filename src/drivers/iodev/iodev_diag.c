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
}__attribute__((packed)) vchar_t;

/* The video memory address. */
#define VIDEO          0xB8000

struct diag_tty_data {
	char attr;
	vchar_t *video;
};

static void diag_tty_init(struct vterm *t) {
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

static void diag_tty_cursor(struct vterm *t, unsigned short x, unsigned short y) {
	unsigned int pos;

	pos = x + y * t->width;
	out16((pos & 0xff00) | 0x0e, VGA_CRTC_INDEX);
	out16((pos << 8) | 0x0f, VGA_CRTC_INDEX);
}

static void diag_tty_putc(struct vterm *t, char ch, unsigned short x, unsigned short y) {
	struct diag_tty_data *data;

	data = (struct diag_tty_data *) t->data;
	data->video[x + y * t->width] = (vchar_t) {.c = ch, .a = data->attr};
}


static void diag_clear_strip(struct vterm *t, short row, unsigned short count){
	struct diag_tty_data *data = t->data;

	for (int i = row * t->width; i < (row + count) * t->width; ++i){
		data->video[i] = (vchar_t) {.c = ' ', .a = data->attr};
	}
}

#if 0
static void diag_vterm_copy_rows(struct vterm *t,
		unsigned short to, unsigned short from, short nrows) {
	struct diag_tty_data *data = t->data;

	memmove(data->video[to * t->width],
			data->video[from * t->width],
			sizeof(data->video[0]) * nrows * t->width);
}
#endif

void diag_vterm_scroll(struct vterm *t, short nrows) {
	struct diag_tty_data *data = t->data;

	if (nrows > 0) {
		memmove(data->video,
				data->video + nrows * t->width,
				t->width * (t->height - nrows) * sizeof data->video[0]);

		diag_clear_strip(t, t->height - nrows, nrows);

	} else {
		nrows = - nrows;
		memmove(data->video + nrows * t->width,
				data->video,
				t->width * (t->height - nrows) * sizeof data->video[0]);

		diag_clear_strip(t, 0, nrows);
	}
}

static void diag_tty_clear(struct vterm *t, unsigned short x, unsigned short y,
		unsigned short width, unsigned short height) {
        uint32_t i, j;
        struct diag_tty_data *data;

        data = (struct diag_tty_data *) t->data;

        width = x + width > t->width ? t->width - x : width;
        height = y + height > t->height ? t->height - y : height;

        if ((width == 0) || (height == 0))
                return;

        for (i = x; i < x + width; ++i) {
                for (j = y; j < y + height; ++j) {
                        data->video[i + j * t->width] =
                                        (vchar_t) {.c = 0x20, .a = data->attr};
                }
        }
}

static const struct vterm_ops diag_tty_ops = {
		.init = &diag_tty_init,
		.cursor = &diag_tty_cursor,
		.putc = &diag_tty_putc,
		.clear_strip = &diag_clear_strip,
		.clear = &diag_tty_clear,
		.scroll = &diag_vterm_scroll
};

static int iodev_diag_init(void) {
	static struct diag_tty_data data =
			{ .attr = 0x7, .video = (vchar_t *) VIDEO };

	vterm_init(&diag_tty, 80, 24, &diag_tty_ops, &data);
	return 0;
}

const struct iodev_ops iodev_diag_ops_struct = { .init = &iodev_diag_init,
		.getc = &diag_getc, .putc = &diag_putc, .kbhit = &diag_kbhit };

struct vterm diag_tty;
const struct iodev_ops * const iodev_diag_ops = &iodev_diag_ops_struct;
