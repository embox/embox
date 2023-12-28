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

#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <fs/hlpr_path.h>
#include <kernel/task/resource/vfs.h>
#include <util/log.h>

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

/**
 * @brief Get the length of next element int the path
 * @param path Pointer to the path
 *
 * @return The length of next element in the path
 * @revtal -1 Error
 */
int dvfs_path_next_len(const char *path) {
	int len;
	int off;

	len = strlen(path);
	off = 0;

	while ((path[off] != '/') && (off < len)) {
		off++;
	}

	return off;
}
/**
 * @brief Resolve one more element in the path
 * @param path   Pointer to relative path
 * @param dentry The previous dentry
 * @param lookup Structure which is to contain result of path walk
 *
 * @return Negative error code
 * @retval             0 Ok
 * @retval       -ENOENT Node not found
 * @retval      -ENOTDIR Intermediate part of the path is not a directory
 * @retval -ENAMETOOLONG path is too long
 */
int dvfs_path_walk(const char *path, struct dentry *parent,
    struct lookup *lookup) {
	char buff[NAME_MAX];
	struct inode *inode;
	struct dentry *d;
	int len;

	assert(parent);
	assert(path);

	while (*path == '/') {
		path++;
	}

	len = dvfs_path_next_len(path);
	if (len >= NAME_MAX) {
		return -ENAMETOOLONG;
	}

	memcpy(buff, path, len);
	buff[len] = '\0';

	if (buff[0] == '\0') {
		dentry_ref_inc(parent);
		*lookup = (struct lookup){
		    .item = parent,
		    .parent = parent->parent,
		};
		return 0;
	}

	if (!S_ISDIR(parent->flags)) {
		return -ENOTDIR;
	}

	if ((d = local_lookup(parent, buff))) {
		return dvfs_path_walk(path + strlen(buff), d, lookup);
	}

	if (strlen(buff) > 1 && path_is_double_dot(buff)) {
		return dvfs_path_walk(path + 2, parent->parent, lookup);
	}

	if (path_is_single_dot(buff)) {
		return dvfs_path_walk(path + 1, parent, lookup);
	}

	/* TODO use cache instead */
	assert(parent->d_sb);
	assert(parent->d_sb->sb_iops);
	assert(parent->d_sb->sb_iops->lookup);

	inode = parent->d_sb->sb_iops->lookup(buff, parent->d_inode);
	if (!inode) {
		*lookup = (struct lookup){
		    .item = NULL,
		    .parent = parent,
		};
		return -ENOENT;
	}

	d = dvfs_alloc_dentry();
	inode->i_dentry = parent;
	dentry_fill(parent->d_sb, inode, d, parent);
	strcpy(d->name, buff);
	d->flags = inode->i_mode;

	return dvfs_path_walk(path + strlen(buff), inode->i_dentry, lookup);
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
 * @retval -ENAMETOOLONG path is too long
 */
int dvfs_lookup(const char *path, struct lookup *lookup) {
	struct dentry *dentry;
	struct dentry *res;
	int errcode;

	assert(path);
	assert(lookup);

	if (*path == '/') {
		dentry = task_self_resource_vfs()->root;
		path++;
	}
	else {
		if (lookup->item == NULL) {
			dentry = task_self_resource_vfs()->pwd;
		}
		else {
			dentry = lookup->item;
		}
	}

	if (*path == '\0') {
		dentry_ref_inc(dentry);
		*lookup = (struct lookup){
		    .item = dentry,
		    .parent = dentry->parent,
		};
		return 0;
	}

	if (dentry->d_sb == NULL) {
		lookup->item = NULL;
		return -ENOENT;
	}

	/* TODO preprocess path ? Delete "/../" */

	if ((res = dvfs_cache_lookup(path, dentry))) {
		*lookup = (struct lookup){
		    .item = res,
		    .parent = res->parent,
		};
		return 0;
	}

	errcode = dvfs_path_walk(path, dentry, lookup);
	if (!errcode) {
		dvfs_cache_add(lookup->item);
	}

	return errcode == -ENOENT ? 0 : errcode;
}
