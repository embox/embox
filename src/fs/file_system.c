/**
 * @file
 * @brief
 *
 * @date 26.11.2012
 * @author Andrey Gazukin
 */

#include <string.h>
#include <embox/unit.h>
#include <util/array.h>
#include <util/dlist.h>

#include <fs/file_system.h>
#include <fs/fs_driver.h>

#include <mem/misc/pool.h>

POOL_DEF(filesystem_pool, struct filesystem, OPTION_GET(NUMBER,fs_quantity));

static DLIST_DEFINE(fs_list);

struct filesystem *filesystem_alloc(const char *drv_name) {
	struct filesystem *fs;

	fs = pool_alloc(&filesystem_pool);
	if (NULL == fs) {
		return NULL;
	}

	if (0 != *drv_name) {
		fs->drv = fs_driver_find_drv(drv_name);
	}
	if(0 != strcmp(drv_name, "empty")) {
		dlist_add_next(dlist_head_init((struct dlist_head*)&fs->link), &fs_list);
	}

	return fs;
}

void filesystem_free(struct filesystem *fs) {
	if (NULL == fs) {
		return;
	}
	dlist_del((struct dlist_head *)&fs->link);
	pool_free(&filesystem_pool, fs);
}

int filesystem_get_mount_list(char *buff) {
	struct dlist_head *tmp;
	struct dlist_head *fs_link;
	struct filesystem *fs;
	*buff = 0;

	dlist_foreach(fs_link, tmp, &fs_list) {
		fs = member_cast_out(fs_link, struct filesystem, link);

		strcat(buff, fs->mntfrom);
		strcat(buff, " on ");
		strcat(buff, fs->mntto);
		strcat(buff, " type ");
		strcat(buff, fs->drv->name);
		strcat(buff, "\n");
	}
	return 0;
}

