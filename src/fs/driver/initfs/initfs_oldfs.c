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

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

#include <mem/misc/pool.h>
#include <kernel/printk.h>
#include <util/array.h>

#include "initfs.h"

#include <framework/mod/options.h>

struct initfs_file_info_tmp {
	struct node_info ni; /* must be the first member */
	struct initfs_file_info *priv;
	struct initfs_file_info initfs_file_info;
};
POOL_DEF(file_pool, struct initfs_file_info_tmp, OPTION_GET(NUMBER,file_quantity));

static int initfs_mount(struct super_block *sb, struct inode *dest) {
	extern char _initfs_start, _initfs_end;
	char *cpio = &_initfs_start;
	struct nas *dir_nas;
	struct inode *node;
	struct initfs_file_info_tmp *fi;
	struct cpio_entry entry;
	char name[PATH_MAX + 1];

	dir_nas = dest->nas;

	if (&_initfs_start == &_initfs_end) {
		return -1;
	}
	printk("%s: unpack initinitfs at %p into %s\n", __func__,
			cpio, dest->name);

	while ((cpio = cpio_parse_entry(cpio, &entry))) {
		if (entry.name_len > PATH_MAX) {
			return -ENAMETOOLONG;
		}
		memcpy(name, entry.name, entry.name_len);
		name[entry.name_len] = '\0';

		node = vfs_subtree_create_intermediate(dest, name, entry.mode);
		if (NULL == node) {
			return -ENOMEM;
		}

		fi = pool_alloc(&file_pool);
		if (!fi) {
			return -ENOMEM;
		}
		fi->priv = &fi->initfs_file_info;
		fi->initfs_file_info.start_pos = (intptr_t)entry.data;
		fi->ni.size = entry.size;
		fi->ni.mtime = entry.mtime;

		node->nas->fi = (struct node_fi *) fi;
		node->nas->fs = dir_nas->fs;
	}

	return 0;
}

extern struct file_operations initfs_fops;

static struct fsop_desc initfs_fsop = {
	.mount = initfs_mount,
};

static struct fs_driver initfs_driver = {
	.name = "initfs",
	.file_op = &initfs_fops,
	.fsop = &initfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(initfs_driver);
