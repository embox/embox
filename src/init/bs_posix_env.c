/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.06.2012
 */

#include <fcntl.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include <drivers/iodev.h>
#include <drivers/tty.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(iodev_env_init);
#if 0
static int iodev_read(struct idx_desc *data, void *buf, size_t nbyte) {
	char *cbuf = (char *) buf;

	while (nbyte--) {
		*cbuf++ = iodev_getc();
	}

	return (int) cbuf - (int) buf;

}
#endif

static int iodev_read(struct idx_desc *desc, void *buf, size_t nbyte) {
	struct tty *tty = desc->data->fd_struct;

	return tty_read(tty, buf, nbyte);
}

static int iodev_write(struct idx_desc *data, const void *buf, size_t nbyte) {
	char *cbuf = (char *) buf;

	while (nbyte--) {
		iodev_putc(*cbuf++);
	}

	return (int) cbuf - (int) buf;
}

static int iodev_close(struct idx_desc *idx) {
	return 0;
}

static int iodev_ioctl(struct idx_desc *desc, int request, void *data) {
	struct tty *tty = desc->data->fd_struct;

	if(NULL == tty) {
		return -EINVAL;
	}

	return tty_ioctl(tty, request, data);
}

static const struct task_idx_ops iodev_idx_ops = {
	.read = iodev_read,
	.write = iodev_write,
	.close = iodev_close,
	.ioctl = iodev_ioctl,
	.type = TASK_RES_OPS_TTY
};

static int check_valid(int fd, int reference_fd) {
	if (fd == reference_fd) {
		return 0;
	}

	for (int i = 0; i < reference_fd; i++) {
		close(i);
	}

	if (fd < 0) {
		return fd;
	}

	close(fd);
	return -1;
}

extern struct tty *diag_tty;

static int iodev_env_init(void) {
	int fd;
	int res = 0;


	fd = task_self_idx_alloc(&iodev_idx_ops, diag_tty);
	if ((res = check_valid(fd, 0)) != 0) {
		return res;
	}

	for (int i = 1; i <= 2; i++) {
		fd = dup(0);
		if ((res = check_valid(fd, i)) != 0) {
			return res;
		}
	}

	return 0;
}
