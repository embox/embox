/**
* @file dvfs_dentry.c
* @brief Handle dentry routines
* @author Denis Deryugin <deryugin.denis@gmail.com>
* @version 0.1
* @date 2015-06-01
*/
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <lib/cwalk.h>

#include <fs/dvfs.h>
#include <fs/dentry.h>
#include <fs/super_block.h>
#include <fs/inode_operation.h>
#include <fs/inode.h>

#include <kernel/task/resource/vfs.h>

#include <util/log.h>

#include <framework/mod/options.h>

#define DENTRY_POOL_SIZE OPTION_GET(NUMBER, dentry_pool_size)

/**
 * @brief Get full path from global root to given dentry
 *
 * @param dentry
 * @param buf
 *
 * @return
 */
int dentry_full_path(struct dentry *dentry, char *buf) {
	int cur_len;
	size_t nlen;

	cur_len = 0;

	/* A rename on another core moves names and parents; hold the tree still
	 * while walking up it. */
	dvfs_lock();
	do {
		nlen = strlen(dentry->name);
		if (cur_len) {
			cur_len++;
			memmove(buf + nlen + 1, buf, cur_len);
			buf[nlen] = '/';
		}
		memcpy(buf, dentry->name, nlen);
		cur_len += nlen;
		dentry = dentry->parent;
	} while (dentry != dvfs_root());
	dvfs_unlock();

	if (buf[0] != '/') {
		memmove(buf + 1, buf, cur_len);
		buf[0] = '/';
		cur_len++;
	}

	buf[cur_len] = '\0';

	return 0;
}

extern struct dentry *local_lookup(struct dentry *parent, char *name);
extern int dvfs_default_destroy_inode(struct inode *);
/**
 * @brief Resolve one more element in the path
 * @param segment Segment of a path
 * @param parent  The previous dentry, held by the caller
 * @param dentry  Result of path walk, referenced for the caller
 *
 * @return Negative error code
 * @retval             0 Ok
 * @retval       -ENOENT Node not found
 * @retval       -ENOMEM Cannot alloc dentry
 * @retval      -ENOTDIR Intermediate part of the path is not a directory
 * @retval -ENAMETOOLONG Path is too long
 *
 * The child is referenced before anything else is allocated: an allocation
 * may reclaim any unreferenced cached dentry.
 */
int dvfs_path_walk(struct cwk_segment *segment, struct dentry *parent,
    struct dentry **dentry) {
	struct dentry *d;
	struct inode *inode;
	char buff[NAME_MAX];
	int retried = 0;

	assert(parent);
	assert(segment);
	dvfs_lock_assert("dvfs_path_walk");

	if (!S_ISDIR(parent->flags)) {
		return -ENOTDIR;
	}

	if (segment->size >= NAME_MAX) {
		return -ENAMETOOLONG;
	}

	d = NULL;

	switch (cwk_path_get_segment_type(segment)) {
	case CWK_BACK:
		parent = parent->parent;
		/* fallthrough */

	case CWK_CURRENT:
		d = parent;
		dentry_ref_inc(d);
		break;

	case CWK_NORMAL:
		if (parent->flags & DVFS_DYING) {
			/* A removed directory has no names left, and its driver data may
			 * be gone with it: never ask the driver about it. */
			return -ENOENT;
		}
		if (parent->d_sb == NULL) {
			/* A virtual directory has nothing beyond what is in memory */
			memcpy(buff, segment->begin, segment->size);
			buff[segment->size] = '\0';
			if ((d = local_lookup(parent, buff))) {
				dentry_ref_inc(d);
				break;
			}
			return -ENOENT;
		}

		assert(parent->d_sb->sb_iops);
		assert(parent->d_sb->sb_iops->ino_lookup);

		memcpy(buff, segment->begin, segment->size);
		buff[segment->size] = '\0';

		if ((d = local_lookup(parent, buff))) {
			dentry_ref_inc(d);
			break;
		}

again:
		inode = dvfs_alloc_inode(parent->d_sb);
		if (NULL == inode) {
			return -ENOMEM;
		}

		if (!parent->d_sb->sb_iops->ino_lookup(inode, buff, parent->d_inode)) {
			dvfs_default_destroy_inode(inode);
			/* A driver with its own pool (FAT, initfs) says "not found" when
			 * the pool is empty: free a cached dentry and ask once more. */
			if (!retried && dvfs_fs_dentry_try_free(parent->d_sb)) {
				retried = 1;
				goto again;
			}
			return -ENOENT;
		}

		d = dvfs_alloc_dentry();
		if (!d) {
			dvfs_destroy_inode(inode);
			return -ENOMEM;
		}

		dentry_fill(parent->d_sb, inode, d, parent);
		strcpy(d->name, buff);
		d->flags = inode->i_mode;
		dentry_ref_inc(d);
	}

	if (dentry) {
		*dentry = d;
	} else {
		dentry_ref_dec(d);
	}

	return 0;
}

