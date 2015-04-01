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
#include <fs/hlpr_path.h>
#include <framework/mod/options.h>
#include <kernel/task/resource/file_table.h>
#include <util/bitmap.h>

extern struct inode *dvfs_default_alloc_inode(struct super_block *sb);
extern int dvfs_default_destroy_inode(struct inode *inode);
extern int dvfs_default_pathname(struct inode *inode, char *buf);

static int inode_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry) {
	*inode = (struct inode) {
		.i_dentry = dentry,
		.i_sb     = sb,
		.i_ops    = sb ? sb->sb_iops : NULL,
	};

	return 0;
}

static int dentry_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry, struct dentry *parent) {
	*dentry = (struct dentry) {
		.d_inode = inode,
		.d_sb    = sb,
		.d_ops   = sb ? sb->sb_dops : NULL,
	};

	/* TODO lists */

	return 0;
}

extern struct super_block *dfs_sb(void);
struct dentry *dvfs_root(void) {
	static struct dentry *root = NULL;

	if (!root) {
		root = dvfs_alloc_dentry();
		*root = (struct dentry) {
			.d_sb = dfs_sb(),
			.parent = root,
		};
	}

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

struct dentry *dvfs_path_walk(const char *path, struct dentry *parent) {
	const char *p;
	struct inode *in;
	size_t len;

	if (path[0] == '\0')
		return parent;

	assert(parent);
	assert(path);

	len = strlen(path);
	p = path_next(path, &len);

	if (strlen(p) > 1 && path_is_double_dot(p))
		return dvfs_path_walk(p + 2, parent->parent);

	assert(parent->d_sb);
	assert(parent->d_sb->sb_iops);
	assert(parent->d_sb->sb_iops->lookup);

	if ((in = parent->d_sb->sb_iops->lookup(p, parent)))
		return NULL;

	return dvfs_path_walk(p, in->i_dentry);
}

struct dentry *dvfs_lookup(const char *path) {
	struct dentry *dentry;

	if (path[0] == '/')
		dentry = task_fs()->root;
	else
		dentry = task_fs()->pwd;

	/* TODO look in dcache */
	/* TODO flocks */

	return dvfs_path_walk(path, dentry);

/*	len = strlen(d->name);

	while (offt < len && d->name[offt] == path[offt])
		offt++;

	offt++; */
}

int dvfs_open(const char *path, struct file *desc, int mode) {
	struct super_block *sb;
	struct dentry *parent = NULL, *d = dvfs_lookup(path);
	struct inode  *i_no = d->d_inode;

	assert(desc);

	if (!d) {
		if (mode & O_CREAT) {
			/* TODO Find super_block */
			sb   = dfs_sb();
			i_no = dvfs_alloc_inode(sb);
			d    = dvfs_alloc_dentry();

			inode_fill(sb, i_no, d);
			dentry_fill(sb, i_no, d, parent);

			/* TODO add dentry to cache */
		} else {
			return -ENOENT;
		}
	}

	*desc = (struct file) {
		.f_dentry = d,
		.f_inode  = i_no,
		.f_ops    = d->d_sb->sb_fops,
	};

	if (i_no == NULL) {
		dvfs_destroy_dentry(d);
		return -ENOENT;
	}

	assert(desc->f_ops);
	assert(desc->f_ops->open);

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
