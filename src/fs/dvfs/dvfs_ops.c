/* @author Denis Deryugin
 * @date 27 Mar 2014
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <fs/dvfs.h>
#include <fs/dfs.h>
#include <framework/mod/options.h>
#include <mem/misc/pool.h>
#include <util/bitmap.h>

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
