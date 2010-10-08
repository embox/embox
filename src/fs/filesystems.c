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
}fs_driver_head_t;

static int unit_init(void) {
	size_t i;

//	for (i = 0; i < NUMBER_OF_FS; i++) {
//		if ((NULL == fs_list[i].fsop) || (NULL == fs_list[i].fsop ->init)) {
//			LOG_ERROR("fs with id has wrong operations desc\n");
//			continue;
//		}
//		register_filesystem(alloc_fs_drivers());
//	}

	return 0;
}

static LIST_HEAD(file_systems);

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
//	file_system_driver_t *p;
//
//	p = find_filesystem(fs->name);
//	if (NULL != p) {
//		return -EBUSY;
//	}
//	list_add((struct list_head *)fs, &file_systems);
//	TRACE("register %s\n", fs->name);
	return res;
}

int unregister_filesystem(file_system_driver_t *fs) {
//	if(NULL == fs) {
//		return -EINVAL;
//	}
//	list_del_init((struct list_head *)fs);
	return -EINVAL;
}

