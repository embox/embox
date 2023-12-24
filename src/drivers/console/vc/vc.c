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
#include <embox/unit.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <kernel/task/resource/idesc.h>
#include <mem/misc/pool.h>

#define VC_DEV_NAME  "vc"

#define VC_POOL_SIZE OPTION_GET(NUMBER, vc_quantity)

POOL_DEF(cdev_vc_pool, struct dev_module, VC_POOL_SIZE);

EMBOX_UNIT_INIT(vc_init);

static struct vterm vc_vterm;

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

	return tty_read(&vc_vterm.tty, (char *)buf, nbyte);
}

static ssize_t vc_write(struct idesc *desc, const struct iovec *iov, int cnt) {
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

static int vc_ioctl(struct idesc *desc, int request, void *data) {
	return tty_ioctl(&vc_vterm.tty, request, data);
}

static int vc_status(struct idesc *idesc, int mask) {
	return tty_status(&vc_vterm.tty, mask);
}

static struct idesc *vc_open(struct dev_module *mod, void *dev_priv) {
	struct vterm_video *vc_vga;

	vc_vga = vc_vga_init();

	vterm_init(&vc_vterm, vc_vga, NULL);

	vterm_open_indev(&vc_vterm, "keyboard");

	return char_dev_idesc_create(mod);
}

static int vc_init(void) {
	static const struct idesc_ops vc_idesc_ops;

	struct dev_module *vc_dev;

	vc_dev = pool_alloc(&cdev_vc_pool);
	if (!vc_dev) {
		return -ENOMEM;
	}

	memset(vc_dev, 0, sizeof(*vc_dev));
	strncpy(vc_dev->name, VC_DEV_NAME, sizeof(vc_dev->name));
	vc_dev->name[sizeof(vc_dev->name) - 1] = '\0';

	vc_dev->dev_iops = &vc_idesc_ops;
	vc_dev->dev_open = vc_open;

	return char_dev_register(vc_dev);
}

static const struct idesc_ops vc_idesc_ops = {
    .id_readv = vc_read,
    .id_writev = vc_write,
    .ioctl = vc_ioctl,
    .close = vc_close,
    .status = vc_status,
    .fstat = char_dev_idesc_fstat,
};
