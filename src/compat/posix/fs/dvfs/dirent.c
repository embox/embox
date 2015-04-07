/**
 * @file
 * @brief
 *
 * @date 5 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <posix_errno.h>
#include <string.h>

#include <dirent.h>
#include <defines/null.h>
#include <dirent_dvfs_impl.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <mem/objalloc.h>

#define MAX_DIR_QUANTITY OPTION_GET(NUMBER, dir_quantity)

OBJALLOC_DEF(dir_pool, DIR, MAX_DIR_QUANTITY);

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
	d = objalloc(&dir_pool);
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

	dir->dir_dentry->usage_count--;
	objfree(&dir_pool, dir);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct lookup l;
	struct dir_ctx c;

	if (!dir) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	l = (struct lookup) {
		.parent = dir->dir_dentry,
		.item   = dir->prv_dentry,
	};

	c.pos = dir->pos;
	if (dvfs_iterate(&l, &c)) {
		SET_ERRNO(EAGAIN);
		return NULL;
	}

	if (!l.item) {
		dir->pos = 0;
		return NULL;
	}
	fill_dirent(&dir->dirent, l.item);
	dir->pos++;

	return &dir->dirent;
}
