/**
 * @file
 * @brief table of configured filesystems
 *
 * @date 05.10.2010
 * @author Nikolay Korotky
 */

#include <fs/fs.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

typedef struct fs_driver_head {
	struct list_head *next;
	struct list_head *prev;
	file_system_driver_t *drv;
} fs_driver_head_t;

static fs_driver_head_t pool[CONFIG_MAX_FS_DRIVERS];
static LIST_HEAD(free_list);
static LIST_HEAD(file_systems);

#define drv_to_head(fs_drv) (uint32_t)(fs_drv - offsetof(fs_driver_head_t, drv))

static void init_pool(void) {
	int i;
	for(i = 0; i < ARRAY_SIZE(pool); i ++) {
		list_add((struct list_head *)&pool[i], &free_list);
	}
}

static fs_driver_head_t *alloc(file_system_driver_t *drv) {
	fs_driver_head_t *head;
	if(list_empty(&free_list)) {
		return NULL;
	}
	head = (fs_driver_head_t *)free_list.next;
	head->drv = drv;
	list_move((struct list_head*)head, &file_systems);
	TRACE("register %s\n", drv->name);
	return head;
}

static void free(file_system_driver_t *drv) {
	list_move((struct list_head*)drv_to_head(drv), &free_list);
	TRACE("unregister %s\n", drv->name);
	return;
}

static int unit_init(void) {
	extern file_system_driver_t *_drivers_fs_start, *_drivers_fs_end;
	file_system_driver_t **drv = &_drivers_fs_start;
	fs_driver_head_t *head;
	file_system_driver_t *root_fs;

	init_pool();

	for (; ((uint32_t)drv) < (uint32_t)&_drivers_fs_end; drv ++) {
		if (NULL == (head = alloc(*drv))) {
			return 0;
		}
	}

	if (NULL == (root_fs = find_filesystem("rootfs"))) {
		TRACE("File systems not found rootfs driver\n");
	}
	else {
		root_fs->fsop->init();
	}

	return ENOERR;
}

file_system_driver_t *find_filesystem(const char *name) {
	struct list_head *p;
	list_for_each(p, &file_systems) {
		if(0 == strcmp(((fs_driver_head_t *)p)->drv->name, name)) {
			return ((fs_driver_head_t *)p)->drv;
		}
	}
	return NULL;
}

int register_filesystem(file_system_driver_t *fs) {
	int res = 0;
	file_system_driver_t *p;

	if (NULL == fs) {
		return EINVAL;
	}

	p = find_filesystem(fs->name);
	if (NULL != p) {
		return -EBUSY;
	}
	if(NULL == alloc(fs)) {
		return -EBUSY;
	}

	return res;
}

int unregister_filesystem(file_system_driver_t *fs) {
	if(NULL == fs) {
		return -EINVAL;
	}
	free(fs);

	return ENOERR;
}

