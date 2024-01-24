/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#include <string.h>

#include <embox/unit.h>
#include <fs/dentry.h>
#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/super_block.h>
#include <mem/misc/pool.h>
#include <lib/libds/array.h>

POOL_DEF(super_block_pool, struct super_block, OPTION_GET(NUMBER,fs_quantity));

struct super_block *super_block_alloc(const char *fs_type, const char *source) {
	struct super_block *sb;
	const struct fs_driver *drv;
	struct inode *node;

	drv = fs_driver_find(fs_type);
	if (NULL == drv) {
		return NULL;
	}

	if (NULL == (sb = pool_alloc(&super_block_pool))) {
		return NULL;
	}

	node = node_alloc("", 0);
	node->i_sb = sb;
	node->i_mode = S_IFDIR;

	*sb = (struct super_block) {
		.fs_drv = drv,
		.sb_root   = node,
	};

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

int super_block_free(struct super_block *sb) {
	int ret = 0;

	if (NULL == sb) {
		return EINVAL;
	}

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
