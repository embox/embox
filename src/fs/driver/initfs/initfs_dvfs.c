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
 * @note   Initfs is based on CPIO archieve format. By design, this format
 *         has no directory absraction, as all files are stored with full
 *         path names. Beacause of this it could be tricky to handle some
 *         VFS calls.
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <cpio.h>
#include <stdarg.h>
#include <limits.h>

#include <embox/unit.h>
#include <fs/dvfs.h>
#include <mem/misc/pool.h>
#include <util/array.h>

#define INITFS_MAX_NAMELEN 32

/**
* @brief Should be used as inode->i_data, but only for directories
*/
struct initfs_dir_info {
	char  *path;
	size_t path_len;
	char  *name;
	size_t name_len;
};

POOL_DEF(initfs_dir_pool, struct initfs_dir_info, OPTION_GET(NUMBER,dir_quantity));

/**
* @brief Used to handle POSIX readdir
*/
struct initfs_ctx {
	char *prev;
	char *next;
};

POOL_DEF(initfs_ctx_pool, struct initfs_ctx, OPTION_GET(NUMBER,ctx_quantity));

static int initfs_open(struct inode *node, struct file *file) {
	return 0;
}

static size_t initfs_read(struct file *desc, void *buf, size_t size) {
	struct inode *inode;

	inode = desc->f_inode;

	if (size > inode->length - desc->pos) {
		size = inode->length - desc->pos;
	}

	memcpy(buf, (char *) inode->start_pos + desc->pos, size);

	return size;
}

static int initfs_ioctl(struct file *desc, int request, ...) {
	struct inode *inode = desc->f_inode;
	char **p_addr;
	va_list args;

	va_start(args, request);
	p_addr = va_arg(args, char **);
	va_end(args);

	*p_addr = (char*) inode->start_pos;

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
static int fill_inode_entry(struct inode *node, char *cpio, struct cpio_entry *entry) {
	*node = (struct inode) {
		.i_no      = (int) cpio,
		.start_pos = (int) entry->data,
		.length    = (size_t) entry->size,
	};
	return 0;
}

static struct inode *initfs_lookup(char const *name, struct dentry const *dir) {
	extern char _initfs_start;
	char *cpio = &_initfs_start;
	struct cpio_entry entry;
	struct inode *node;
	struct initfs_dir_info *di = dir->d_inode->i_data;

	while ((cpio = cpio_parse_entry(cpio, &entry)))
		if (!memcmp(di->path, entry.name, di->path_len) &&
		    !strncmp(name, entry.name + di->path_len, strlen(name))) {
			if (NULL == (node = dvfs_alloc_inode(dir->d_sb)))
				return NULL;

			if (fill_inode_entry(node, cpio, &entry)) {
				dvfs_destroy_inode(node);
				return NULL;
			}

			return node;
		}

	return NULL;
}

static int initfs_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	struct initfs_ctx *initfs_ctx = ctx->fs_ctx;
	struct cpio_entry entry;
	extern char _initfs_start;
	struct initfs_dir_info *di = parent->i_data;
	char last_name[INITFS_MAX_NAMELEN];
	char new_name[INITFS_MAX_NAMELEN];
	char *cpio;
	char *prev;

	if (!initfs_ctx) {
		/* Read first entry of directory */
		initfs_ctx = pool_alloc(&initfs_ctx_pool);

		if (!initfs_ctx)
			return -1;

		prev = cpio = &_initfs_start;
		while ((cpio = cpio_parse_entry(cpio, &entry))) {
			if (!memcmp(di->path, entry.name, di->path_len)) {
				fill_inode_entry(next, prev, &entry);
				break;
			}
			prev = cpio;
		}

		*initfs_ctx = (struct initfs_ctx) {
			.prev = prev,
			.next = cpio,
		};

		fill_inode_entry(next, &_initfs_start, &entry);
	} else {
		/* Get the name of last item */
		cpio = cpio_parse_entry(initfs_ctx->prev, &entry);
		memcpy(last_name, entry.name + di->path_len, entry.name_len - di->path_len);
		last_name[INITFS_MAX_NAMELEN - 1] = '\0';
		for (char *prev = last_name; prev < last_name + INITFS_MAX_NAMELEN; prev++)
			if (*prev == '/') {
				*prev = '\0';
				break;
			}

		initfs_ctx->prev = initfs_ctx->next;
		initfs_ctx->next = cpio;

		while ((cpio = cpio_parse_entry(cpio, &entry))) {
			if (memcmp(entry.name, di->path, di->path_len)) {
				/* This function can work faster, if you
				 * simply break the cycle here, but this hack
				 * requiers CPIO archieve not to be modified
				 * after creation */
				continue;
			}

			memcpy(new_name, entry.name + di->path_len, INITFS_MAX_NAMELEN);
			new_name[INITFS_MAX_NAMELEN - 1] = '\0';
			for (char *prev = new_name; prev < new_name + INITFS_MAX_NAMELEN; prev++)
				if (*prev == '/') {
					*prev = '\0';
					break;
				}

			if (strcmp(last_name, new_name)) {
				fill_inode_entry(next, initfs_ctx->next, &entry);
				initfs_ctx->prev = initfs_ctx->next;
				initfs_ctx->next = cpio;
				return 0;
			}

			initfs_ctx->next = cpio;
		}

		/* End of directory */
		pool_free(&initfs_ctx_pool, initfs_ctx);
		return -1;
	}

	return 0;
}

static int initfs_pathname(struct inode *inode, char *buf, int flags) {
	struct cpio_entry entry;
	char *c;

	if (NULL == cpio_parse_entry((char*) inode->i_no, &entry))
		return -1;

	switch (flags) {
	case DVFS_PATH_FS:
		memcpy(buf, entry.name, entry.name_len);
		buf[entry.name_len] = '\0';
		break;
	case DVFS_NAME:
		c = strrchr(entry.name, '/');
		memcpy(buf, c ? c : entry.name, entry.name_len - (c ? (c - entry.name) : 0));
		break;
	default:
		return -1;
	}

	return 0;
}

struct inode_operations initfs_iops = {
	.lookup   = initfs_lookup,
	.iterate  = initfs_iterate,
	.pathname = initfs_pathname,
};

struct file_operations initfs_fops = {
	.open  = initfs_open,
	.read  = initfs_read,
	.ioctl = initfs_ioctl,
};

static int initfs_fill_sb(struct super_block *sb, struct block_dev *dev) {
	sb->sb_iops = &initfs_iops;
	sb->sb_fops = &initfs_fops;

	return 0;
}

static struct dumb_fs_driver initfs_dumb_driver = {
	.name      = "initfs",
	.fill_sb   = initfs_fill_sb,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &initfs_dumb_driver);
