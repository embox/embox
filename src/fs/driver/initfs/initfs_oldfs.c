/**
 * @file
 * @details Read-only filesystem with direct address space mapping.
 *
 * @date 29.06.09
 * @author Anton Bondarev
 *	        - initial implementation
 * @author Nikolay Korotky
 *	        - rework using vfs
 * @author Eldar Abusalimov
 *	        - rework mount to use cpio_parse_entry
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <cpio.h>
#include <stdarg.h>
#include <limits.h>

#include <fs/dir_context.h>
#include <fs/file_desc.h>
#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/vfs.h>
#include <fs/super_block.h>

#include <framework/mod/options.h>

#include "initfs.h"

struct inode_operations initfs_iops = {
	.ino_create = initfs_create,
	.ino_iterate = initfs_iterate,
};

static int initfs_mount(struct super_block *sb, struct inode *dest) {
#if 0
	extern char _initfs_start, _initfs_end;
	struct initfs_file_info *fi;

	if (&_initfs_start == &_initfs_end) {
		return -1;
	}

	fi = initfs_alloc_inode();
	if (fi == NULL) {
		return -ENOMEM;
	}

	memset(fi, 0, sizeof(*fi));
	inode_priv_set(dest, fi);

	dest->i_ops = &initfs_iops;
#endif

	return 0;
}

extern struct file_operations initfs_fops;
extern int initfs_fill_sb(struct super_block *sb, const char *source);

struct super_block_operations initfs_sbops = {
	//.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = initfs_destroy_inode,
};

static struct fsop_desc initfs_fsop = {
	.mount = initfs_mount,
};

static struct fs_driver initfs_driver = {
	.name = "initfs",
	.fill_sb   = initfs_fill_sb,
	.fsop = &initfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(initfs_driver);
