/* @file
 * @brief  DVFS allocators, fillers and default handlers
 * @author Denis Deryugin
 * @date   8 Apr 2014
 */

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>

#include <drivers/device.h>
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

#define FREE_DENTRY_ANY    0
#define FREE_DENTRY_INODE  1
static int dvfs_free_dentry(int with_ino);

/* Default FS-nondependent operations */
/* @brief Alloc inode and set it superblock and inode_operations
 * @param sb Superblock of related file system
 *
 * @return Pointer to the new inode
 * @retval NULL inode could not be allocated
 */
struct inode *dvfs_alloc_inode(struct super_block *sb) {
	struct inode *inode;
	if (!sb)
		return NULL;

	inode = pool_alloc(&inode_pool);
	if (!inode) {
		if (!dvfs_free_dentry(FREE_DENTRY_INODE)) {
			inode = pool_alloc(&inode_pool);
		} else {
			return NULL;
		}
	}

	assert(inode);

	*inode = (struct inode) {
		.i_no = -1,
		.i_sb = sb,
		.i_ops = sb->sb_iops,
	};

	return inode;
}

/* @brief Remove inode from inode_pool
 * @param inode inode to be removed from pool
 *
 * @retval 0 Ok
 */
int dvfs_default_destroy_inode(struct inode *inode) {
	pool_free(&inode_pool, inode);
	return 0;
}

/* @brief Try to resolve pathname according to the dentry
 * @param inode The inode which is to be path-resolved
 * @param buf   Buffer for the path
 * @param flags Used to figure out pathname format, see dvfs_pathname doc
 *
 * @retval 0 Ok
 */
int dvfs_default_pathname(struct inode *inode, char *buf, int flags) {
	assert(inode);
	if (inode->i_dentry)
		strcpy(buf, inode->i_dentry->name);
	else
		strcpy(buf, "empty");

	return 0;
}

/* @brief Try to allocate superblock using file system driver and given device
 * @param drv Name of file system driver
 * @param dev Path to device (e.g. /dev/sda1)
 *
 * @return Pointer to the new superblock
 * @retval NULL Superblock could not be allocated
 */
struct super_block *dvfs_alloc_sb(const struct dumb_fs_driver *drv, struct file *bdev_file) {
	struct super_block *sb;
	assert(drv);

	sb = pool_alloc(&superblock_pool);

	if (bdev_file) {
		assert(bdev_file->f_inode);
		assert(bdev_file->f_inode->i_data);
	}

	*sb = (struct super_block) {
		.fs_drv    = drv,
		.bdev_file = bdev_file,
		.bdev      = bdev_file ? ((struct dev_module *) bdev_file->f_inode->i_data)->dev_priv : NULL,
	};

	if (drv->fill_sb)
		drv->fill_sb(sb, bdev_file);

	return sb;
}

extern int dvfs_default_destroy_inode(struct inode *);
/* @brief Try to destroy the inode
 * @param inode Pointer to the inode to be destroyed
 *
 * @return Negative error code
 * @retval 0 Ok
 */
int dvfs_destroy_inode(struct inode *inode) {
	assert(inode);

	if (inode->i_dentry)
		inode->i_dentry->d_inode = NULL;

	if (inode->i_sb && inode->i_sb->sb_ops &&
	    inode->i_sb->sb_ops->destroy_inode)
		inode->i_sb->sb_ops->destroy_inode(inode);
	return dvfs_default_destroy_inode(inode);
}

/**
* @brief Double-linked list of all dentries
*/
DLIST_DEFINE(dentry_dlist);

/**
 * @brief Free entries with zero usage count (i. e. cached entries)
 *
 * @param mode  FREE_DENTRY_ANY	   Find any dentry to delete
 *              FREE_DENTRY_INODE  Find dentry with inodes
 *
 * @return 0      if succeeded
 *         -EBUSY if no free dentry found
 */
static int dvfs_free_dentry(int mode) {
	struct dentry *dentry;
	dlist_foreach_entry(dentry, &dentry_dlist, d_lnk) {
		if (mode == FREE_DENTRY_INODE && dentry->d_inode == NULL)
			continue;

		if (dentry->usage_count == 0) {
			dvfs_destroy_dentry(dentry);
			return 0;
		}
	}

	return -EBUSY;
}

/* @brief Get new dentry from pool
 *
 * @return Pointer to the new dentry
 * @retval NULL Pool is full
 */
struct dentry *dvfs_alloc_dentry(void) {
	struct dentry *dentry = pool_alloc(&dentry_pool);
	if (!dentry) {
		if (!dvfs_free_dentry(0)) {
			dentry = pool_alloc(&dentry_pool);
		} else {
			return NULL;
		}
	}

	memset(dentry, 0, sizeof(struct dentry));
	dentry_ref_inc(dentry);

	dlist_add_next(&dentry->d_lnk, &dentry_dlist);
	dlist_init(&dentry->children);

	return dentry;
}

extern int dvfs_cache_del(struct dentry *dentry);
/**
 * @brief Remove dentry from pool
 */
int dvfs_destroy_dentry(struct dentry *dentry) {
	assert(dentry->usage_count >= 0);
	if (dentry->usage_count == 0) {
		if (dentry->d_inode)
			dvfs_destroy_inode(dentry->d_inode);
		dentry_ref_dec(dentry->parent);
		dlist_del(&dentry->children_lnk);
		dlist_del(&dentry->d_lnk);
		dvfs_cache_del(dentry);
		pool_free(&dentry_pool, dentry);
		return 0;
	} else
		return -EBUSY;
}

/**
 * @brief Update dentry flags according to it's inode content
 *
 * @param dentry Pointer to dentry to be updated
 */
