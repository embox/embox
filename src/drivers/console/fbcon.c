/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.03.2013
 */

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <drivers/input/keymap.h>
#include <drivers/keyboard.h>
#include <drivers/console/mpx.h>
#include <drivers/console/fbcon.h>
#include <drivers/video/fb.h>
#include <drivers/video/font.h>
#include <drivers/tty.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/thread/sched_lock.h>
#include <mem/objalloc.h>
#include <cmd/shell.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(fbcon_init);

volatile struct fbcon *vc_fbcon_cur;

static void inpevent(struct vc *vc, struct input_event *ev);
static void visd(struct vc *vc, struct fb_info *fbinfo);
static void devisn(struct vc *vc);

const struct vc_callbacks thiscbs = {
	.handle_input_event = inpevent,
	.visualized = visd,
	.schedule_devisualization = devisn,
};

static struct fbcon fbcons[VC_MPX_N];

struct video_tty_data {
	const struct font_desc *font;
	int fg_color;
	int bg_color;
	int cur_color;
};

static void inpevent(struct vc *vc, struct input_event *ev) {
	struct fbcon *fbcon = (struct fbcon *) vc;
	unsigned char ascii[4];
	int len;

	if (!(ev->type & KEY_PRESSED)) {
		return;
	}

	len = keymap_to_ascii(ev, ascii);

	if (fbcon->hasc + len >= FBCON_INPB) {
		return;
	}

	memcpy((void *) fbcon->ascii + fbcon->hasc, ascii, len);
	fbcon->hasc += len;

	event_notify(&fbcon->inpevent);


}

static void visd(struct vc *vc, struct fb_info *fbinfo) {
	struct tty *tty;

	vc_fbcon_cur = (struct fbcon *) vc;

	tty = (struct tty *) &vc_fbcon_cur->tty_this;

	tty->cur_x = tty->cur_y = 0;

	tty_cursor(tty);

	tty_clear(tty);

}

static void devisn(struct vc *vc) {
	mpx_devisualized(vc);
}

static char fbcon_getc(struct fbcon *fbcon) {
	char ret;

	sched_lock();

	while (!fbcon->hasc) {
		event_wait_ms(&fbcon->inpevent, EVENT_TIMEOUT_INFINITE);
	}

	fbcon->hasc --;

	ret = fbcon->ascii[0];
	memcpy((void *) fbcon->ascii, (void *) &fbcon->ascii[1], 3);

	sched_unlock();

	return ret;
}

static struct fbcon *data2fbcon(struct idx_desc *data) {
	return (struct fbcon *) (data->data->fd_struct);
}

static int tty_read(struct idx_desc *data, void *buf, size_t nbyte) {
	struct fbcon *fbcon = data2fbcon(data);
	char *cbuf = (char *) buf;

	assert(fbcon);

	while (nbyte--) {
		*cbuf++ = fbcon_getc(fbcon);
	}

	return (int) cbuf - (int) buf;

}

static int tty_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	struct fbcon *fbcon = data2fbcon(data);
	char *cbuf = (char *) buf;

	while (nbyte--) {
		tty_putc(&fbcon->tty_this, *cbuf++);
	}

	return (int) cbuf - (int) buf;
}

static int tty_close(struct idx_desc *idx) {
	return 0;
}

static const struct task_idx_ops this_idx_ops = {
	.read = tty_read,
	.write = tty_write,
	.close = tty_close,
};

static void *run(void *data) {
	int fd = task_self_idx_alloc(&this_idx_ops, data);
	const struct shell *sh = shell_any();

	close(0);
	close(1);
	close(2);

	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);

	close(fd);

	shell_run(sh);


	return NULL;

}

static void video_tty_init(struct tty *t) { }

