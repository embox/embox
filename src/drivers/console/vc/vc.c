/**
 * @file
 * @brief
 *
 * @date 05.02.13
 * @author Ilia Vaprol
 * @author Anton Kozlov
 */
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/console/vc/vc_vga.h>
#include <drivers/video_term.h>
#include <drivers/char_dev.h> //XXX
#include <fs/idesc.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(vc_init);

static struct vterm vc_vterm;

static struct idesc *vc_open(struct node *node, struct file_desc *file_desc, int flags);

static struct kfile_operations vc_file_ops = {
	.open = vc_open,
};
static const struct idesc_ops idesc_vc_ops;

/*
 * file_operations
 */
static struct idesc *vc_open(struct node *node, struct file_desc *desc, int flags) {
	struct vterm_video *vc_vga;

	vc_vga = vc_vga_init();

	vterm_init(&vc_vterm, vc_vga, NULL);

	vterm_open_indev(&vc_vterm, "keyboard");

	assert(desc);

	idesc_init(&desc->idesc, &idesc_vc_ops, S_IROTH | S_IWOTH);
	vc_vterm.tty.idesc = &desc->idesc;

	return &desc->idesc;
}

static void vc_close(struct idesc *desc) {
	vc_vterm.tty.idesc = NULL;
}

static ssize_t vc_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	void *buf;
	size_t nbyte;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;

	return tty_read(&vc_vterm.tty, (char *) buf, nbyte);
}

static ssize_t vc_write(struct idesc *desc,  const struct iovec *iov, int cnt) {
	int i;
	char *b;

	assert(iov);
	assert(cnt == 1);

	b = (char *) iov->iov_base;
	for (i = iov->iov_len; i > 0; i--) {
		vterm_putc(&vc_vterm, *b++);
	}

	return (ssize_t)iov->iov_len;
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
		.id_readv = vc_read,
		.id_writev = vc_write,
		.ioctl = vc_ioctl,
		.close = vc_close,
		.status = vc_status,
		.fstat = vc_fstat,
};

static int vc_init(void) {
	return char_dev_register("vc", &vc_file_ops);
}

