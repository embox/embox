/**
 * @file
 * @brief table of configured filesystems
 *
 * @date 05.10.2010
 * @author Nikolay Korotky
 */

#include <fs/fs.h>
#include <errno.h>

static file_system_type *file_systems;

static file_system_type **find_filesystem(const char *name, unsigned len) {
	file_system_type **p;
	for(p = &file_systems; *p; p = &(*p)->next) {
		if (strlen((*p)->name) == len &&
		    strncmp((*p)->name, name, len) == 0)
			break;
	}
	return p;
}

int register_filesystem(file_system_type *fs) {
	int res = 0;
	file_system_type **p;
	if (fs->next)
		return -EBUSY;
	INIT_LIST_HEAD(&fs->fs_supers);
	p = find_filesystem(fs->name, strlen(fs->name));
	if (*p)
		res = -EBUSY;
	else
		*p = fs;
	return res;
}

int unregister_filesystem(file_system_type *fs) {
	file_system_type **tmp;

	tmp = &file_systems;
	while (*tmp) {
		if (fs == *tmp) {
			*tmp = fs->next;
			fs->next = NULL;
			return 0;
		}
		tmp = &(*tmp)->next;
	}
        return -EINVAL;
}

