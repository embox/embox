/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */

#include <drivers/console/vc/vc_vga.h>
#include <drivers/video_term.h>
#include <embox/device.h> //XXX
#include <fs/idesc.h>
#include <fs/file_desc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(vc_init);

static struct vterm vc_vterm;

static int vc_open(struct node *node, struct file_desc *file_desc,
	int flags);

static struct kfile_operations vc_file_ops = {
	.open = vc_open,
};
static const struct idesc_ops idesc_vc_ops;

/*
 * file_operations
 */
static int vc_open(struct node *node, struct file_desc *desc, int flags) {
	struct vterm_video *vc_vga;

	vc_vga = vc_vga_init();

	vterm_init(&vc_vterm, vc_vga, NULL);

	vterm_open_indev(&vc_vterm, "keyboard");

	assert(desc);

	idesc_init(&desc->idesc, &idesc_vc_ops, FS_MAY_READ | FS_MAY_WRITE);
	vc_vterm.tty.idesc = &desc->idesc;

	return 0;

}

static void vc_close(struct idesc *desc) {
	vc_vterm.tty.idesc = NULL;
}

static ssize_t vc_read(struct idesc *desc, void *buff, size_t size) {

	return tty_read(&vc_vterm.tty, (char *) buff, size);
}

static ssize_t vc_write(struct idesc *desc, const void *buff, size_t size) {
	size_t cnt;
	char *b;

	for (cnt = size, b = (char *) buff; cnt > 0; b++, cnt--) {
		vterm_putc(&vc_vterm, *b);
	}

	return (ssize_t)size;
}

static int vc_ioctl(struct idesc *desc, int request, void *data) {

	return tty_ioctl(&vc_vterm.tty, request, data);
}

static int vc_fstat(struct idesc *data, void *buff) {
	struct stat *st = buff;

	st->st_mode = S_IFCHR;

	return 0;

}

static int vc_status(struct idesc *idesc, int mask) {

	return tty_status(&vc_vterm.tty, mask);
}

static const struct idesc_ops idesc_vc_ops = {
		.read = vc_read,
		.write = vc_write,
		.ioctl = vc_ioctl,
		.close = vc_close,
		.status = vc_status,
		.fstat = vc_fstat,
};

static int vc_init(void) {
	return char_dev_register("vc", &vc_file_ops);
}