void dentry_upd_flags(struct dentry *dentry) {
	if (dentry->d_inode) {
		dentry->flags |= dentry->d_inode->flags & (S_IFMT | S_IRWXA);
	}
}

/* @brief Get new file descriptor from pool
 *
 * @return Pointer to the new file descriptor
 * @retval NULL Pool is full
 */
struct file *dvfs_alloc_file(void) {
	return pool_alloc(&file_pool);
}

/* @brief Remove file descriptor from pool
 */
int dvfs_destroy_file(struct file *desc) {
	pool_free(&file_pool, desc);
	return 0;
}

/* Fillers */
/* @brief Fills inode fields according to existing dentry and SB
 * @param inode      The structure to be filled
 * @param superblock The superblock of FS of the inode
 * @param dentry     The dentry related to inode
 */
int inode_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry) {
	inode->i_dentry = dentry;
	inode->i_sb     = sb;
	inode->i_ops    = sb ? sb->sb_iops : NULL;
	/* Other fields are left without changes on purpose */

	return 0;
}
/* @brief Fills dentry fields according to existing superblok,
 *        inode and prent dentry
 * @param superblock The superblock of FS of the dentry
 * @param inode      The inode related to dentry
 * @param dentry     The structure to be filled
 * @param parent     The parent of the denrty
 */
int dentry_fill(struct super_block *sb, struct inode *inode,
                      struct dentry *dentry, struct dentry *parent) {
	dentry->d_inode     = inode;
	dentry->d_sb        = sb;
	dentry->d_ops       = sb ? sb->sb_dops : NULL;
	dentry->parent      = parent;
	dentry->d_lnk       = dentry->d_lnk;
	dentry->usage_count = 1;

	if (inode) {
		inode->i_dentry = dentry;
	}

	dlist_init(&dentry->children);
	dlist_head_init(&dentry->children_lnk);

	if (parent) {
		dlist_add_prev(&dentry->children_lnk, &parent->children);
		dentry_ref_inc(parent);
	}
	return 0;
}

int dentry_ref_inc(struct dentry *dentry) {
	assert(dentry);
	return ++dentry->usage_count;
}

int dentry_ref_dec(struct dentry *dentry) {
	assert(dentry);
	assert(dentry->usage_count > 0);
	return --dentry->usage_count;
}

/* Root-related stuff */
static struct dentry *global_root = NULL;
extern struct super_block *rootfs_sb(void);
/* @brief Updating task resources realted to file system or
 *        initialize them if they are empty
 */
int dvfs_update_root(void) {
	struct super_block *sb;
	struct inode *inode;
	if (global_root == NULL)
		global_root = dvfs_alloc_dentry();

	assert(global_root);

	sb = rootfs_sb();
	inode = global_root->d_inode;
	if (inode == NULL)
		inode = dvfs_alloc_inode(sb);

	*global_root = (struct dentry) {
		.d_sb        = sb,
		.d_inode     = inode,
		.parent      = global_root,
		.name        = "/",
		.flags       = S_IFDIR | DVFS_DIR_VIRTUAL | DVFS_MOUNT_POINT,
		.usage_count = 1,
		.d_lnk       = global_root->d_lnk
	};

	if (global_root->d_inode)
		*(global_root->d_inode) = (struct inode) {
			.flags    = S_IFDIR,
			.i_ops    = sb->sb_iops,
			.i_sb     = sb,
			.i_dentry = global_root,
		};

	if (global_root->d_sb)
		global_root->d_sb->root = global_root;

	dlist_init(&global_root->children);
	dlist_init(&global_root->children_lnk);
	return 0;
}

/* @brief Global root getter
 * @return Pointer to the global VFS root
 */
struct dentry *dvfs_root(void) {
	if (!global_root) {
		dvfs_update_root();
	}

	return global_root;
}

/**
* @brief Check if element with given name presents as a subelement
*        of the folder in RAM.
*
* @param parent
* @param name
*
* @return Pointer to dentry if found or NULL if not
*/
struct dentry *local_lookup(struct dentry *parent, char *name) {
	struct dentry *d;
	struct dlist_head *l;

	dlist_foreach(l, &parent->children) {
		if (l == &parent->children)
			continue;
		d = mcast_out(l, struct dentry, children_lnk);

		if (!strcmp(d->name, name))
			return d;
	}

	return NULL;
}

/**
 * @brief Free superblock resources
 *
 * @param sb Superblock to be destroyed
 *
 * @return Negative error code or zero if succeed
 */
int dvfs_destroy_sb(struct super_block *sb) {
	/* TODO fs-specific resource free? */
	if (sb->root)
		sb->root->d_sb = NULL;

	pool_free(&superblock_pool, sb);
	return 0;
}

/**
 * @brief Read from block device pointed by bdev_file
 * with args similar to old-style bdev usage
 *
 * @param bdev_file File pointing to opened device
 * @param buff Buffer to read from
 * @param count Number of bytes to be read
 * @param blkno Number of block of device
 *
 * @return Size of read chunk or negative error number
 * if failed
 */
int dvfs_bdev_read(
		struct file *bdev_file,
		char *buff,
		size_t count,
		int blkno) {
	struct block_dev *bdev = bdev_file->f_inode->i_data;
	return block_dev_read(bdev, buff, count, blkno);
}

/**
 * @brief Write data to block device using args similar
 * to old-style bdev usage
 *
 * @param bdev_file File pointing to opened block device
 * @param buff Buffer to be written
 * @param count Number of bytes to be written
 * @param blkno Number of block of device
 *
 * @return Size of written chunk of negative error number
 * if failed
 */
int dvfs_bdev_write(
		struct file *bdev_file,
		char *buff,
		size_t count,
		int blkno) {
	struct block_dev *bdev = bdev_file->f_inode->i_data;
	return block_dev_write(bdev, buff, count, blkno);
}