/* DVFS interface */

void dvfs_lookup_put(struct lookup *lookup) {
	if (lookup->item) {
		dentry_ref_dec(lookup->item);
		lookup->item = NULL;
	}
	if (lookup->parent) {
		dentry_ref_dec(lookup->parent);
		lookup->parent = NULL;
	}
}

int dvfs_lookup_at(struct dentry *base, const char *path,
    struct lookup *lookup, char *last) {
	struct dentry *dentry, *next;
	struct cwk_segment segment;
	int err;
	char normal_path[PATH_MAX];

	assert(path);
	assert(lookup);
	dvfs_lock_assert("dvfs_lookup_at");

	lookup->item = NULL;
	lookup->parent = NULL;

	if (*path == '\0') {
		return -ENOENT;
	}

	if (strlen(path) >= sizeof(normal_path)) {
		return -ENAMETOOLONG;
	}

	cwk_path_normalize(path, normal_path, sizeof(normal_path));

	if (cwk_path_is_absolute(normal_path)) {
		dentry = dvfs_root();
	}
	else if (base) {
		dentry = base;
	}
	else {
		dentry = task_self_resource_vfs()->pwd;
		if (dentry == NULL) {
			dentry = dvfs_root();
		}
	}

	if (dentry->d_sb == NULL && !(dentry->flags & VFS_DIR_VIRTUAL)) {
		return -ENOENT;
	}

	dentry_ref_inc(dentry);

	if (!cwk_path_get_first_segment(normal_path, &segment)) {
		/* "/" or "." -- the start is the answer, and its own parent */
		lookup->item = dentry;
		lookup->parent = dentry->parent;
		dentry_ref_inc(lookup->parent);
		return 0;
	}

	for (;;) {
		err = dvfs_path_walk(&segment, dentry, &next);
		if (err) {
			break;
		}
		dentry_ref_dec(dentry);
		dentry = next;

		if (!cwk_path_get_next_segment(&segment)) {
			/* The whole path resolved */
			lookup->item = dentry;
			lookup->parent = dentry->parent;
			dentry_ref_inc(lookup->parent);
			return 0;
		}
	}

	/* DENTRY is the last directory reached and is still held. */
	if (err == -ENOENT) {
		struct cwk_segment rest = segment;

		if (!cwk_path_get_next_segment(&rest)) {
			/* Only the last component is missing: that is the answer a
			 * create needs, with the directory it goes into. */
			if (last) {
				size_t n = segment.size < NAME_MAX - 1 ? segment.size
				                                       : NAME_MAX - 1;
				memcpy(last, segment.begin, n);
				last[n] = '\0';
			}
			lookup->parent = dentry;
			return 0;
		}
	}

	dentry_ref_dec(dentry);

	return err;
}

/**
 * @brief Try to find dentry at specified path
 * @param path   Absolute or relative path
 * @param lookup Structure where result will be stored
 *
 * @return Negative error code
 * @retval             0 Ok (item may be NULL: only the last part is missing)
 * @retval       -ENOENT An earlier part of the path is missing
 * @retval      -ENOTDIR Intermediate part of the path is not a directory
 * @retval -ENAMETOOLONG Path is too long
 *
 * For callers outside the tree. lookup->item comes back referenced;
 * lookup->parent does not -- see fs/dentry.h. Whatever lookup held on entry
 * is ignored: relative paths start at the working directory.
 */
int dvfs_lookup(const char *path, struct lookup *lookup) {
	int err;

	dvfs_lock();
	err = dvfs_lookup_at(NULL, path, lookup, NULL);
	if (err == 0 && lookup->parent) {
		dentry_ref_dec(lookup->parent);
	}
	dvfs_unlock();

	return err;
}
