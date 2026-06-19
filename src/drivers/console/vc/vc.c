/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <drivers/console/vc/vc_vga.h>
#include <drivers/video_term.h>
#include <fs/file_desc.h>

static struct vterm vc_vterm;

static void vc_close(struct char_dev *cdev) {
	vc_vterm.tty.idesc = NULL;
}

static ssize_t vc_read(struct char_dev *cdev, void *buf, size_t nbyte, int flags) {
	return tty_read(&vc_vterm.tty, (char *)buf, nbyte);
}

static ssize_t vc_write(struct char_dev *cdev, const void *buf, size_t nbyte,
    int flags) {
	int i;
	char *b;

	b = (char *)buf;
	for (i = nbyte; i > 0; i--) {
		vterm_putc(&vc_vterm, *b++);
	}

	return (ssize_t)nbyte;
}

static int vc_ioctl(struct char_dev *cdev, int request, void *data) {
	return tty_ioctl(&vc_vterm.tty, request, data);
}

static int vc_status(struct char_dev *cdev, int mask) {
	return tty_status(&vc_vterm.tty, mask);
}

static int vc_open(struct char_dev *cdev, struct idesc *idesc) {
	struct vterm_video *vc_vga;

	vc_vga = vc_vga_init();

	vterm_init(&vc_vterm, vc_vga, NULL);

	vterm_open_indev(&vc_vterm, "keyboard");

	return 0;
}

static const struct char_dev_ops vc_dev_ops = {
    .read = vc_read,
    .write = vc_write,
    .ioctl = vc_ioctl,
    .open = vc_open,
    .close = vc_close,
    .status = vc_status,
};

static struct char_dev vc_dev = CHAR_DEV_INIT(vc_dev, "vc", &vc_dev_ops, 0);

CHAR_DEV_REGISTER(&vc_dev);
