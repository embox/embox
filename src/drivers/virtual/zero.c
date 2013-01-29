/**
 * @file
 * @brief Creates file /dev/zero
 *
 * @date 08.09.11
 * @author Anton Kozlov
 */

#include <embox/device.h>
#include <fs/file_operation.h>
#include <stdlib.h>
#include <string.h>

#define ZERO_DEV_NAME "zero"

/* forward declaration */
static int zero_init(void);
static struct kfile_operations zero_ops;

EMBOX_DEVICE(ZERO_DEV_NAME, &zero_ops, zero_init);

static int zero_open(struct node *node, struct file_desc *file_desc, int flags) {
	return 0;
}

static int zero_close(struct file_desc *desc) {
	return 0;
}

static size_t zero_read(struct file_desc *desc, void *buf, size_t size) {
	memset(buf, 0, size);
	return size;
}

static struct kfile_operations zero_ops = {
		.open = zero_open,
		.close = zero_close,
		.read = zero_read
};

#include <fs/file_desc.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/file_operation.h>
static int zero_init(void) {
	struct node *nod, *devnod;
	struct nas *dev_nas;

	/* register char device */
	nod = vfs_find_node("/dev", NULL);
	if (nod == NULL) {
		return -1;
	}

	devnod = vfs_add_path(ZERO_DEV_NAME, nod);
	if (devnod == NULL) {
		return -1;
	}

	dev_nas = devnod->nas;
	dev_nas->fs = alloc_filesystem("empty");
	if (dev_nas->fs == NULL) {
		return -1;
	}

	dev_nas->fs->file_op = &zero_ops;

	return 0;
}
