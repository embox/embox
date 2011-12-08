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
#include <linux/init.h>
#include <mem/objalloc.h>
#include <embox/unit.h>
#include <util/array.h>
#include <fs/fs.h>
#include <fs/driver_registry.h>

EMBOX_UNIT_INIT(unit_init);

OBJALLOC_DEF(fs_driver_pool, struct fs_driver_head, CONFIG_MAX_FS_DRIVERS);

ARRAY_SPREAD_DEF(const fs_drv_t *, __fs_drivers_registry);

static LIST_HEAD(file_systems);

#define drv_to_head(fs_drv) (uint32_t)(fs_drv - offsetof(fs_driver_head_t, drv))


static fs_driver_head_t *filesystem_alloc(fs_drv_t *drv) {
	fs_driver_head_t *head;

	head = objalloc(&fs_driver_pool);
	head->drv = drv;
	list_add((struct list_head*) head, &file_systems);

	return head;
}

static void filesystem_free(fs_drv_t *drv) {
	fs_driver_head_t *head = (fs_driver_head_t *)drv_to_head(drv);

	list_del((struct list_head*) head);
	objfree(&fs_driver_pool, head);
	return;
}

static int __init unit_init(void) {
	fs_driver_head_t *head;
	size_t i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__fs_drivers_registry); i++) {
		if (NULL == (head = filesystem_alloc(
				(fs_drv_t *) __fs_drivers_registry[i]))) {
			return 0;
		}

		__fs_drivers_registry[i]->fsop->init(NULL);
	}

	return ENOERR;
}

fs_drv_t *filesystem_find_drv(const char *name) {
	struct list_head *p;
	list_for_each(p, &file_systems) {
		if (0 == strcmp(((fs_driver_head_t *) p)->drv->name, name)) {
			return ((fs_driver_head_t *) p)->drv;
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
//	if (NULL == filesystem_alloc(fs)) {
//		return -EBUSY;
//	}

	return res;
}

int filesystem_unregister_drv(fs_drv_t *fs) {
	if (NULL == fs) {
		return -EINVAL;
	}
	filesystem_free(fs);

	return ENOERR;
}
