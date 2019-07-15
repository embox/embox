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
	struct lookup l = {0, 0};
	int err;

	if ((err = dvfs_lookup(path, &l)) || l.item == NULL) {
		SET_ERRNO(-err);
		return NULL;
	}

	if (!(d = pool_alloc(&dir_pool))) {
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	*d = (DIR) {
		.dir_dentry = l.item,
	};

	fill_dirent(&d->dirent, l.item);
	dentry_ref_inc(l.item);

	return d;
}

int closedir(DIR *dir) {
	if (!dir) {
		SET_ERRNO(EBADF);
		return -1;
	}

	dentry_ref_dec(dir->dir_dentry);

	pool_free(&dir_pool, dir);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct lookup l = {0, 0};
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

	if (dvfs_iterate(&l, &dir->ctx)) {
		SET_ERRNO(EAGAIN);
		return NULL;
	}

	if (!l.item) {
		return NULL;
	}
	fill_dirent(&dir->dirent, l.item);

	dentry_ref_dec(l.item);

	return &dir->dirent;
}
