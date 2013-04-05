/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    11.03.2013
 */

#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <drivers/iodev.h>
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

static void inpevent(struct vc *vc, struct input_event *ev);
static void visd(struct vc *vc, struct fb_info *fbinfo);
static void devisn(struct vc *vc);

static void fbcon_vterm_clear_rows(struct vterm_video *t, short row, unsigned short count);

const struct vc_callbacks thiscbs = {
	.handle_input_event = inpevent,
	.visualized = visd,
	.schedule_devisualization = devisn,
};

static struct fbcon fbcons[VC_MPX_N];

static struct fbcon_displ_data fbcon_displ_data = {
	.font = &font_vga_8x16,
	.fg_color = 0x0000,
	.bg_color = 0xFFFF,
	.cur_color = 0x00F0,
};

static void inpevent(struct vc *vc, struct input_event *ev) {
	struct fbcon *fbcon = (struct fbcon *) vc;
	unsigned char ascii[4];
	int len;

	if (ev->devtype != INPUT_DEV_KBD) {
		return;
	}

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

static void vterm_reinit(struct vterm_video *t, int x, int y);

static void visd(struct vc *vc, struct fb_info *fbinfo) {
	struct fbcon *fbcon = (struct fbcon *) vc;

	fbcon->resbpp.x = fbinfo->var.xres;
	fbcon->resbpp.y = fbinfo->var.yres;
	fbcon->resbpp.bpp = fbinfo->var.bits_per_pixel;

	vterm_reinit(&fbcon->vterm_video, fbcon->resbpp.x / fbcon_displ_data.font->width,
		       	fbcon->resbpp.y / fbcon_displ_data.font->height);

	fbcon_vterm_clear_rows(&fbcon->vterm_video, 0, fbcon->vterm_video.height);

}

static void devisn(struct vc *vc) {
	mpx_devisualized(vc);
}

static char fbcon_getc(struct fbcon *fbcon) {
	char ret;

	sched_lock();

	while (!fbcon->hasc) {
		event_wait(&fbcon->inpevent, EVENT_TIMEOUT_INFINITE);
	}

	fbcon->hasc --;

	ret = fbcon->ascii[0];
	memcpy((void *) fbcon->ascii, (void *) &fbcon->ascii[1], 3);

	sched_unlock();

	return ret;
}

static inline struct fbcon *data2fbcon(struct idx_desc *data) {
	return (struct fbcon *) (data->data->fd_struct);
}

static int this_tty_read(struct idx_desc *data, void *buf, size_t nbyte) {
	struct fbcon *fbcon = data2fbcon(data);
	char *cbuf = (char *) buf;

	assert(fbcon);

	while (nbyte--) {
		*cbuf++ = fbcon_getc(fbcon);
	}

	return (int) cbuf - (int) buf;

}

static int this_tty_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	struct fbcon *fbcon = data2fbcon(data);
	char *cbuf = (char *) buf;

	while (nbyte--) {
		vterm_putc(&fbcon->vterm, *cbuf++);
	}

	return (int) cbuf - (int) buf;
}

static int this_tty_ioctl(struct idx_desc *desc, int request, void *data) {
	return tty_ioctl(&data2fbcon(data)->vterm.tty, request, data);
}

static int this_tty_close(struct idx_desc *idx) {
	return 0;
}

static const struct task_idx_ops this_idx_ops = {
	.read  = this_tty_read,
	.write = this_tty_write,
	.close = this_tty_close,
	.ioctl = this_tty_ioctl,
	.type  = TASK_RES_OPS_TTY,
};

