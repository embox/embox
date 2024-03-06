/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.05.2014
 */

#include <string.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/super_block.h>

#include <embox/cmd.h>

#define BINFS_NAME "binfs"

struct inode *binfs_lookup(struct inode *node, char const *name, struct inode const *dir) {
	const struct cmd *cmd;

	assert(dir);
	assert(dir->i_sb);

	cmd_foreach(cmd) {
		if (strcmp(name, cmd->desc->name)) {
			continue;
		}

		return node;
	}

	return NULL;
}

extern int binfs_destroy_inode(struct inode *inode) ;

extern struct file_operations binfs_fops;

extern struct inode_operations binfs_iops;

extern int binfs_fill_sb(struct super_block *sb, const char *source);

extern struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);
struct super_block_operations binfs_sbops = {
    .open_idesc = dvfs_file_open_idesc,
    .destroy_inode = binfs_destroy_inode,
};

static const struct fs_driver binfs_driver = {
    .name = BINFS_NAME,
    .fill_sb = binfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(binfs_driver);

FILE_SYSTEM_AUTOMOUNT("/bin", binfs_driver);
