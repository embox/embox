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
#include <sys/stat.h>

#include <dirent.h>
#include <dirent_dvfs_impl.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <mem/misc/pool.h>

#define MAX_DIR_QUANTITY OPTION_GET(NUMBER, dir_quantity)

POOL_DEF(dir_pool, DIR, MAX_DIR_QUANTITY);

/* The inode's mode, or the dentry's flags for a virtual directory that has
 * no inode. */
static unsigned char dirent_type(const struct dentry *dentry) {
	mode_t mode = dentry->d_inode ? dentry->d_inode->i_mode : dentry->flags;

	switch (mode & S_IFMT) {
	case S_IFREG:  return DT_REG;
	case S_IFDIR:  return DT_DIR;
	case S_IFCHR:  return DT_CHR;
	case S_IFBLK:  return DT_BLK;
	case S_IFIFO:  return DT_FIFO;
	case S_IFLNK:  return DT_LNK;
	case S_IFSOCK: return DT_SOCK;
	default:       return DT_UNKNOWN;
	}
}

static inline void fill_dirent(struct dirent *dirent, struct dentry *dentry) {
	/* i_no is -1 on a driver with no inode numbers; 0 means "no number". */
	dirent->d_ino = (dentry->d_inode && dentry->d_inode->i_no > 0)
	                    ? (ino_t)dentry->d_inode->i_no : 0;
	dirent->d_type = dirent_type(dentry);
	memcpy(dirent->d_name, dentry->name, NAME_MAX);
}

/* "." and "..": names the tree resolves, not driver entries, so they come
 * from here before the driver's own. The parent is read under the tree's
 * lock; the root is its own "..". */
static void fill_dot(struct dirent *dirent, struct dentry *dir, int dotdot) {
	struct dentry *d = dir;

	dvfs_lock();
	if (dotdot && dir->parent) {
		d = dir->parent;
	}
	dirent->d_ino = (d->d_inode && d->d_inode->i_no > 0)
	                    ? (ino_t)d->d_inode->i_no : 0;
	dvfs_unlock();

	dirent->d_type = DT_DIR;
	dirent->d_reclen = sizeof(*dirent);
	strcpy(dirent->d_name, dotdot ? ".." : ".");
}

DIR *opendir(const char *path) {
	DIR *d;
	struct lookup l = {0, 0};
	int err;

	if ((err = dvfs_lookup(path, &l))) {
		SET_ERRNO(-err);
		return NULL;
	}

	if (l.item == NULL) {
		SET_ERRNO(ENOENT);
		return NULL;
	}

	if (!S_ISDIR(l.item->flags)) {
		dentry_ref_dec(l.item);
		SET_ERRNO(ENOTDIR);
		return NULL;
	}

	if (!(d = pool_alloc(&dir_pool))) {
		dentry_ref_dec(l.item);
		SET_ERRNO(ENOMEM);
		return NULL;
	}

	/* The DIR keeps the lookup's reference until closedir(). */
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

	dentry_ref_dec(dir->dir_dentry);

	pool_free(&dir_pool, dir);

	return 0;
}

struct dirent *readdir(DIR *dir) {
	struct lookup l;
	int err;

	if (!dir) {
		SET_ERRNO(EBADF);
		return NULL;
	}

	/* Positions 0 and 1; the driver's iteration starts after them, which is
	 * why rewinddir(), telldir() and seekdir() need nothing of their own. */
	if (dir->pos < 2) {
		fill_dot(&dir->dirent, dir->dir_dentry, dir->pos == 1);
		dir->pos++;
		dir->dirent.d_off = dir->pos;
		return &dir->dirent;
	}

	l = (struct lookup) {
		.parent = dir->dir_dentry,
	};

	if ((err = dvfs_iterate(&l, &dir->ctx))) {
		SET_ERRNO(-err);
		return NULL;
	}

	if (!l.item) {
		return NULL;
	}

	fill_dirent(&dir->dirent, l.item);
	dir->pos++;
	dir->dirent.d_off = dir->pos;

	/* The entry stays in the tree as cache for reclaim: another core may
	 * have just found it and not yet taken it. */
	dentry_ref_dec(l.item);

	return &dir->dirent;
}

/* A driver keeps only its position in the iteration context, so a zeroed
 * context is a directory not yet read. */
void rewinddir(DIR *dir) {
	if (!dir) {
		return;
	}
	dir->ctx = (struct dir_ctx) {0};
	dir->pos = 0;
}

/* The number of entries read so far: a position seekdir() can return to. */
long telldir(DIR *dir) {
	if (!dir) {
		SET_ERRNO(EBADF);
		return -1;
	}
	return dir->pos;
}

/* The iteration only moves forward, so seeking rereads from the start. */
void seekdir(DIR *dir, long loc) {
	if (!dir || loc < 0) {
		return;
	}
	if (loc < dir->pos) {
		rewinddir(dir);
	}
	while (dir->pos < loc && readdir(dir)) {
	}
}
