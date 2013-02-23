/**
 * @file
 *
 * @brief File System Driver for virtual file system
 *
 * @date Nov 28, 2012
 * @author: Anton Bondarev
 */

#include <fs/fs_driver.h>


static int pseudofs_create(struct node *parent_node, struct node *new_node) {
	return 0;
}

static int pseudofs_delete(struct node *nod) {
	return 0;
}

static const struct fsop_desc pseudofs_fsop = {
	.create_node = pseudofs_create,
	.delete_node = pseudofs_delete,
};

static const struct fs_driver pseudofs_driver = {
	.name = "pseudofs",
	.file_op = NULL,
	.fsop = &pseudofs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(pseudofs_driver);
