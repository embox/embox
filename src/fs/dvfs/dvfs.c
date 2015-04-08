/* @file   dvfs.c
 * @brief  Implementation of common functions of DVFS
 * @author Denis Deryugin
 * @date   11 Mar 2014
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

int inode_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry) {
	*inode = (struct inode) {
		.i_dentry  = dentry,
		.i_sb      = sb,
		.i_ops     = sb ? sb->sb_iops : NULL,
		.start_pos = inode->start_pos,
	};

	return 0;
}

static int dentry_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry, struct dentry *parent) {
	*dentry = (struct dentry) {
		.d_inode = inode,
		.d_sb    = sb,
		.d_ops   = sb ? sb->sb_dops : NULL,
		.parent  = parent,
	};

	inode->i_dentry = dentry;

	dlist_init(&dentry->children);
	dlist_head_init(&dentry->children_lnk);

	if (parent)
		dlist_add_prev(&dentry->children_lnk, &parent->children);

	return 0;
}

extern struct super_block *rootfs_sb(void);
static struct dentry *global_root = NULL;
int dvfs_update_root(void) {
	if (!global_root)
		global_root = dvfs_alloc_dentry();

	if (!global_root->d_inode)
		global_root->d_inode = dvfs_alloc_inode(rootfs_sb());

	*global_root = (struct dentry) {
		.d_sb = rootfs_sb(),
		.parent = global_root,
		.name = "/",
		.flags = O_DIRECTORY,
	};

	if (global_root->d_inode)
		*(global_root->d_inode) = (struct inode) {
			.flags = O_DIRECTORY,
			.i_ops = rootfs_sb()->sb_iops,
		};

	global_root->d_sb->root = global_root;

	dlist_init(&global_root->children);
	dlist_head_init(&global_root->children_lnk);
	return 0;
}

struct dentry *dvfs_root(void) {
	if (!global_root) {
		dvfs_update_root();
	}

	return global_root;
}

struct inode *dvfs_alloc_inode(struct super_block *sb) {
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

int dvfs_path_next_len(const char *path) {
	int len = strlen(path);
	int off = 0;

	while (path[off] == '/')
		off++;

	while ((path[off] != '/') && (off < len))
		off++;

	return off;
}

int dvfs_path_walk(const char *path, struct dentry *parent, struct lookup *lookup) {
	char buff[DENTRY_NAME_LEN];
	struct inode *in;
	int len = dvfs_path_next_len(path);
	assert(parent);
	assert(path);

	memcpy(buff, path, len);
	buff[len] = '\0';

	if (buff[0] == '\0') {
		*lookup = (struct lookup) {
			.item   = parent,
			.parent = parent->parent,
		};
		return 0;
	}


	if (strlen(buff) > 1 && path_is_double_dot(buff))
		return dvfs_path_walk(path + 2, parent->parent, lookup);

	if (strlen(buff) > 1 && path_is_single_dot(buff))
		return dvfs_path_walk(path + 2, parent, lookup);

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
	}

	return dvfs_path_walk(path + strlen(buff), in->i_dentry, lookup);
}

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

/*	len = strlen(d->name);

	while (offt < len && d->name[offt] == path[offt])
		offt++;

	offt++; */
}

extern struct super_block *dfs_sb(void);
int dvfs_create_new(const char *name, struct lookup *lookup, int flags) {
	struct super_block *sb;
	assert(lookup);
	assert(lookup->parent);
	assert(lookup->parent->flags & O_DIRECTORY);

	/* TODO Find super_block */
	sb = lookup->parent->d_sb;
	lookup->item = dvfs_alloc_dentry();
	if (!lookup->item)
		return -1;

	dentry_fill(sb, NULL, lookup->item, lookup->parent);
	strcpy(lookup->item->name, name);

	lookup->item->d_inode = sb->sb_iops->create(lookup->item, lookup->parent, flags);
	inode_fill(sb, lookup->item->d_inode, lookup->item);
	/* TODO add dentry to cache */

	return 0;
}

int dvfs_open(const char *path, struct file *desc, int mode) {
	struct lookup lookup;
	struct inode  *i_no;

	dvfs_lookup(path, &lookup);

	assert(desc);

	if (!lookup.item) {
		if (mode & O_CREAT) {
			char *last_name = strrchr(path, '/');
			dvfs_create_new(last_name ? last_name + 1 : path, &lookup, 0);
		} else {
			desc->f_inode = NULL;
			desc->f_dentry = NULL;
			return -ENOENT;
		}
	}

	i_no = lookup.item->d_inode;

	*desc = (struct file) {
		.f_dentry = lookup.item,
		.f_inode  = i_no,
		.f_ops    = lookup.item->d_sb->sb_fops,
	};

	if (i_no == NULL || i_no->flags & O_DIRECTORY) {
		if (lookup.item)
			dvfs_destroy_dentry(lookup.item);
		return -ENOENT;
	}

	assert(desc->f_ops);
	assert(desc->f_ops->open);

	lookup.item->usage_count++;

	return desc->f_ops->open(i_no, desc);
}

int dvfs_close(struct file *desc) {
	if (!desc || !desc->f_inode || !desc->f_dentry)
		return -1;

	desc->f_dentry->usage_count--;
	if (!desc->f_dentry->usage_count) {
		dvfs_destroy_dentry(desc->f_dentry);
		dvfs_destroy_inode(desc->f_inode);
	}

	dvfs_destroy_file(desc);
	return 0;
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

int dvfs_mount(char *dev, char *dest, char *fstype, int flags) {
	struct lookup lookup;
	struct dumb_fs_driver *drv;
	struct dvfsmnt *mnt;
	struct super_block *sb;
	struct dentry *d;

	dvfs_lookup(dest, &lookup);

	if (lookup.item == NULL)
		return -ENOENT;

	/* find by name ? */
	drv = dumb_fs_driver_find(fstype);
	mnt = dvfs_alloc_mnt();
	sb  = dvfs_alloc_sb(drv, dev);
	/* TODO init sb */
	d   = dvfs_alloc_dentry();

	dentry_fill(sb, NULL, d, NULL);
	d->usage_count++;

	*mnt = (struct dvfsmnt) {
		.mnt_sb = sb,
		.mnt_root = d, /* XXX */
		.mnt_mountpoint = lookup.item,
	};

	return 0;
}

int dvfs_iterate(struct lookup *lookup, struct dir_ctx *ctx) {
	struct super_block *sb;
	struct inode *parent_inode;
	struct inode *next_inode;
	struct dentry *next_dentry = NULL;
	assert(lookup);
	assert(ctx);

	/* TODO free prev dentry */

	sb = lookup->parent->d_sb;
	parent_inode = lookup->parent->d_inode;

	next_inode  = sb->sb_iops->iterate(parent_inode, ctx);
	if (!next_inode) {
		ctx->pos = 0;
	} else {
		ctx->pos++;
		next_dentry = dvfs_alloc_dentry();
		dentry_fill(sb, next_inode, next_dentry, lookup->parent);
		sb->sb_iops->pathname(next_inode, next_dentry->name);
	}

	lookup->item = next_dentry;

	return 0;
}
