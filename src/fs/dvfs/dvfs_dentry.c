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
#include <string.h>

#include <embox/block_dev.h>
#include <fs/dvfs.h>
#include <fs/hlpr_path.h>
#include <kernel/task/resource/vfs.h>

extern int dentry_fill(struct super_block *, struct inode *,
                       struct dentry *, struct dentry *);
extern struct dentry *local_lookup(struct dentry *parent, char *name);

/* @brief Get the length of next element int the path
 * @param path Pointer to the path
 *
 * @return The length of next element in the path
 * @revtal -1 Error
 */
int dvfs_path_next_len(const char *path) {
	int len = strlen(path);
	int off = 0;

	while ((path[off] != '/') && (off < len))
		off++;

	return off;
}

/**
 * @brief Resolve one more element in the path
 * @param path   Pointer to relative path
 * @param dentry The previous dentry
 * @param lookup Structure which is to contain result of path walk
 *
 * @return Negative error code
 * @retval       0 Ok
 * @retval -ENOENT Node not found
 */
int dvfs_path_walk(const char *path, struct dentry *parent, struct lookup *lookup) {
	char buff[DENTRY_NAME_LEN];
	struct inode *in;
	int len;
	struct dentry *d;
	assert(parent);
	assert(path);

	while (*path == '/')
		path++;

	len = dvfs_path_next_len(path);
	memcpy(buff, path, len);
	buff[len] = '\0';

	if (buff[0] == '\0') {
		*lookup = (struct lookup) {
			.item   = parent,
			.parent = parent->parent,
		};
		return 0;
	}

	if ((d = local_lookup(parent, buff)))
		return dvfs_path_walk(path + strlen(buff), d, lookup);

	if (strlen(buff) > 1 && path_is_double_dot(buff))
		return dvfs_path_walk(path + 2, parent->parent, lookup);

	if (strlen(buff) > 1 && path_is_single_dot(buff))
		return dvfs_path_walk(path + 2, parent, lookup);

	/* TODO use cache instead */
	assert(parent->d_sb);
	assert(parent->d_sb->sb_iops);
	assert(parent->d_sb->sb_iops->lookup);

	if (!(in = parent->d_sb->sb_iops->lookup(buff, parent))) {
		*lookup = (struct lookup) {
			.item   = NULL,
			.parent = parent,
		};
		return -ENOENT;
	} else {
		struct dentry *d;
		d = dvfs_alloc_dentry();
		in->i_dentry = parent;
		dentry_fill(parent->d_sb, in, d, parent);
		strcpy(d->name, buff);
		d->flags = in->flags;
	}

	return dvfs_path_walk(path + strlen(buff), in->i_dentry, lookup);
}

/* DVFS interface */

/**
 * @brief Try to find dentry at specified path
 * @param path   Absolute or relative path
 * @param lookup Structure where result will be stored
 *
 * @return Negative error code
 * @retval       0 Ok
 * @retval -ENOENT No node found or incorrect root/pwd dentry
 */
int dvfs_lookup(const char *path, struct lookup *lookup) {
	struct dentry *dentry;
	if (path[0] == '/') {
		dentry = task_fs()->root;
		path++;
	} else
		dentry = task_fs()->pwd;

	if (dentry->d_sb == NULL)
		return -ENOENT;

	/* TODO look in dcache */
	/* TODO flocks */

	return dvfs_path_walk(path, dentry, lookup);
}
