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

static void vc_close(struct idesc *idesc) {
	vc_vterm.tty.idesc = NULL;
}

static ssize_t vc_read(struct idesc *idesc, const struct iovec *iov, int cnt) {
	void *buf;
	size_t nbyte;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;

	return tty_read(&vc_vterm.tty, (char *)buf, nbyte);
}

static ssize_t vc_write(struct idesc *idesc, const struct iovec *iov, int cnt) {
	int i;
	char *b;

	assert(iov);
	assert(cnt == 1);

	b = (char *)iov->iov_base;
	for (i = iov->iov_len; i > 0; i--) {
		vterm_putc(&vc_vterm, *b++);
	}

	return (ssize_t)iov->iov_len;
}

static int vc_ioctl(struct idesc *idesc, int request, void *data) {
	return tty_ioctl(&vc_vterm.tty, request, data);
}

static int vc_status(struct idesc *idesc, int mask) {
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

static struct char_dev vc_dev = CHAR_DEV_INIT(vc_dev, "vc", &vc_dev_ops);

CHAR_DEV_REGISTER(&vc_dev);
