/**
 * @brief Common interface for flash subsystem
 *
 * @date 21.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#include <errno.h>
#include <string.h>

#include <framework/mod/options.h>
#include <drivers/flash/flash.h>
#include <mem/misc/pool.h>
#include <util/indexator.h>
#include <util/err.h>

#define MAX_DEV_QUANTITY OPTION_GET(NUMBER,dev_quantity)

POOL_DEF(flash_pool, struct flash_dev, MAX_DEV_QUANTITY);
INDEX_DEF(flash_idx, 0, MAX_DEV_QUANTITY);

static struct flash_dev *flashdev_tab[MAX_DEV_QUANTITY];

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
	if (idx < 0 || idx >= MAX_DEV_QUANTITY) {
		return NULL;
	}

	if (!flash_initialized) {
		flash_devs_init();
	}

	return flashdev_tab[idx];
}

int flash_max_id(void) {
	return MAX_DEV_QUANTITY;
}

ARRAY_SPREAD_DEF(const flash_dev_module_t, __flash_dev_registry);
int flash_devs_init(void) {
	int ret;
	const flash_dev_module_t *fdev_module;

	if (flash_initialized) {
		return 0;
	}

	flash_initialized = 1;

	/* TODO pass some device-specific data to init funcion? */
	array_spread_foreach_ptr(fdev_module, __flash_dev_registry) {
		if (fdev_module->init != NULL) {
			ret = fdev_module->init(NULL);
			if (ret != 0) {
				return ret;
			}
		}
	}

	return 0;
}

/* Handlers to check ranges and call device-specific funcions */
int flash_read(struct flash_dev *flashdev, unsigned long offset,
		void *buf, size_t len) {
	assert(buf);
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_read);

	assert(offset + len <= flashdev->size);

	return flashdev->drv->flash_read(flashdev, offset, buf, len);
}

int flash_write(struct flash_dev *flashdev, unsigned long offset,
		const void *buf, size_t len) {
	assert(buf);
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_program);

	assert(offset + len <= flashdev->size);

	return flashdev->drv->flash_program(flashdev, offset, buf, len);
}

int flash_erase(struct flash_dev *flashdev, uint32_t block) {
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_erase_block);

	return flashdev->drv->flash_erase_block(flashdev, block);
}

int flash_copy(struct flash_dev *flashdev, uint32_t to,
		uint32_t from, size_t len) {
	assert(flashdev);
	assert(flashdev->drv);
	assert(flashdev->drv->flash_copy);

	return flashdev->drv->flash_copy(flashdev, to, from, len);
}
