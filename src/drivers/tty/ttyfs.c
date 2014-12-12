/**
 * @file
 *
 * @date Mar 6, 2013
 * @author: Anton Bondarev
 */

#include <fs/file_operation.h>
#include <fs/vfs.h>

static mount_tty(struct tty* tty, const struct kfile_operations *file_ops) {
	struct path node;
	/* register char device */
	node = vfs_lookup("/dev");
	if (!node) {
		return -1;
	}

	mode = S_IFCHR | S_IRALL | S_IWALL;

	node = vfs_create_child(node, name, mode);
	if (!node) {
		return -1;
	}

	nas = node->nas;
	if (NULL == (nas->fs = filesystem_create("empty"))) {
		return -1;
	}

	nas->fs->file_op = file_ops;
	nas->fi = dev;
}

int ttyfs_mount(void) {
	return 0;
}
