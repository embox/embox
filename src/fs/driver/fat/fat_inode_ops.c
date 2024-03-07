/**
 * @file
 *
 * @date Dec 12, 2019
 * @author Anton Bondarev
 */
#include <stddef.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/inode_operation.h>
#include <fs/dir_context.h>

#include "fat.h"


/* @brief Get next inode in directory
 * @param inode   Structure to be filled
 * @param parent  Inode of parent directory
 * @param dir_ctx Directory context
 *
 * @return Error code
 */
int fat_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx) {
	struct dirinfo *dirinfo;
	struct fat_dirent de;
	int res;

	assert(parent->i_sb);

	dirinfo = inode_priv(parent);
	dirinfo->currententry = (uintptr_t) ctx->fs_ctx;

	if (dirinfo->currententry == 0) {
		/* Need to get directory data from drive */
		fat_reset_dir(dirinfo);
	}

	read_dir_buf(dirinfo);

	while (((res = fat_get_next_long(dirinfo, &de, NULL)) ==  DFS_OK) || res == DFS_ALLOCNEW) {
		if (de.attr & ATTR_VOLUME_ID) {
			continue;
		}

		if (!memcmp(de.name, MSDOS_DOT, strlen(MSDOS_DOT)) ||
			!memcmp(de.name, MSDOS_DOTDOT, strlen(MSDOS_DOT))) {
			continue;
		}

		break;
	}

	switch (res) {
	case DFS_OK: {
		char tmp_name[128];

		if (0 > fat_fill_inode(next, &de, dirinfo)) {
			return -1;
		}
		if (DFS_OK != fat_read_filename(inode_priv(next), fat_sector_buff, tmp_name)) {
			return -1;
		}
		strncpy(name, tmp_name, NAME_MAX-1);
		name[NAME_MAX - 1] = '\0';

		ctx->fs_ctx = (void *) ((uintptr_t) dirinfo->currententry);
		return 0;
	}
	case DFS_EOF:
		/* Fall through */
	default:
		return -1;
	}
}

int fat_truncate(struct inode *node, off_t length) {
	assert(node);

	inode_size_set(node, length);

	/* TODO realloc blocks*/

	return 0;
}

/* @brief Create new file or directory
 * @param i_new Inode to be filled
 * @param i_dir Inode realted to the parent
 * @param mode  Used to figure out file type
 *
 * @return Negative error code
 */
int fat_create(struct inode *i_new, struct inode *i_dir, int mode) {
	struct fat_file_info *fi;
	struct fat_fs_info *fsi;
	struct dirinfo *di;
	char *name;

	assert(i_dir && i_new);

	inode_size_set(i_new, 0);

	di = inode_priv(i_dir);

	name = inode_name(i_new);
	assert(name);

	/* TODO check file exists */
	assert(i_dir->i_sb);
	fsi = i_dir->i_sb->sb_data;

	fat_reset_dir(di);
	read_dir_buf(di);

	if (S_ISDIR(i_new->i_mode)) {
		struct dirinfo *new_di;
		new_di = fat_dirinfo_alloc();
		if (!new_di) {
			return -ENOMEM;
		}
		new_di->p_scratch = fat_sector_buff;
		fi = &new_di->fi;
	} else {
		fi = fat_file_alloc();
		if (!fi) {
			return -ENOMEM;
		}
	}

	inode_priv_set(i_new, fi);

	fi->volinfo = &fsi->vi;
	fi->fdi     = di;
	fi->fsi     = fsi;
	fi->mode   |= i_new->i_mode;

	if (0 != fat_create_file(fi, di, name, fi->mode)) {
		return -EIO;
	}

	return 0;
}
extern int fat_dir_empty(struct fat_file_info *fi);
int fat_delete(struct inode *dir, struct inode *node) {
	struct fat_file_info *fi;

	fi = inode_priv(node);

	if (S_ISDIR(node->i_mode) && !fat_dir_empty(fi)) {
		return -EPERM;
	}

	if (fat_unlike_file(fi, (uint8_t *) fat_sector_buff)) {
		return -1;
	}

	if (S_ISDIR(node->i_mode)) {
		fat_dirinfo_free((void *) fi);
	} else {
		fat_file_free(fi);
	}

	return 0;
}

extern struct inode *fat_ilookup(struct inode *node, char const *name, struct inode const *dir);

struct inode_operations fat_iops = {
	.ino_create   = fat_create,
	.ino_lookup   = fat_ilookup,
	.ino_remove   = fat_delete,
	.ino_iterate  = fat_iterate,
	.ino_truncate = fat_truncate,
};