static void video_tty_cursor(struct tty *t, uint32_t x, uint32_t y) {
	struct fb_cursor cursor;
	struct video_tty_data *data;
	struct fb_info *fb;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	cursor.enable = 1;
	cursor.rop = ROP_XOR;
	cursor.image.width = data->font->width;
	cursor.image.height = data->font->height;
	cursor.image.fg_color = data->cur_color;

	cursor.hot.x = x;
	cursor.hot.y = y;

	fb = member_cast_out(t, struct fbcon, tty_this)->vc_this.fb;
	if (!fb) {
		return;
	}
	assert(fb->ops != NULL);
	assert(fb->ops->fb_cursor != NULL);
	fb->ops->fb_cursor(fb, &cursor);
}

static void video_tty_putc(struct tty *t, char ch, uint32_t x, uint32_t y) {
	struct fb_image symbol;
	struct video_tty_data *data;
	struct fb_info *fb;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	symbol.dx = x * data->font->width;
	symbol.dy = y * data->font->height;
	symbol.width = data->font->width;
	symbol.height = data->font->height;
	symbol.fg_color = data->fg_color;
	symbol.bg_color = data->bg_color;
	symbol.depth = 1;
	symbol.data = data->font->data + (unsigned char)ch * data->font->height * data->font->width / 8;

	fb = member_cast_out(t, struct fbcon, tty_this)->vc_this.fb;
	if (!fb) {
		return;
	}
	assert(fb->ops != NULL);
	assert(fb->ops->fb_imageblit != NULL);
	fb->ops->fb_imageblit(fb, &symbol);
}

static void video_tty_clear(struct tty *t, uint32_t x, uint32_t y,
		uint32_t width, uint32_t height) {
	struct fb_fillrect rect;
	struct video_tty_data *data;
	struct fb_info *fb;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	rect.dx = x * data->font->width;
	rect.dy = y * data->font->height;
	rect.width = width * data->font->width;
	rect.height = height * data->font->height;
	rect.color = data->bg_color;
	rect.rop = ROP_COPY;

	fb = member_cast_out(t, struct fbcon, tty_this)->vc_this.fb;
	if (!fb) {
		return;
	}
	assert(fb->ops != NULL);
	assert(fb->ops->fb_fillrect != NULL);
	fb->ops->fb_fillrect(fb, &rect);
}

static void video_tty_move(struct tty *t, uint32_t sx, uint32_t sy,
		uint32_t width, uint32_t height, uint32_t dx, uint32_t dy) {
	struct fb_copyarea area;
	struct video_tty_data *data;
	struct fb_info *fb;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	area.dx = dx * data->font->width;
	area.dy = dy * data->font->height;
	area.width = width * data->font->width;
	area.height = height * data->font->height;
	area.sx = sx * data->font->width;
	area.sy = sy * data->font->height;

	fb = member_cast_out(t, struct fbcon, tty_this)->vc_this.fb;
	if (!fb) {
		return;
	}
	assert(fb->ops != NULL);
	assert(fb->ops->fb_copyarea != NULL);
	fb->ops->fb_copyarea(fb, &area);
}

static const struct tty_ops video_tty_ops = {
	.init = &video_tty_init,
	.cursor = &video_tty_cursor,
	.putc = &video_tty_putc,
	.clear = &video_tty_clear,
	.move = &video_tty_move
};

static struct video_tty_data tty_data = {
	.font = &font_vga_8x16,
	.fg_color = 0x0000,
	.bg_color = 0xFFFF,
	.cur_color = 0x00F0,
};

static int make_task(int i, char innewtask) {
	struct fbcon *fbcon = &fbcons[i];
	int ret;

	fbcon->vc_this.callbacks = &thiscbs;

	tty_init(&fbcon->tty_this, 80, 24, &video_tty_ops, &tty_data);

	event_init(&fbcon->inpevent, "fbcon input event");

	if (0 > (ret = mpx_register_vc(&fbcon->vc_this))) {
		return ret;
	}

	if (innewtask) {
		return new_task("vc", run, &fbcons[i]);
	}

	run(fbcon);

	return 0;
}


static int fbcon_init(void) {

	make_task(0, true);
	make_task(1, false);

	return 0;
}
