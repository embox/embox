/**
 * @file
 * @brief table of configured filesystems
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <embox/unit.h>

#include <mem/misc/pool.h>
#include <util/dlist.h>

#include <util/array.h>
#include <fs/fs_drv.h>

EMBOX_UNIT_INIT(unit_init);

typedef struct fs_driver_head {
	struct dlist_head link;
	fs_drv_t *drv;
} fs_driver_head_t;

POOL_DEF(fs_driver_pool, struct fs_driver_head, OPTION_GET(NUMBER,drivers_quantity));

ARRAY_SPREAD_DEF(const fs_drv_t *, __fs_drivers_registry);

static DLIST_DEFINE(file_systems);


static fs_driver_head_t *filesystem_alloc(fs_drv_t *drv) {
	fs_driver_head_t *head;

	head = pool_alloc(&fs_driver_pool);
	head->drv = drv;

	dlist_add_next(dlist_head_init((struct dlist_head*)head), &file_systems);

	return head;
}

static void filesystem_free(fs_drv_t *drv) {
	fs_driver_head_t *head = (fs_driver_head_t *)drv;

	dlist_del((struct dlist_head *)head);
	pool_free(&fs_driver_pool, head);
	return;
}

static int unit_init(void) {
	fs_driver_head_t *head;
	size_t i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__fs_drivers_registry); i++) {
		if (NULL == (head = filesystem_alloc(
				(fs_drv_t *) __fs_drivers_registry[i]))) {
			return -EINVAL;
		}

		__fs_drivers_registry[i]->fsop->init(NULL);
	}

	return ENOERR;
}

fs_drv_t *filesystem_find_drv(const char *name) {
	struct fs_driver_head *tmp;
	struct fs_driver_head *fs_drv;

	dlist_foreach_entry(fs_drv, tmp, &file_systems, link) {
		if (0 == strcmp(fs_drv->drv->name, name)) {
			return fs_drv->drv;
		}
	}
	return NULL;
}

int filesystem_register_drv(fs_drv_t *fs) {
	int res = 0;
	fs_drv_t *p;

	if (NULL == fs) {
		return EINVAL;
	}

	p = filesystem_find_drv(fs->name);
	if (NULL != p) {
		return -EBUSY;
	}

	return res;
}

int filesystem_unregister_drv(fs_drv_t *fs) {
	if (NULL == fs) {
		return -EINVAL;
	}
	filesystem_free(fs);

	return ENOERR;
}
