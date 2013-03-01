/**
 * @file
 * @brief Creates diag file in /dev/
 *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <fs/file_operation.h>
#include <drivers/diag.h>
#include <embox/device.h>
#include <errno.h>

#include <fcntl.h>

static int nonblocking = 0;

static void *diag_open(struct file_desc *desc, const char *mode);
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
static const char mode;

FILE *diag_device_get(void) {
	return diag_open(&diag_desc, &mode);
}

/*
 * file_operation
 */
static void *diag_open(struct file_desc *desc, const char *mode) {
	desc->ops = &file_op;
	return (void *) desc;
}

static int diag_close(struct file_desc *desc) {
	return 0;
}

/**
 * This used for save current new ch to buff
 * and it's replace sequences \r\n, \r, \n to \n on the fly
 * @return non-zero if symbol was saved, zero otherwise
 */
static int save_to_buf(char *save_to, char ch) {
	static int last_is_cr = 0;

	if (last_is_cr && (ch == '\n')) {
		last_is_cr = 0;
		return 0; /* resolve \r\n case */
	}

	last_is_cr = (ch == '\r');
	ch = ((ch == '\r') ? '\n' : ch); /* resolve \r case */

	/* Save to buffer */
	*save_to = ch;

	return 1;
}

static size_t diag_read(void *buf, size_t size, size_t count, void *file) {
	char *ch_buf = (char *) buf;
	int n = count * size;

	if (nonblocking) {
		for (int i = 0; i < n; ++i) {
			if (!diag_kbhit()) {
				if (0 == i) {
					return -EAGAIN;
				}
				return i;
			}
			if (save_to_buf(ch_buf, diag_getc())) {
				ch_buf++;
			}
		}
	} else {
		int i = n;
		while (i--) {
			if (save_to_buf(ch_buf, diag_getc())) {
				ch_buf++;
			}
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
	case O_NONBLOCK:
		nonblocking = va_arg(args, int);
	default:
		/* SET_ERRNO(EINVAL) */
		return -1; /* no such request value */
	}

	return 0;
}

/* doesn't matter if we have fs:
if have, dependency added automaticly;
else this have no effect*/
EMBOX_DEVICE("diag", &file_op);
