/**
 * @file
 * @brief Creates /dev/null
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */
#include <errno.h>
#include <stdlib.h>

#include <util/err.h>

#include <drivers/char_dev.h>
#include <fs/dvfs.h>

#define NULL_DEV_NAME "null"

/* forward declaration */
static int null_init(void);
static struct idesc_ops idesc_cdev_null_ops;
static struct file_operations null_ops;

static struct idesc *null_open(struct inode *node, struct idesc *idesc) {
	struct file *file;

	file = dvfs_alloc_file();
	if (!file) {
		return err_ptr(ENOMEM);
	}
	*file = (struct file) {
		.f_idesc  = {
				.idesc_amode = FS_MAY_READ | FS_MAY_WRITE,
				.idesc_ops   = &idesc_cdev_null_ops,
		},
	};
	return &file->f_idesc;
}

static void null_close(struct idesc *desc) {
	dvfs_destroy_file((struct file *)desc);
}

static ssize_t null_write(struct idesc *desc, const void *buf, size_t size) {
	return size;
}

static ssize_t null_read(struct idesc *desc, void *buf, size_t size) {
	return 0;
}

static struct file_operations null_ops = {
		.open = null_open,
};

static int null_init(void) {
	return char_dev_register(NULL_DEV_NAME, &null_ops);
}

static struct idesc_ops idesc_cdev_null_ops = {
	.close = null_close,
	.read = null_read,
	.write = null_write,
};

CHAR_DEV_DEF(NULL_DEV_NAME, &null_ops, &idesc_cdev_null_ops, null_init);

