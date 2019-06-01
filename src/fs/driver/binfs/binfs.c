/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.05.2014
 */
#include <errno.h>

#include <embox/cmd.h>
#include <fs/vfs.h>
#include <fs/fs_driver.h>

#define BINFS_NAME "binfs"

static int binfs_mount(void *dev, void *dir) {
	struct node *dir_node = dir;
	const struct cmd *cmd;

	dir_node = dir;

	cmd_foreach(cmd) {
		vfs_subtree_create_child(dir_node, cmd_name(cmd),
				S_IFREG | S_IXUSR | S_IXGRP | S_IXOTH);
	}

	if (NULL == (dir_node->nas->fs = filesystem_create(BINFS_NAME))) {
		return -ENOMEM;
	}
	return 0;
}

static int binfs_umount(void *dir) {
	struct node *dir_node = dir;
	struct node *child;

	while (NULL != (child = vfs_subtree_get_child_next(dir_node, NULL))) {
		vfs_del_leaf(child);
	}

	return 0;
}

static struct fsop_desc binfs_fsop = {
	.mount = binfs_mount,
	.umount = binfs_umount,
};

static struct fs_driver binfs_driver = {
	.name = BINFS_NAME,
	.fsop = &binfs_fsop,
	.mount_dev_by_string = true,
};

DECLARE_FILE_SYSTEM_DRIVER(binfs_driver);
