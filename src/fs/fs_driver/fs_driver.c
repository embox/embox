/**
 * @file
 * @brief table of configured filesystems
 *
 * @date 05.10.10
 * @author Nikolay Korotky
 */

#include <util/log.h>

#include <string.h>

#include <fs/fs_driver.h>
#include <lib/libds/array.h>

ARRAY_SPREAD_DEF(const struct fs_driver *const, fs_drivers_registry);

const struct fs_driver *fs_driver_find(const char *name) {
	const struct fs_driver *fs_drv;

	log_debug("looking for %s there are %d drvs", name, ARRAY_SPREAD_SIZE(fs_drivers_registry));
	array_spread_foreach(fs_drv, fs_drivers_registry) {
		log_debug("checking %s", fs_drv->name);
		if (!strcmp(name, fs_drv->name)) {
			log_debug("found %s", fs_drv->name);
			return fs_drv;
		}
	}
	log_debug("did not find %s", name);
	return NULL;
}
