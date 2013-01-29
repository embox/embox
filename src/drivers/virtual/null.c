/**
 * @file
 * @brief Creates file /dev/null
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */

#include <embox/device.h>
#include <fs/file_operation.h>
#include <stdlib.h>

#define NULL_DEV_NAME "null"

/* forward declaration */
static int null_init(void);
static struct kfile_operations null_ops;

EMBOX_DEVICE(NULL_DEV_NAME, &null_ops, null_init);

static int null_open(struct node *node, struct file_desc *file_desc, int flags) {
	return 0;
}

static int null_close(struct file_desc *desc) {
	return 0;
}

static size_t null_write(struct file_desc *desc, void *buf, size_t size) {
	return size;
}

static struct kfile_operations null_ops = {
		.open = null_open,
		.close = null_close,
		.write = null_write
};

#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>
static int null_init(void) {
	struct node *nod, *devnod;
	struct nas *dev_nas;

	/* register char device */
	nod = vfs_find_node("/dev", NULL);
	if (nod == NULL) {
		return -1;
	}

	devnod = vfs_add_path(NULL_DEV_NAME, nod);
	if (devnod == NULL) {
		return -1;
	}

	dev_nas = devnod->nas;
	dev_nas->fs = alloc_filesystem("empty");
	if (dev_nas->fs == NULL) {
		return -1;
	}

	dev_nas->fs->file_op = &null_ops;

	return 0;
}
