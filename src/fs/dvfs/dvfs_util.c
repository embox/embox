/* @file   dvfs_util.c
 * @brief  DVFS allocators, fillers and default handlers
 * @author Denis Deryugin
 * @date   8 Apr 2014
 */

#include <fcntl.h>
#include <string.h>

#include <fs/dvfs.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>

#define SUPERBLOCK_POOL_SIZE OPTION_GET(NUMBER, superblock_pool_size)
#define INODE_POOL_SIZE OPTION_GET(NUMBER, inode_pool_size)
#define DENTRY_POOL_SIZE OPTION_GET(NUMBER, dentry_pool_size)
#define FILE_POOL_SIZE OPTION_GET(NUMBER, file_pool_size)
#define MNT_POOL_SIZE OPTION_GET(NUMBER, mnt_pool_size)

POOL_DEF(superblock_pool, struct super_block, SUPERBLOCK_POOL_SIZE);
POOL_DEF(inode_pool, struct inode, INODE_POOL_SIZE);
POOL_DEF(dentry_pool, struct dentry, DENTRY_POOL_SIZE);
POOL_DEF(file_pool, struct file, FILE_POOL_SIZE);
POOL_DEF(mnt_pool, struct dvfsmnt, MNT_POOL_SIZE);

/* Default FS-nondependent operations */
struct inode *dvfs_default_alloc_inode(struct super_block *sb) {
	struct inode *inode;
	if (!sb)
		return NULL;

	if ((inode = pool_alloc(&inode_pool)))
		*inode = (struct inode) {
			.i_no = -1,
			.i_sb = sb,
			.i_ops = sb->sb_iops,
		};

	return inode;
}

int dvfs_default_destroy_inode(struct inode *inode) {
	pool_free(&inode_pool, inode);
	return 0;
}

int dvfs_default_pathname(struct inode *inode, char *buf) {
	assert(inode);
	if (inode->i_dentry)
		strcpy(buf, inode->i_dentry->name);
	else
		strcpy(buf, "empty");

	return 0;
}

struct super_block *dvfs_alloc_sb(struct dumb_fs_driver *drv, char *dev) {
	assert(drv);

	if (drv->alloc_sb)
		return drv->alloc_sb(dev);
	else
		return NULL;
}

struct inode *dvfs_alloc_inode(struct super_block *sb) {
	if (sb->sb_ops && sb->sb_ops->alloc_inode)
		return sb->sb_ops->alloc_inode(sb);
	else
		return dvfs_default_alloc_inode(sb);
}

extern int dvfs_default_destroy_inode(struct inode *);
int dvfs_destroy_inode(struct inode *inode) {
	assert(inode);

	if (inode->i_sb && inode->i_sb->sb_ops &&
	    inode->i_sb->sb_ops->destroy_inode)
		return inode->i_sb->sb_ops->destroy_inode(inode);
	else
		return dvfs_default_destroy_inode(inode);
}

struct dentry *dvfs_alloc_dentry(void) {
	return pool_alloc(&dentry_pool);
}

int dvfs_destroy_dentry(struct dentry *dentry) {
	pool_free(&dentry_pool, dentry);
	return 0;
}

struct file *dvfs_alloc_file(void) {
	return pool_alloc(&file_pool);
}

int dvfs_destroy_file(struct file *desc) {
	pool_free(&file_pool, desc);
	return 0;
}

struct dvfsmnt *dvfs_alloc_mnt(void) {
	return pool_alloc(&mnt_pool);
}

int dvfs_destroy_mnt(struct dvfsmnt *mnt) {
	pool_free(&mnt_pool, mnt);
	return 0;
}

/* Fillers */
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

int dentry_fill(struct super_block *sb, struct inode *inode,
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

/* Root-related stuff */
static struct dentry *global_root = NULL;
extern struct super_block *rootfs_sb(void);
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

