/**
 * @file
 *
 * @date 06.10.2010
 * @author Anton Bondarev
 */

#include <linux/init.h>
#include <embox/kernel.h>
#include <embox/unit.h>
#include <lib/list.h>
#include <fs/fs.h>

typedef struct fs_driver_head {
	struct list_head     *next;
	struct list_head     *prev;
	file_system_driver_t  drv;
} fs_driver_head_t;

static fs_driver_head_t pool[CONFIG_MAX_FS_DRIVERS];
static LIST_HEAD(free_list);

#define drv_to_head(fs_drv) (uint32_t)(fs_drv - offsetof(fs_driver_head_t, drv))

EMBOX_UNIT_INIT(unit_init);

static int __init unit_init(void) {
	size_t i;
	for (i = 0; i < ARRAY_SIZE(pool); i++) {
		list_add((struct list_head *)&pool[i], &free_list);
	}
	return 0;
}

file_system_driver_t *alloc_fs_drivers(void) {
	file_system_driver_t *drv;
	if (list_empty(&free_list)) {
		return NULL;
	}
	drv = &(((fs_driver_head_t *)(free_list.next))->drv);

	list_del_init(&free_list);

	return drv;
}

void free_fs_drivers(file_system_driver_t *fs_drv) {
	if (NULL == fs_drv) {
		return;
	}
	list_add((struct list_head *) drv_to_head(fs_drv), &free_list);
}

