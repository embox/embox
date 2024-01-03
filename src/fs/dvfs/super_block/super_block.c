/**
 * @file
 *
 * @date Jul 21, 2021
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <sys/stat.h>

#include <framework/mod/options.h>
#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#define SUPER_BLOCK_POOL_SZ OPTION_GET(NUMBER, super_block_pool_sz)

POOL_DEF(super_block_pool, struct super_block, SUPER_BLOCK_POOL_SZ);

/**
 * @brief Try to allocate superblock using file system driver and given device
 * @param drv Name of file system driver
 * @param dest Path to device (e.g. /dev/sda1)
 *
 * @return Pointer to the new superblock
 * @retval NULL Superblock could not be allocated
 */
struct super_block *super_block_alloc(const char *fs_type, const char *source) {
	struct super_block *sb;
	const struct fs_driver *drv;
	struct inode *node;

	assert(fs_type);

	drv = fs_driver_find(fs_type);
	if (NULL == drv) {
		return NULL;
	}

	if (NULL == (sb = pool_alloc(&super_block_pool))) {
		return NULL;
	}

	*sb = (struct super_block){
	    .fs_drv = drv,
	};

	node = dvfs_alloc_inode(sb);
	if (!node) {
		super_block_free(sb);
		return NULL;
	}
	*node = (struct inode){
	    .i_mode = S_IFDIR,
	    .i_sb = sb,
	};

	sb->sb_root = node;

	if (drv->fill_sb) {
		if (0 != drv->fill_sb(sb, source)) {
			dvfs_destroy_inode(node);
			pool_free(&super_block_pool, sb);
			return NULL;
		}
	}

	node->i_ops = sb->sb_iops;

	return sb;
}

/**
 * @brief Free superblock resources
 *
 * @param sb Superblock to be destroyed
 *
 * @return Negative error code or zero if succeed
 */
int super_block_free(struct super_block *sb) {
	int err = 0;

	assert(sb);
	assert(sb->fs_drv);

	if (sb->fs_drv->clean_sb) {
		err = sb->fs_drv->clean_sb(sb);
	}

	pool_free(&super_block_pool, sb);

	return err;
}
