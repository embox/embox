/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.05.2014
 */
#include <errno.h>
#include <string.h>
#include <limits.h>

#include <embox/cmd.h>

//#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/fs_driver.h>
#include <fs/super_block.h>
#include <fs/dir_context.h>
#include <fs/file_desc.h>

#define BINFS_NAME "binfs"

extern struct super_block_operations binfs_sbops;

int binfs_destroy_inode(struct inode *inode) {
	return 0;
}

static struct inode *binfs_lookup(char const *name, struct inode const *dir) {
	return NULL;
}

static int binfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx) {
	const struct cmd *cmd;
	int i;
	int cnt = 0;

	i = ((intptr_t) ctx->fs_ctx);

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
//	.open  = binfs_open,
	.ioctl = binfs_ioctl,
};

static struct inode_operations binfs_iops = {
	.ino_lookup   = binfs_lookup,
	.ino_iterate  = binfs_iterate,
};

int binfs_fill_sb(struct super_block *sb, const char *source) {

	sb->sb_iops = &binfs_iops;
	sb->sb_fops = &binfs_fops;
	sb->sb_ops  = &binfs_sbops;

	return 0;
}
