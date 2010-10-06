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

static LIST_HEAD(file_systems);

file_system_driver_t *find_filesystem(const char *name) {
	struct list_head *p;
	list_for_each(p, &file_systems) {
		if(0 == strcmp(((file_system_driver_t *)p)->name, name)) {
			return (file_system_driver_t *)p;
		}
	}
	return NULL;
}

int register_filesystem(file_system_driver_t *fs) {
	int res = 0;
	file_system_driver_t *p;

	p = find_filesystem(fs->name);
	if (NULL != p) {
		return -EBUSY;
	}
	list_add((struct list_head *)fs, &file_systems);
	TRACE("register %s\n", fs->name);
	return res;
}

int unregister_filesystem(file_system_driver_t *fs) {
	if(NULL == fs) {
		return -EINVAL;
	}
	list_del_init((struct list_head *)fs);
	return -EINVAL;
}