static void *run(void *data) {
	int fd = task_self_idx_alloc(&this_idx_ops, data);
	const struct shell *sh = shell_lookup("tish");

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

static void fbcon_vterm_init(struct vterm_video *t) {
}

static int prev_x = -1, prev_y = -1;

static void fbcon_vterm_cursor(struct vterm_video *t, unsigned short x, unsigned short y) {
	struct fbcon *fbcon = member_cast_out(t, struct fbcon, vterm_video);
	struct fbcon_displ_data *data = fbcon->fbcon_disdata;
	struct fb_cursor cursor;
	struct fb_info *fb;

	assert(data != NULL);

	assert(data->font != NULL);

	cursor.enable = 1;
	cursor.rop = ROP_XOR;
	cursor.image.width = data->font->width;
	cursor.image.height = data->font->height;
	cursor.image.fg_color = data->cur_color;

	cursor.hot.x = prev_x;
	cursor.hot.y = prev_y;

	fb = fbcon->vc_this.fb;
	if (!fb) {
		return;
	}
	assert(fb->ops != NULL);
	assert(fb->ops->fb_cursor != NULL);

	if (prev_y >= 0) {
		fb->ops->fb_cursor(fb, &cursor);
	}

	prev_x = cursor.hot.x = x;
	prev_y = cursor.hot.y = y;

	fb->ops->fb_cursor(fb, &cursor);

}

static void fbcon_vterm_putc(struct vterm_video *t, char ch, unsigned short x, unsigned short y) {
	struct fbcon *fbcon = member_cast_out(t, struct fbcon, vterm_video);
	struct fb_image symbol;
	struct fbcon_displ_data *data = fbcon->fbcon_disdata;
	struct fb_info *fb;

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

	fb = fbcon->vc_this.fb;
	if (!fb) {
		return;
	}

	if (prev_x == x && prev_y == y) {
		prev_x = prev_y = -1;
	}

	assert(fb->ops != NULL);
	assert(fb->ops->fb_imageblit != NULL);
	fb->ops->fb_imageblit(fb, &symbol);
}


static void fbcon_vterm_clear_rows(struct vterm_video *t, short row, unsigned short count){
	struct fbcon *fbcon = member_cast_out(t, struct fbcon, vterm_video);
	struct fb_fillrect rect;
	struct fbcon_displ_data *data = fbcon->fbcon_disdata;
	struct fb_info *fb;

	assert(data != NULL);

	assert(data->font != NULL);

	rect.dx = 0;
	rect.dy = row * data->font->height;
	rect.width = t->width * data->font->width;
	rect.height = count * data->font->height;
	rect.color = data->bg_color;
	rect.rop = ROP_COPY;

	fb = fbcon->vc_this.fb;
	if (!fb) {
		return;
	}
	assert(fb->ops != NULL);
	assert(fb->ops->fb_fillrect != NULL);
	fb->ops->fb_fillrect(fb, &rect);

	if (prev_y >= row && prev_y < row + count) {
		prev_y = prev_x = -1;
	}
}

static void fbcon_vterm_copy_rows(struct vterm_video *t,
		unsigned short to, unsigned short from, short nrows) {
	struct fbcon *fbcon = member_cast_out(t, struct fbcon, vterm_video);
	struct fb_copyarea area;
	struct fbcon_displ_data *data = fbcon->fbcon_disdata;
	struct fb_info *fb;

	assert(data != NULL);

	assert(data->font != NULL);

	area.dx = 0;
	area.dy = to * data->font->height;
	area.width = t->width * data->font->width;
	area.height = nrows * data->font->height;
	area.sx = 0;
	area.sy = from * data->font->height;

	fb = fbcon->vc_this.fb;
	if (!fb) {
		return;
	}

	assert(fb->ops != NULL);
	assert(fb->ops->fb_copyarea != NULL);
	fb->ops->fb_copyarea(fb, &area);

	if (prev_y >= from && prev_y < from + nrows) {
		prev_y -= from - to;
	}
}

static const struct vterm_video_ops fbcon_vterm_video_ops = {
		.init = &fbcon_vterm_init,
		.cursor = &fbcon_vterm_cursor,
		.putc = &fbcon_vterm_putc,
		.clear_rows = &fbcon_vterm_clear_rows,
		.copy_rows = &fbcon_vterm_copy_rows
};

static void vterm_reinit(struct vterm_video *t, int x, int y) {
	t->width = x;
	t->height = y;
};

static int make_task(int i, char innewtask) {
	struct fbcon *fbcon = &fbcons[i];
	int ret;

	fbcon->vc_this.callbacks = &thiscbs;
	fbcon->fbcon_disdata = &fbcon_displ_data;

	fbcon->vterm_video.ops = &fbcon_vterm_video_ops;

	vterm_init(&fbcon->vterm, &fbcon->vterm_video, NULL);

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

static int iodev_stdio_init(void) {
	return 0;
}

static void iodev_stdio_putc(char ch) {
	vterm_putc(&fbcons[0].vterm, ch);
}

const struct iodev_ops iodev_stdio_ops = {
	.init = iodev_stdio_init,
	.getc = NULL,//&diag_getc,
	.putc = iodev_stdio_putc,
	.kbhit = NULL //&diag_kbhit,
};


static int fbcon_init(void) {

	make_task(0, true);
	make_task(1, true);

	iodev_setup(&iodev_stdio_ops);

	return 0;
}
