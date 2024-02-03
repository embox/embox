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
#include <lib/libds/array.h>

#include "initfs.h"

static struct inode *initfs_lookup(char const *name, struct inode const *dir) {
	extern char _initfs_start;
	char *cpio = &_initfs_start;
	struct cpio_entry entry;
	struct inode *node = NULL;
	struct initfs_file_info *fi = inode_priv(dir);

	while ((cpio = cpio_parse_entry(cpio, &entry))) {
		if (fi->path && memcmp(fi->path, entry.name, fi->path_len)) {
			continue;
		}
		if (!strcmp(name,
		             entry.name + fi->path_len + (*(entry.name + fi->path_len) == '/' ? 1 : 0)) &&
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

struct super_block_operations initfs_sbops = {
	.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = initfs_destroy_inode,
};

struct inode_operations initfs_iops = {
	.ino_create   = initfs_create,
	.ino_lookup   = initfs_lookup,
	.ino_iterate  = initfs_iterate,
};

extern int initfs_fill_sb(struct super_block *sb, const char *source);

static const struct fs_driver initfs_dumb_driver = {
	.name      = "initfs",
	.fill_sb   = initfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(initfs_dumb_driver);
