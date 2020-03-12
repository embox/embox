/**
 * @file
 * @details Read-only filesystem with direct address space mapping.
 *
 * @date 7 May 2015
 * @author Anton Bondarev
 *	        - initial implementation
 * @author Nikolay Korotky
 *	        - rework using vfs
 * @author Eldar Abusalimov
 *	        - rework mount to use cpio_parse_entry
 * @author Denis Deryugin
 *              - port from old VFS
 *
 * @note   Initfs is based on CPIO archive format. By design, this format
 *         has no directory abstraction, as all files are stored with full
 *         path names. Because of this it could be tricky to handle some
 *         VFS calls.
 */
#include <util/log.h>

#include <stdint.h>
#include <cpio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <framework/mod/options.h>

#include <fs/dvfs.h>
#include <mem/misc/pool.h>
#include <util/array.h>

#include "initfs.h"

static int initfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	return -EACCES;
}

static struct inode *initfs_lookup(char const *name, struct inode const *dir) {
	extern char _initfs_start;
	char *cpio = &_initfs_start;
	struct cpio_entry entry;
	struct inode *node = NULL;
	struct initfs_file_info *fi = inode_priv(dir);

	while ((cpio = cpio_parse_entry(cpio, &entry))) {
		if (!memcmp(fi->path, entry.name, fi->path_len) &&
		    !strncmp(name,
		             entry.name + fi->path_len + (*(entry.name + fi->path_len) == '/' ? 1 : 0),
		             strlen(name)) &&
			strrchr(entry.name + fi->path_len + 1, '/') == NULL) {

			if (!S_ISDIR(entry.mode) && !S_ISREG(entry.mode)) {
				log_error("Unknown inode type in cpio\n");
				break;
			}

			node = dvfs_alloc_inode(dir->i_sb);
			if (node == NULL) {
				break;
			}

			if (0 > initfs_fill_inode(node, cpio, &entry)) {
				dvfs_destroy_inode(node);
				return NULL;
			}

			return node;
		}
	}

	return NULL;
}

static int initfs_destroy_inode(struct inode *inode) {
	if (inode_priv(inode) != NULL) {
		initfs_file_free(inode_priv(inode));
	}

	return 0;
}

struct super_block_operations initfs_sbops = {
	.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = initfs_destroy_inode,
};

struct inode_operations initfs_iops = {
	.create   = initfs_create,
	.lookup   = initfs_lookup,
	.iterate  = initfs_iterate,
};

extern struct file_operations initfs_fops;

static int initfs_fill_sb(struct super_block *sb, const char *source) {
	struct initfs_file_info *fi;

	fi = initfs_file_alloc();
	if (fi == NULL) {
		return -ENOMEM;
	}

	sb->sb_iops = &initfs_iops;
	sb->sb_fops = &initfs_fops;
	sb->sb_ops  = &initfs_sbops;
	sb->bdev    = NULL;

	memset(fi, 0, sizeof(struct initfs_file_info));
	inode_priv_set(sb->sb_root, fi);

	return 0;
}

static const struct fs_driver initfs_dumb_driver = {
	.name      = "initfs",
	.fill_sb   = initfs_fill_sb,
};

ARRAY_SPREAD_DECLARE(const struct fs_driver *const, fs_drivers_registry);
ARRAY_SPREAD_ADD(fs_drivers_registry, &initfs_dumb_driver);
