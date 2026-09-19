/**
 * @file
 * @brief
 *
 * @date    06.08.2026
 * @author  Anton Bondarev
 */

#include <assert.h>
#include <string.h>

#include <drivers/dma.h>
#include <embox/unit.h>
#include <lib/libds/array_spread.h>
#include <util/log.h>

ARRAY_SPREAD_DEF(struct dma_dev *, __dma_device_registry);

EMBOX_UNIT_INIT(dma_subsystem_init);

struct dma_dev *dma_dev_by_id(int id) {
	struct dma_dev *dev;

	array_spread_foreach(dev, __dma_device_registry) {
		if (dev->dd_idx == id) {
			return (struct dma_dev *)dev;
		}
	}

	return NULL;
}

static int dma_subsystem_init(void) {
	struct dma_dev *dev;

	array_spread_foreach(dev, __dma_device_registry) {
		if (!dev->dd_ops) {
			continue;
		}
		if (!dev->dd_ops->do_init) {
			continue;
		}
		if (dev->dd_ops->do_init(dev)) {
			dev->dd_ops->do_init(dev);
		}
	}

	return 0;
}
