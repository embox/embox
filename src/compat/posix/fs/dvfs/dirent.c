/**
 * @file
 * @brief
 *
 * @date 5 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <posix_errno.h>
#include <stddef.h>
#include <string.h>

#include <dirent.h>
#include <dirent_dvfs_impl.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <mem/misc/pool.h>

#define MAX_DIR_QUANTITY OPTION_GET(NUMBER, dir_quantity)

POOL_DEF(dir_pool, DIR, MAX_DIR_QUANTITY);

static inline void fill_dirent(struct dirent *dirent, struct dentry *dentry) {
	dirent->d_ino = dentry->d_inode->i_no;
	memcpy(dirent->d_name, dentry->name, DIRENT_DNAME_LEN);
}

DIR *opendir(const char *path) {
	DIR *d;
	struct lookup l;

	if (dvfs_lookup(path, &l)) {
		SET_ERRNO(ENOENT);
		return NULL;
	}

	l.item->usage_count++;
	d = pool_alloc(&dir_pool);
	*d = (DIR) {
		.dir_dentry = l.item,
	};

	fill_dirent(&d->dirent, l.item);

	return d;
}

int closedir(DIR *dir) {
	if (!dir) {
		SET_ERRNO(EBADF);
		return -1;
	}

	if (dir->prv_dentry) {
		dir->prv_dentry->usage_count--;
		dvfs_destroy_dentry(dir->prv_dentry);
	}

	dir->dir_dentry->usage_count--;
	dvfs_destroy_dentry(dir->dir_dentry);

	pool_free(&dir_pool, dir);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct lookup l;
	struct dentry *prev;

	if (!dir) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	prev = dir->prv_dentry;

	l = (struct lookup) {
		.parent = dir->dir_dentry,
		.item   = prev,
	};

	if (prev) {
		dvfs_destroy_dentry(prev);
	}

	if (dvfs_iterate(&l, &dir->ctx)) {
		SET_ERRNO(EAGAIN);
		return NULL;
	}

	if (!l.item) {
		dir->ctx.pos = 0;
		return NULL;
	}
	fill_dirent(&dir->dirent, l.item);

	return &dir->dirent;
}
