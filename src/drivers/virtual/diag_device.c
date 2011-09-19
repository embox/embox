/**
 * @file
 * @brief Creates diag file in /dev/
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <kernel/file.h>
#include <fs/node.h>
#include <kernel/diag.h>
#include <embox/device.h>
#include <diag/diag_device.h>
#include <errno.h>

static int nonblocking = 0;

static void *diag_open(struct file_desc *desc);
static int diag_close(struct file_desc *desc);
static size_t diag_read(void *buf, size_t size, size_t count, void *file);
static size_t diag_write(const void *buff, size_t size, size_t count, void *file);
static int diag_ctl(void *file, int request, va_list args);

static file_operations_t file_op = {
		.fread = diag_read,
		.fopen = diag_open,
		.fclose = diag_close,
		.fwrite = diag_write,
		.ioctl = diag_ctl

};

static struct file_desc diag_desc;

FILE *diag_device_get(void) {
	return diag_open(&diag_desc);
}

/*
 * file_operation
 */
static void *diag_open(struct file_desc *desc) {
	desc->ops = &file_op;
	return (void *) desc;
}

static int diag_close(struct file_desc *desc) {
	return 0;
}

static size_t diag_read(void *buf, size_t size, size_t count, void *file) {
	char *ch_buf = (char *) buf;
	int n = count * size;

	if (nonblocking) {
		int i = 0;
		while (i < n) {
			if (!diag_has_symbol()) {
				if (0 == i) {
					return -EAGAIN;
				}
				return i;
			}
			*(ch_buf++) = diag_getc();
			i++;
		}
	} else {
		int i = n;
		while (i --) {
			*(ch_buf++) = diag_getc();
		}
	}

	return n;
}

static size_t diag_write(const void *buff, size_t size, size_t count, void *file) {
	size_t cnt = 0;
	char *b = (char*) buff;

	while (cnt != count * size) {
		diag_putc(b[cnt++]);
	}
	return cnt;
}

static int diag_ctl(void *file, int request, va_list args) {
	switch (request) {
	case O_NONBLOCK_SET:
		nonblocking = 1;
		break;
	case O_NONBLOCK_CLEAR:
		nonblocking = 0;
		break;
	default:
		break;
	}
	return 0;
}

/* doesn't matter if we have fs:
if have, dependency added automaticly;
else this have no effect*/
EMBOX_DEVICE("diag", &file_op);
