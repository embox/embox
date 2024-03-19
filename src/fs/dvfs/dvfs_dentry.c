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
 * @param parent  The previous dentry
 * @param parent  The previous dentry
 * @param dentry  Result of path walk
 *
 * @return Negative error code
 * @retval             0 Ok
 * @retval       -ENOENT Node not found
 * @retval       -ENOMEM Cannot alloc dentry
 * @retval      -ENOTDIR Intermediate part of the path is not a directory
 * @retval -ENAMETOOLONG Path is too long
 */
int dvfs_path_walk(struct cwk_segment *segment, struct dentry *parent,
    struct dentry **dentry) {
	struct dentry *d;
	struct inode *inode;
	char buff[NAME_MAX];

	assert(parent);
	assert(segment);

	if (!S_ISDIR(parent->flags)) {
		return -ENOTDIR;
	}

	if (segment->size >= NAME_MAX) {
		return -ENAMETOOLONG;
	}

	switch (cwk_path_get_segment_type(segment)) {
	case CWK_BACK:
		parent = parent->parent;

	case CWK_CURRENT:
		d = parent;
		break;

	case CWK_NORMAL:
		assert(parent->d_sb);
		assert(parent->d_sb->sb_iops);
		assert(parent->d_sb->sb_iops->ino_lookup);

		memcpy(buff, segment->begin, segment->size);
		buff[segment->size] = '\0';

		if ((d = local_lookup(parent, buff))) {
			break;
		}
		
		inode = dvfs_alloc_inode(parent->d_sb);
		if (NULL == inode) {
			return -ENOMEM;
		}

		if (!parent->d_sb->sb_iops->ino_lookup(inode, buff, parent->d_inode)) {
			dvfs_default_destroy_inode(inode);
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
	}

	if (dentry) {
		*dentry = d;
	}

	return 0;
}

/* DVFS interface */

/**
 * @brief Try to find dentry at specified path
 * @param path   Absolute or relative path
 * @param lookup Structure where result will be stored
 *
 * @return Negative error code
 * @retval             0 Ok
 * @retval       -ENOENT Incorrect root/pwd dentry
 * @retval      -ENOTDIR Intermediate part of the path is not a directory
 * @retval -ENAMETOOLONG Path is too long
 */
int dvfs_lookup(const char *path, struct lookup *lookup) {
	struct dentry *dentry;
	struct cwk_segment segment;
	int err;
	char normal_path[PATH_MAX];

	err = 0;

	assert(path);
	assert(lookup);

	cwk_path_normalize(path, normal_path, sizeof(normal_path));

	if (cwk_path_is_absolute(normal_path)) {
		dentry = dvfs_root();
	}
	else {
		if (lookup->item == NULL) {
			dentry = task_self_resource_vfs()->pwd;
		}
		else {
			dentry = lookup->item;
		}
	}

	if (dentry->d_sb == NULL) {
		lookup->item = NULL;
		return -ENOENT;
	}

	if (cwk_path_get_first_segment(path, &segment)) {
		while (!(err = dvfs_path_walk(&segment, dentry, &dentry))
		       && cwk_path_get_next_segment(&segment)) {}
	}

	if (err) {
		lookup->item = NULL;
		lookup->parent = dentry;
	}
	else {
		dentry_ref_inc(dentry);
		lookup->item = dentry;
		lookup->parent = dentry->parent;
	}

	return (err == -ENOENT) ? 0 : err;
}
