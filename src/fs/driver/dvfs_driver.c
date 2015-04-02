/**
 * @file
 * @brief
 *
 * @date 2 Apr 2015
 * @author Denis Deryugin
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <embox/unit.h>

#include <fs/dvfs.h>
#include <mem/misc/pool.h>
#include <util/dlist.h>
#include <util/array.h>
#include <util/member.h>

struct dvfs_driver_head {
	struct dlist_head link;
	struct dumb_fs_driver *drv;
};

ARRAY_SPREAD_DEF(const struct dumb_fs_driver, dumb_drv_tab);

struct dumb_fs_driver *dumb_fs_driver_find(const char *name) {
	struct dumb_fs_driver *fs_drv;
	array_spread_foreach_ptr(fs_drv, dumb_drv_tab){
		if (!strcmp(name, fs_drv->name)) {
			return fs_drv;
		}
	}
	return NULL;
}

