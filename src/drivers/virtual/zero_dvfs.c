/**
 * @file
 * @brief Handle /dev/zero. Ported from old vfs, originally by Anton Kozlov
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#include <stdlib.h>
#include <string.h>

#include <drivers/char_dev.h>
#include <fs/dvfs.h>

#define ZERO_DEV_NAME "zero"

/* forward declaration */
static int zero_init(void);
static struct file_operations zero_ops;

CHAR_DEV_DEF(ZERO_DEV_NAME, &zero_ops, zero_init);

static int zero_open(struct inode *node, struct file *file_desc) {
	return 0;
}

static int zero_close(struct file *desc) {
	return 0;
}

static size_t zero_read(struct file *desc, void *buf, size_t size) {
	memset(buf, 0, size);
	return size;
}

static size_t zero_write(struct file *desc, void *buf, size_t size) {
	return size;
}

static struct file_operations zero_ops = {
		.open = zero_open,
		.close = zero_close,
		.read = zero_read,
		.write = zero_write
};

static int zero_init(void) {
	return 0;
}
