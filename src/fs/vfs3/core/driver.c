/**
 * @brief File system driver registry
 *
 * @date 02.01.24
 * @author Aleksey Zhmulin
 */

#include <stddef.h>
#include <string.h>

#include <lib/libds/array.h>
#include <vfs/core.h>

ARRAY_SPREAD_DEF(const struct vfs_driver *const, __vfs_drv_registry);

const struct vfs_driver *vfs_driver_find(const char *name) {
	const struct vfs_driver *drv;

	array_spread_foreach(drv, __vfs_drv_registry) {
		if (!strcmp(name, drv->name)) {
			return drv;
		}
	}

	return NULL;
}
