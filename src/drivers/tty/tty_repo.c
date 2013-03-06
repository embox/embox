/*
 * @file
 *
 * @date Mar 5, 2013
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <util/dlist.h>
#include <sys/stat.h>

#include <fs/file_operation.h>
#include <fs/vfs.h>


int tty_register(const char *name, void *dev, const struct kfile_operations *file_ops) {
	struct node *node;
	struct nas *nas;
	mode_t mode;

	/* register char device */
	node = vfs_lookup(NULL, "/dev");
	if (!node) {
		return -1;
	}

	mode = S_IFCHR | S_IRALL | S_IWALL;

	node = vfs_create_child(node, name, mode);
	if (!node) {
		return -1;
	}

	nas = node->nas;
	if (NULL == (nas->fs = filesystem_alloc("empty"))) {
		return -1;
	}

	nas->fs->file_op = file_ops;
	nas->fi = dev;

	return ENOERR;
}

int tty_unregister(const char *name) {
	return ENOERR;
}
