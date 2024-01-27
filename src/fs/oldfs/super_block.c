/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#include <util/log.h>

#include <stddef.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <mem/misc/pool.h>

#include <framework/mod/options.h>

#define SUPER_BLOCK_POOL_SZ           OPTION_GET(NUMBER,fs_quantity)

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

	sb = pool_alloc(&super_block_pool);
	if (sb == NULL) {
		return NULL;
	}

	memset(sb, 0, sizeof(*sb));

	sb->fs_drv = drv;

	node = node_alloc("", 0);
	if (!node) {
		pool_free(&super_block_pool, sb);
		return NULL;
	}

	node->i_sb = sb;
	node->i_mode = S_IFDIR;

	sb->sb_root = node;

	if (drv->fill_sb) {
		if (0 != drv->fill_sb(sb, source)) {
			node_free(node);
			pool_free(&super_block_pool, sb);
			return NULL;
		}
	}

	node->i_ops = sb->sb_iops;
	node->nas->fs = sb;

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
	int ret = 0;

	if (NULL == sb) {
		return EINVAL;
	}
	assert(sb->fs_drv);

	if (sb->fs_drv->clean_sb) {
		ret = sb->fs_drv->clean_sb(sb);
	}

	if (sb->sb_root) {
		/* Mount root should be generally
		 * freed on umount */
		node_free(sb->sb_root);
	}

	pool_free(&super_block_pool, sb);

	return ret;
}