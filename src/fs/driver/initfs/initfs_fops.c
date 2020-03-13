/**
 * @
 *
 * @date Dec 23, 2019
 * @author Anton Bondarev
 */
#include <cpio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>

#include <fs/dir_context.h>
#include <fs/file_operation.h>
#include <fs/file_desc.h>
#include <fs/inode.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#include "initfs.h"

POOL_DEF(initfs_file_pool, struct initfs_file_info, OPTION_GET(NUMBER,file_quantity));

static size_t initfs_read(struct file_desc *desc, void *buf, size_t size) {
	struct initfs_file_info *fi;
	off_t pos;

	pos = file_get_pos(desc);
	fi = file_get_inode_data(desc);

	if (pos + size > file_get_size(desc)) {
		size = file_get_size(desc) - pos;
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

static size_t initfs_write(struct file_desc *desc, void *buf, size_t size) {
	return -EACCES;
}

struct file_operations initfs_fops = {
	.read  = initfs_read,
	.write = initfs_write,
	.ioctl = initfs_ioctl,
};

struct initfs_file_info *initfs_file_alloc(void) {
	return pool_alloc(&initfs_file_pool);
}

void initfs_file_free(struct initfs_file_info *fi) {
	pool_free(&initfs_file_pool, fi);
}

/**
* @brief Initialize initfs inode
*
* @param node  Structure to be initialized
* @param entry Information about file in cpio archieve
*
* @return Negative error code
*/
int initfs_fill_inode(struct inode *node, char *cpio,
		struct cpio_entry *entry) {
	struct initfs_file_info *fi;

	inode_size_set(node, entry->size);
	inode_mtime_set(node, entry->mtime);
	node->i_mode = entry->mode & (S_IFMT | S_IRWXA);

	fi = initfs_file_alloc();
	if (!fi) {
		return -ENOMEM;
	}

	memset(fi, 0, sizeof(*fi));

	inode_priv_set(node, fi);
	fi->start_pos = (intptr_t) entry->data;
	fi->entry     = (void *) cpio;

	if (S_ISDIR(entry->mode)) {
		fi->path      = entry->name;
		fi->path_len  = strlen(entry->name);
	}

	return 0;
}

static int initfs_fillname(struct inode *inode, char *buf) {
	struct cpio_entry entry;
	struct initfs_file_info *fi = inode_priv(inode);
	size_t name_len = 0;
	char *name;

	if (NULL == cpio_parse_entry((char *) fi->entry, &entry)) {
		return -1;
	}

	/* CPIO entry contains full path, so we need to throw away
	 * everything before the last '/' symbol */
	name = strrchr(entry.name, '/');
	if (name) {
		name++;
	} else {
		name = entry.name;
	}

	name_len = entry.name_len - (name - entry.name);

	memcpy(buf, name, name_len);
	buf[name_len] = '\0';

	return 0;
}

int initfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx) {
	struct cpio_entry entry;
	extern char _initfs_start;
	struct initfs_file_info *di = inode_priv(parent);
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

			initfs_fillname(next, name);
			ctx->fs_ctx = cpio;

			return 0;
		}
	}

	/* End of directory */
	return -1;
}
