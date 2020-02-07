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
#include <fs/inode.h>
#include <fs/fs_driver.h>

#define BINFS_NAME "binfs"

static int binfs_mount(struct super_block *sb, struct inode *dest) {
	const struct cmd *cmd;

	cmd_foreach(cmd) {
		vfs_subtree_create_child(dest, cmd_name(cmd),
				S_IFREG | S_IXUSR | S_IXGRP | S_IXOTH);
	}

	return 0;
}

static int binfs_umount(struct inode *dir) {
	struct inode *child;

	while (NULL != (child = vfs_subtree_get_child_next(dir, NULL))) {
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
};

DECLARE_FILE_SYSTEM_DRIVER(binfs_driver);
