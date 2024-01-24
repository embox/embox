/**
 * @brief Common interface for flash subsystem
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#include <errno.h>
#include <string.h>

#include <drivers/block_dev.h>
#include <drivers/flash/flash.h>

#include <mem/misc/pool.h>
#include <util/err.h>
#include <lib/libds/indexator.h>

#include <framework/mod/options.h>

#define MAX_FLASHDEV_QUANTITY OPTION_GET(NUMBER, dev_quantity)

POOL_DEF(flash_pool, struct flash_dev, MAX_FLASHDEV_QUANTITY);
INDEX_DEF(flash_idx, 0, MAX_FLASHDEV_QUANTITY);

static struct flash_dev *flashdev_tab[MAX_FLASHDEV_QUANTITY];

struct flash_dev *flash_alloc(void) {
	struct flash_dev *flash;
	int idx;

	if (NULL == (flash = pool_alloc(&flash_pool))) {
		return err_ptr(ENOMEM);
	}

	idx = index_alloc(&flash_idx, INDEX_MIN);
	if (idx == INDEX_NONE) {
		pool_free(&flash_pool, flash);
		return err_ptr(ENOMEM);
	}

	memset(flash, 0, sizeof(*flash));
	flash->idx = idx;

	flashdev_tab[idx] = flash;

	return flash;
}

int flash_free(struct flash_dev *dev) {
	assert(dev);

	index_free(&flash_idx, dev->idx);
	flashdev_tab[dev->idx] = 0;
	pool_free(&flash_pool, dev);

	return 0;
}

static int flash_initialized = 0;
struct flash_dev *flash_by_id(int idx) {
	if (idx < 0 || idx >= MAX_FLASHDEV_QUANTITY) {
		return NULL;
	}

	if (!flash_initialized) {
		flash_devs_init();
	}

	return flashdev_tab[idx];
}

int flash_max_id(void) {
	return MAX_FLASHDEV_QUANTITY;
}

ARRAY_SPREAD_DEF(const struct flash_dev_module, __flash_dev_registry);
int flash_devs_init(void) {
	int ret;
	const struct flash_dev_module *fdev_module;

	if (flash_initialized) {
		return 0;
	}

	flash_initialized = 1;

	/* TODO pass some device-specific data to init funcion? */
	array_spread_foreach_ptr(fdev_module, __flash_dev_registry) {
		if (fdev_module->dev_drv->flash_init != NULL) {
			ret = fdev_module->dev_drv->flash_init(NULL, NULL);
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}
