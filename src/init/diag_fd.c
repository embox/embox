/**
 * @file    diag_fd.c
 * @brief   Initialization of terminal input/output
 * @author  Anton Kozlov
 * @date    09.06.2012
 */

#include <defines/size_t.h>
#include <fs/dvfs.h>
#include <termios.h>
#include <drivers/diag.h>
#include <kernel/task.h>
#include <kernel/task/resource/file_table.h>

static struct file diag_file;

static size_t diag_read(struct file *desc, void *buf, size_t size) {
	char *cbuf = (char *) buf;

	while (size--) {
		*cbuf++ = diag_getc();
	}

	return (void *) cbuf - buf;
}

static size_t diag_write(struct file *f, void *buf, size_t nbyte) {
	char *cbuf = (char *) buf;

	while (nbyte--) {
		diag_putc(*cbuf++);
	}

	return (void *) cbuf - buf;
}

static int diag_close(struct file *f) {
	return 0;
}

/*static int diag_ioctl(struct file *f, int request, void *data) {
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
} */

struct file_operations diag_file_ops = {
	.read = diag_read,
	.write = diag_write,
	.close = diag_close,
	//.ioctl = diag_ioctl,
};

int diag_fd(void) {
	struct file_table *ft = task_fs();

	if (!ft) {
		return -ENOSYS;
	}

	diag_file = (struct file) {
		.f_ops = &diag_file_ops,
	};

	return file_table_add(ft, &diag_file);
}
