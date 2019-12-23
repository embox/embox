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

#define INITFS_MAX_NAMELEN 32

struct initfs_file_info {
	int start_pos;

	size_t        size;
	unsigned int  ctime; /* time of last status change */
	unsigned int  mtime;
};

/**
* @brief Should be used as inode->i_data, but only for directories
*/
struct initfs_dir_info {
	int    start_pos;

	char  *path;
	size_t path_len;
	char  *name;
	size_t name_len;
};

POOL_DEF (initfs_file_pool, struct initfs_file_info, OPTION_GET(NUMBER,file_quantity));
POOL_DEF(initfs_dir_pool, struct initfs_dir_info, OPTION_GET(NUMBER,dir_quantity));

static size_t initfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct initfs_file_info *fi;
	off_t pos;

	pos = file_get_pos(desc);
	fi = file_get_inode_data(desc);

	if (pos + size > file_get_size(desc)) {
		size = file_get_size(desc) - desc->pos;
	}

	memcpy(buf, (char *) (uintptr_t) (fi->start_pos + pos), size);

	return size;
}

static int initfs_ioctl(struct file_desc *desc, int request, void *data) {
	char **p_addr;
	struct initfs_file_info *fi;

	fi = file_get_inode_data(desc);

	p_addr = data;

	*p_addr = (char*) (uintptr_t) fi->start_pos;

	return 0;
}

/**
* @brief Initialize initfs inode
*
* @param node  Structure to be initialized
* @param entry Information about file in cpio archieve
*
* @return Negative error code
*/
static int initfs_fill_inode(struct inode *node, char *cpio,
		struct cpio_entry *entry) {
	struct initfs_file_info *fi;
	struct initfs_dir_info *di;

	node->i_no      = (intptr_t) cpio;
	node->length    = (size_t) entry->size;
	node->i_mode    = entry->mode & (S_IFMT | S_IRWXA);

	if (S_ISDIR(entry->mode)) {
		di = pool_alloc(&initfs_dir_pool);
		if (NULL == di) {
			return -ENOMEM;
		}
		di->path = entry->name;
		di->path_len = strlen(entry->name);
		di->start_pos = (intptr_t)entry->data;

		node->i_data = di;

		return 0;
	}

	fi = pool_alloc(&initfs_file_pool);
	if (!fi) {
		return -ENOMEM;
	}

	fi->start_pos = (intptr_t)entry->data;

	node->i_data    = fi;

	return 0;
}

static struct inode *initfs_lookup(char const *name, struct dentry const *dir) {
	extern char _initfs_start;
	char *cpio = &_initfs_start;
	struct cpio_entry entry;
	struct inode *node = NULL;
	struct initfs_dir_info *di = dir->d_inode->i_data;

	while ((cpio = cpio_parse_entry(cpio, &entry))) {
		if (!memcmp(di->path, entry.name, di->path_len) &&
		    !strncmp(name,
		             entry.name + di->path_len + (*(entry.name + di->path_len) == '/' ? 1 : 0),
		             strlen(name)) &&
			strrchr(entry.name + di->path_len + 1, '/') == NULL) {

			if (!S_ISDIR(entry.mode) && !S_ISREG(entry.mode)) {
				log_error("Unknown inode type in cpio\n");
				break;
			}

			node = dvfs_alloc_inode(dir->d_sb);
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

static int initfs_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	struct cpio_entry entry;
	extern char _initfs_start;
	struct initfs_dir_info *di = parent->i_data;
	char *cpio = ctx->fs_ctx;
	char *prev;

	assert(di);

	if (!cpio) {
		cpio = &_initfs_start;
	}

	while ((prev = cpio, cpio = cpio_parse_entry(cpio, &entry))) {
		if (!memcmp(di->path, entry.name, di->path_len) &&
			entry.name[di->path_len] != '\0' &&
			strrchr(entry.name + di->path_len + 1, '/') == NULL) {

			if (!S_ISDIR(entry.mode) && !S_ISREG(entry.mode)) {
				log_error("Unknown inode type in cpio\n");
				break;
			}

			if (0 > initfs_fill_inode(next, prev, &entry)) {
				return -1;
			}

			ctx->fs_ctx = cpio;

			return 0;
		}
	}

	/* End of directory */
	return -1;
}

static int initfs_pathname(struct inode *inode, char *buf, int flags) {
	struct cpio_entry entry;
	char *c;

	if (NULL == cpio_parse_entry((char *) (uintptr_t) inode->i_no, &entry))
		return -1;

	switch (flags) {
	case DVFS_PATH_FS:
		memcpy(buf, entry.name, entry.name_len);
		buf[entry.name_len] = '\0';
		break;
	case DVFS_NAME:
		c = strrchr(entry.name, '/');
		if (c)
			c++;
		memcpy(buf, c ? c : entry.name, entry.name_len - (c ? (c - entry.name) : 0));
		buf[entry.name_len - (c ? (c - entry.name) : 0)] = '\0';
		break;
	default:
		return -1;
	}

	return 0;
}

static int initfs_mount_end(struct super_block *sb) {
	struct initfs_dir_info *di;

	di = pool_alloc(&initfs_dir_pool);
	assert(di);

	memset(di, 0, sizeof(struct initfs_dir_info));
	sb->root->d_inode->i_data = di;

	return 0;
}

static int initfs_destroy_inode(struct inode *inode) {
	if (!inode->i_data) {
		return 0;
	}

	if (S_ISDIR(inode->i_mode)) {
		pool_free(&initfs_dir_pool, inode->i_data);
	} else {
		pool_free(&initfs_file_pool, inode->i_data);
	}
	return 0;
}

struct super_block_operations initfs_sbops = {
	.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = initfs_destroy_inode,
};

struct inode_operations initfs_iops = {
	.lookup   = initfs_lookup,
	.iterate  = initfs_iterate,
	.pathname = initfs_pathname,
};

struct file_operations initfs_fops = {
	.read  = initfs_read,
	.ioctl = initfs_ioctl,
};

static int initfs_fill_sb(struct super_block *sb, struct file_desc *bdev_file) {
	sb->sb_iops = &initfs_iops;
	sb->sb_fops = &initfs_fops;
	sb->sb_ops  = &initfs_sbops;
	sb->bdev = NULL;

	if (bdev_file) {
		return -1;
	}

	return 0;
}

static const struct dumb_fs_driver initfs_dumb_driver = {
	.name      = "initfs",
	.fill_sb   = initfs_fill_sb,
	.mount_end = initfs_mount_end,
};

ARRAY_SPREAD_DECLARE(const struct dumb_fs_driver *const, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &initfs_dumb_driver);
