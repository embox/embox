/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#include <string.h>
#include <embox/unit.h>
#include <fs/super_block.h>
#include <fs/fs_driver.h>
#include <util/array.h>
#include <mem/misc/pool.h>

POOL_DEF(super_block_pool, struct super_block, OPTION_GET(NUMBER,fs_quantity));

struct super_block *super_block_alloc(const char *drv_name, struct block_dev *bdev) {
	struct super_block *sb;

	if (NULL == (sb = pool_alloc(&super_block_pool))) {
		return NULL;
	}

	memset(sb, 0, sizeof(*sb));

	if (0 != *drv_name) {
		sb->fs_drv = fs_driver_find(drv_name);
	}

	if (sb && sb->fs_drv) {
		sb->sb_fops = sb->fs_drv->file_op;
	}

	sb->bdev = bdev;

	return sb;
}

void super_block_free(struct super_block *sb) {
	if (NULL == sb) {
		return;
	}

	pool_free(&super_block_pool, sb);
}
