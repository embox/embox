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

struct super_block *super_block_alloc(const char *fs_type, const char *source) {
	struct super_block *sb;
	const struct fs_driver *drv;

	drv = fs_driver_find(fs_type);
	if (NULL == drv) {
		return NULL;
	}

	if (NULL == (sb = pool_alloc(&super_block_pool))) {
		return NULL;
	}

	*sb = (struct super_block) {
		.fs_drv    = drv,
	};

	if (drv->fill_sb) {
		if (0 != drv->fill_sb(sb, source)) {
			pool_free(&super_block_pool, sb);
			return NULL;
		}
	}

	return sb;
}

void super_block_free(struct super_block *sb) {
	if (NULL == sb) {
		return;
	}

	pool_free(&super_block_pool, sb);
}
