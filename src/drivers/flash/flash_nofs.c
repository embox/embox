/**
 * @brief Stub interface for flash subsystem.
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#include <drivers/flash/flash.h>

/* Devices are not registered anywhere, so we just
 * call common functions to create and delete devices */

struct flash_dev *flash_create(const char *path, size_t size) {
	return flash_alloc();
}

int flash_delete(struct flash_dev *dev) {
	return flash_free(dev);
}

/* XXX we have to do it because abstract modules
 * don't support unit init */
#include <embox/unit.h>
EMBOX_UNIT_INIT(_flash_devs_init);
static int _flash_devs_init(void) {
	return flash_devs_init();
}
