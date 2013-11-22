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

#include <drivers/diag.h>
#include <drivers/tty.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <kernel/task/idesc_table.h>
#include <fs/idesc.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(iodev_env_init);

static struct idesc_diag {
	struct idesc idesc;
	struct tty *tty;
} idesc_diag;

static int iodev_read(struct idesc *data, void *buf, size_t nbyte) {
	char *cbuf = (char *) buf;

	while (nbyte--) {
		*cbuf++ = diag_getc();
	}

	return (int) cbuf - (int) buf;

}

static int iodev_write(struct idesc *data, const void *buf, size_t nbyte) {
	char *cbuf = (char *) buf;

	while (nbyte--) {
		diag_putc(*cbuf++);
	}

	return (int) cbuf - (int) buf;
}

static int iodev_close(struct idesc *data) {
	return 0;
}

static int iodev_ioctl(struct idesc *desc, int request, void *data) {
	struct idesc_diag *idesc_diag = (struct idesc_diag *) desc;
	struct tty *tty;

	tty = idesc_diag->tty;

	if(NULL == tty) {
		return -EINVAL;
	}

	return 0;
}

static int iodev_fstat(struct idesc *data, void *buff) {
	struct stat *st = buff;

	st->st_mode = S_IFCHR;

	return 0;

}

static const struct task_idx_ops iodev_idx_ops = {
	.read = iodev_read,
	.write = iodev_write,
	.close = iodev_close,
	.ioctl = iodev_ioctl,
	.fstat = iodev_fstat,
};

static int iodev_env_init(void) {
	struct idesc_table *idesc_table;
	int fd;

	idesc_table = idesc_table_get_table(task_self()); //kernel task

	idesc_init(&idesc_diag.idesc, &iodev_idx_ops, FS_MAY_READ | FS_MAY_WRITE);
	fd = idesc_table_add(idesc_table, &idesc_diag.idesc, 0);
	if (fd < 0) {
		return fd;
	}

	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	if (fd > STDERR_FILENO) {
		close(fd);
	}

	return 0;
}
