/**
 * @
 *
 * @date Dec 23, 2019
 * @author Anton Bondarev
 */

#include <util/log.h>

#include <cpio.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include <fs/dir_context.h>

#include <fs/inode.h>

#include "initfs.h"


extern int initfs_alloc_inode_priv(struct inode *node);
extern int initfs_fillname(struct inode *inode, char *buf);

struct inode *initfs_lookup(struct inode *node, char const *name, struct inode const *dir) {
	extern char _initfs_start;
	char *cpio = &_initfs_start;
	struct cpio_entry entry;
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

			if (0 > initfs_alloc_inode_priv(node)) {
				return NULL;
			}

			if (0 > initfs_fill_inode(node, cpio, &entry)) {
				initfs_destroy_inode(node);
				return NULL;
			}

			return node;
		}
	}

	return NULL;
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
		if (di->path && memcmp(di->path, entry.name, di->path_len)) {
			continue;
		}
		if (entry.name[di->path_len] != '\0' &&
			strrchr(entry.name + di->path_len + 1, '/') == NULL) {

			if (!S_ISDIR(entry.mode) && !S_ISREG(entry.mode)) {
				log_error("Unknown inode type in cpio\n");
				break;
			}

			if (0 > initfs_alloc_inode_priv(next)) {
				return -1;
			}

			if (0 > initfs_fill_inode(next, prev, &entry)) {
				initfs_destroy_inode(next);
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

