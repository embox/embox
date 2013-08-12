/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <string.h>
#include <asm/io.h>
#include <drivers/diag.h>
#include <drivers/video/vga.h>
#include <drivers/iodev.h>
#include <drivers/video_term.h>
#include <util/member.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(vc_init);

/* The video memory address. */
#define VIDEO          0xB8000

typedef struct vchar {
	char c;
	char a;
}__attribute__((packed)) vchar_t;

struct diag_vterm_data {
	char attr;
	vchar_t *video;
};

struct vga_vterm_video {
	struct vterm_video video;
	struct diag_vterm_data data;
};

static void diag_vterm_init(struct vterm_video *t) {
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

static void diag_vterm_cursor(struct vterm_video *t, unsigned short x, unsigned short y) {
	unsigned int pos;

	pos = x + y * t->width;
	out16((pos & 0xff00) | 0x0e, VGA_CRTC_INDEX);
	out16((pos << 8) | 0x0f, VGA_CRTC_INDEX);
}

static void diag_vterm_putc(struct vterm_video *t, char ch, unsigned short x, unsigned short y) {
	struct diag_vterm_data *data = &member_cast_out(t, struct vga_vterm_video, video)->data;

	data->video[x + y * t->width] = (vchar_t) {.c = ch, .a = data->attr};
}


static void diag_vterm_clear_rows(struct vterm_video *t, short row, unsigned short count){
	struct diag_vterm_data *data = &member_cast_out(t, struct vga_vterm_video, video)->data;

	for (int i = row * t->width; i < (row + count) * t->width; ++i){
		data->video[i] = (vchar_t) {.c = ' ', .a = data->attr};
	}
}

static void diag_vterm_copy_rows(struct vterm_video *t,
		unsigned short to, unsigned short from, short nrows) {
	struct diag_vterm_data *data = &member_cast_out(t, struct vga_vterm_video, video)->data;

	memmove(&data->video[to * t->width],
			&data->video[from * t->width],
			sizeof(data->video[0]) * nrows * t->width);
}

static const struct vterm_video_ops vc_vga_ops = {
		.init = &diag_vterm_init,
		.cursor = &diag_vterm_cursor,
		.putc = &diag_vterm_putc,
		.clear_rows = &diag_vterm_clear_rows,
		.copy_rows = &diag_vterm_copy_rows
};

static struct vga_vterm_video vc_vga_video = {
		.data = { .attr = 0x7, .video = (vchar_t *) VIDEO }
};


/*file ops */

static struct vterm vc_vterm;

#include <embox/device.h> //XXX

static int vc_open(struct node *node, struct file_desc *file_desc,
	int flags);
static int vc_close(struct file_desc *desc);
static size_t vc_read(struct file_desc *desc, void *buf, size_t size);
static size_t vc_write(struct file_desc *desc, void *buf, size_t size);
static int vc_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations vc_file_ops = {
	.open = vc_open,
	.close = vc_close,
	.read = vc_read,
	.write = vc_write,
	.ioctl = vc_ioctl
};

/*
 * file_operations
 */
static int vc_open(struct node *node, struct file_desc *desc, int flags) {

	vterm_video_init((struct vterm_video *) &vc_vga_video, &vc_vga_ops, 80, 24);
	vterm_init(&vc_vterm, (struct vterm_video *) &vc_vga_video, NULL);

	vterm_open_indev(&vc_vterm, "keyboard");

	assert(desc);
	desc->ops = &vc_file_ops;
	desc->file_info = &vc_vterm;

	return 0;

}

static int vc_close(struct file_desc *desc) {
	return 0;
}

static size_t vc_read(struct file_desc *desc, void *buff, size_t size) {

	return tty_read(&vc_vterm.tty, (char *) buff, size);
}

static size_t vc_write(struct file_desc *desc, void *buff, size_t size) {
	size_t cnt;
	char *b;

	for (cnt = size, b = (char *) buff; cnt > 0; b++, cnt--) {
		vterm_putc(&vc_vterm, *b);
	}

	return size;
}

static int vc_ioctl(struct file_desc *desc, int request, ...) {
	va_list va;
	void *data;

	va_start(va, request);
	data = va_arg(va, void *);
	va_end(va);

	return tty_ioctl(&vc_vterm.tty, request, data);
}

static int vc_init(void) {
	return char_dev_register("vc", &vc_file_ops);
}
