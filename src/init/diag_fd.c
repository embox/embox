/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.06.2012
 */

#include <termios.h>
#include <drivers/diag.h>
#include <kernel/task.h>
#include <kernel/task/idesc_table.h>

#include <fs/idesc.h>

static struct idesc diag_idesc;

static ssize_t diag_read(struct idesc *data, void *buf, size_t nbyte) {
	char *cbuf = (char *) buf;

	while (nbyte--) {
		*cbuf++ = diag_getc();
	}

	return (void *) cbuf - buf;
}

static ssize_t diag_write(struct idesc *data, const void *buf, size_t nbyte) {
	char *cbuf = (char *) buf;

	while (nbyte--) {
		diag_putc(*cbuf++);
	}

	return (void *) cbuf - buf;
}

static void diag_close(struct idesc *data) {
}

static int diag_fstat(struct idesc *data, void *buff) {
	struct stat *st = buff;

	st->st_mode = S_IFCHR;

	return 0;

}

static int diag_ioctl(struct idesc *desc, int request, void *data) {

	switch(request) {
	case TIOCGETA:
	case TIOCSETA:
	case TIOCSETAW:
	case TIOCSETAF:
	case TIOCGPGRP:
	case TIOCSPGRP:
		return 0;
	}

	return -ENOTSUP;
}

static const struct idesc_ops diag_idx_ops = {
	.read = diag_read,
	.write = diag_write,
	.close = diag_close,
	.fstat = diag_fstat,
	.ioctl = diag_ioctl,
};

int diag_fd(void) {
	struct idesc_table *idesc_table;

	idesc_table = task_resource_idesc_table(task_self());
	if (!idesc_table) {
		return -ENOSYS;
	}

	idesc_init(&diag_idesc, &diag_idx_ops, FS_MAY_READ | FS_MAY_WRITE);

	return idesc_table_add(idesc_table, &diag_idesc, 0);
}
