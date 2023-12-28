/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.05.2014
 */
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <embox/cmd.h>
#include <fs/dir_context.h>
#include <fs/dvfs.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/super_block.h>

#define BINFS_NAME "binfs"

int binfs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct inode *binfs_lookup(char const *name, struct inode const *dir) {
	struct super_block *sb;
	struct inode *node;
	const struct cmd *cmd;

	assert(dir);
	assert(dir->i_sb);

	sb = dir->i_sb;

	cmd_foreach(cmd) {
		if (strcmp(name, cmd->desc->name)) {
			continue;
		}

		if (NULL == (node = dvfs_alloc_inode(sb))) {
			return NULL;
		}

		return node;
	}

	return NULL;
}

static int binfs_iterate(struct inode *next, char *name, struct inode *parent,
    struct dir_ctx *ctx) {
	const struct cmd *cmd;
	int cnt;
	int i;

	cnt = 0;
	i = ((intptr_t)ctx->fs_ctx);

	cmd_foreach(cmd) {
		if (i == cnt) {
			ctx->fs_ctx = (void *)(intptr_t)i + 1;
			strncpy(name, cmd_name(cmd), NAME_MAX - 1);
			name[NAME_MAX - 1] = '\0';
			return 0;
		}
		cnt++;
	}
	/* End of directory */
	return -1;
}

static int binfs_ioctl(struct file_desc *desc, int request, void *data) {
	return 0;
}

struct file_operations binfs_fops = {
    /* .open  = binfs_open, */
    .ioctl = binfs_ioctl,
};

static struct inode_operations binfs_iops = {
    .lookup = binfs_lookup,
    .iterate = binfs_iterate,
};

static struct super_block_operations binfs_sbops = {
    .open_idesc = dvfs_file_open_idesc,
    .destroy_inode = binfs_destroy_inode,
};

static int binfs_fill_sb(struct super_block *sb, const char *source) {
	sb->sb_iops = &binfs_iops;
	sb->sb_fops = &binfs_fops;
	sb->sb_ops = &binfs_sbops;

	return 0;
}

static const struct fs_driver binfs_driver = {
    .name = "binfs",
    .fill_sb = binfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(binfs_driver);

FILE_SYSTEM_AUTOMOUNT("/bin", binfs_driver);
