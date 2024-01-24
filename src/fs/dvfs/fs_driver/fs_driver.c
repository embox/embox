/**
 * @file
 * @brief table of configured filesystems
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#include <string.h>

#include <fs/fs_driver.h>
#include <lib/libds/array.h>
#include <util/member.h>

ARRAY_SPREAD_DEF(const struct fs_driver *const, fs_drivers_registry);

const struct fs_driver *fs_driver_find(const char *name) {
	const struct fs_driver *fs_drv;

	array_spread_foreach(fs_drv, fs_drivers_registry) {
		if (!strcmp(name, fs_drv->name)) {
			return fs_drv;
		}
	}

	return NULL;
}
