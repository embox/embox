/* @author Denis Deryugin
 * @date 11 Mar 2014
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <fs/dvfs.h>
#include <framework/mod/options.h>
#include <util/bitmap.h>

extern struct inode *dvfs_default_alloc_inode(struct super_block *sb);
extern int dvfs_default_destroy_inode(struct inode *inode);
extern int dvfs_default_pathname(struct inode *inode, char *buf);

struct dentry *dvfs_root(void) {
	static struct dentry *root = NULL;

	return root;
}

struct inode *dvfs_alloc_inode(struct super_block *sb) {
	assert(sb);

	if (sb->sb_ops && sb->sb_ops->alloc_inode)
		return sb->sb_ops->alloc_inode(sb);
	else
		return dvfs_default_alloc_inode(sb);
}

int dvfs_destroy_inode(struct inode *inode) {
	assert(inode);

	if (inode->i_sb && inode->i_sb->sb_ops && inode->i_sb->sb_ops->destroy_inode)
		return inode->i_sb->sb_ops->destroy_inode(inode);
	else
		return dvfs_default_destroy_inode(inode);
}

int dvfs_pathname(struct inode *inode, char *buf) {
	assert(inode);
	assert(buf);

	if (inode->i_ops && inode->i_ops->pathname)
		return inode->i_ops->pathname(inode, buf);
	else
		return dvfs_default_pathname(inode, buf);
}

struct dentry *dvfs_lookup(const char *path) {
	return NULL; /* XXX */
}

int dvfs_open(const char *path, struct file *desc, int mode) {
	//struct inode *inode = dfs_iops.lookup(path, NULL);
	struct dentry *d = dvfs_lookup(path);
	struct inode  *i_no = d->d_inode;

	assert(desc);

	if (!d && (mode & O_CREAT)) {
		/* XXX  Create index node */
		/*
		strcpy(d_new.name, path);
		inode = dfs_iops.create(&d_new, &d_new, 0);
		*/
	}

	*desc = (struct file) {
		.f_dentry = d,
		.f_inode  = i_no,
		.f_ops    = d->d_sb->sb_fops,
	};

	return desc->f_ops->open(i_no, desc);
}

int dvfs_write(struct file *desc, char *buf, int count) {
	if (!desc)
		return -1;

	if (desc->f_ops && desc->f_ops->write)
		return desc->f_ops->write(desc, buf, count);
	else
		return -ENOSYS;
}

int dvfs_read(struct file *desc, char *buf, int count) {
	if (!desc)
		return -1;

	if (desc->f_ops && desc->f_ops->read)
		return desc->f_ops->read(desc, buf, count);
	else
		return -ENOSYS;
}
