/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#include <string.h>
#include <embox/unit.h>
#include <fs/file_system.h>
#include <fs/fs_driver.h>
#include <util/array.h>
#include <mem/misc/pool.h>

POOL_DEF(filesystem_pool, struct filesystem, OPTION_GET(NUMBER,fs_quantity));

static struct filesystem *filesystem_alloc(void) {

	return pool_alloc(&filesystem_pool);
}

struct filesystem *filesystem_create(const char *drv_name) {
	struct filesystem *fs;

	if (NULL == (fs = filesystem_alloc())) {
		return NULL;
	}

	if (0 != *drv_name) {
		fs->drv = fs_driver_find_drv(drv_name);
	}

	return fs;
}

void filesystem_free(struct filesystem *fs) {
	if (NULL == fs) {
		return;
	}

	pool_free(&filesystem_pool, fs);
}

