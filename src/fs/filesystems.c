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
#include <embox/unit.h>
#include <util/array.h>
#include <fs/fs.h>

EMBOX_UNIT_INIT(unit_init);

typedef struct fs_driver_head {
	struct list_head *next;
	struct list_head *prev;
	fs_drv_t *drv;
} fs_driver_head_t;

ARRAY_SPREAD_DEF(const fs_drv_t *, __fs_drivers_registry);

static fs_driver_head_t pool[CONFIG_MAX_FS_DRIVERS];
static LIST_HEAD(free_list);
static LIST_HEAD(file_systems);

#define drv_to_head(fs_drv) (uint32_t)(fs_drv - offsetof(fs_driver_head_t, drv))

static void init_pool(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pool); i++) {
		list_add((struct list_head *) &pool[i], &free_list);
	}
}

static fs_driver_head_t *filesystem_alloc(fs_drv_t *drv) {
	fs_driver_head_t *head;
	if (list_empty(&free_list)) {
		return NULL;
	}
	head = (fs_driver_head_t *) free_list.next;
	head->drv = drv;
	list_move((struct list_head*) head, &file_systems);

	return head;
}

static void filesystem_free(fs_drv_t *drv) {
	list_move((struct list_head*) drv_to_head(drv), &free_list);
	return;
}

static int __init unit_init(void) {
	fs_driver_head_t *head;
	fs_drv_t *root_fs;
	size_t i;

	init_pool();
	for (i = 0; i < ARRAY_SPREAD_SIZE(__fs_drivers_registry); i++) {
		if (NULL == (head = filesystem_alloc(
				(fs_drv_t *) __fs_drivers_registry[i]))) {
			return 0;
		}
		__fs_drivers_registry[i]->fsop->init(NULL);
	}

	if (NULL == (root_fs = filesystem_find_drv("rootfs"))) {
		printk("File systems not found rootfs driver\n");
	} else {
		root_fs->fsop->mount(NULL);
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
	if (NULL == filesystem_alloc(fs)) {
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
