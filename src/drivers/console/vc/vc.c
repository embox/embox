/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <drivers/console/vc_vga.h>
#include <drivers/video_term.h>
#include <embox/device.h> //XXX

#include <embox/unit.h>

EMBOX_UNIT_INIT(vc_init);

static struct vterm vc_vterm;

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
	struct vterm_video *vc_vga;

	vc_vga = vc_vga_init();

	vterm_init(&vc_vterm, vc_vga, NULL);

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
