/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    13.08.2013
 */

#include <string.h>
#include <sys/mman.h>

#include <asm/io.h>
#include <drivers/diag.h>
#include <drivers/video/vga.h>
#include <drivers/console/vc/vc_vga.h>
#include <util/member.h>

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

struct vterm_video *vc_vga_init(void) {
	vterm_video_init(&vc_vga_video.video, &vc_vga_ops, 80, 24);

	return &vc_vga_video.video;
}

struct vc_video_diag {
	struct diag diag;
	struct vterm_video *video;
};

static int vc_diag_init(const struct diag *diag) {
	void *ptr;
	//size_t len = 80*25*2;
	size_t len = 0x1000;

	/* Map in the physical memory; 0xb8000 is text mode VGA video memory */
	ptr = mmap_device_memory((void *)0xb8000, len,
	    PROT_READ | PROT_WRITE | PROT_NOCACHE, MAP_FIXED, (uintptr_t)0xb8000);
	if ( ptr == MAP_FAILED ) {
		return -1;
	}

	return vterm_video_init(&vc_vga_video.video, &vc_vga_ops, 80, 24);
}

static void vc_diag_putc(const struct diag *diag, char ch) {
	vterm_video_putc(&vc_vga_video.video, ch);
}

static const struct diag_ops vc_video_diag_ops = {
	.init = vc_diag_init,
	.putc = vc_diag_putc,
};

const struct vc_video_diag DIAG_IMPL_NAME(__EMBUILD_MOD__) = {
	.diag = {
		.ops = &vc_video_diag_ops,
	},
	.video = &vc_vga_video.video,
};

