/**
 * @file
 * @brief Creates /dev/null
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */

#include <stdlib.h>

#include <drivers/char_dev.h>
#include <fs/dvfs.h>

#define NULL_DEV_NAME "null"

/* forward declaration */
static int null_init(void);
static struct file_operations null_ops;

CHAR_DEV_DEF(NULL_DEV_NAME, &null_ops, null_init);

static int null_open(struct inode *node, struct file *file_desc) {
	return 0;
}

static int null_close(struct file *desc) {
	return 0;
}

static size_t null_write(struct file *desc, void *buf, size_t size) {
	return size;
}

static struct file_operations null_ops = {
		.open = null_open,
		.close = null_close,
		.write = null_write
};

static int null_init(void) {
	return char_dev_register(NULL_DEV_NAME, &null_ops);
}
