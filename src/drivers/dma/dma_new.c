/**
 * @file
 * @brief
 *
 * @date    06.08.2026
 * @author  Anton Bondarev
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include <drivers/dma.h>
#include <lib/libds/array_spread.h>

int dma_config(struct dma_dev *dev, int ch, struct dma_config *conf) {
    int res;

    if (!dev || !dev->dd_ops) {
		return -EINVAL;
	}

    if (dev->dd_ops->do_config) {
        res = dev->dd_ops->do_config(dev, ch, conf);
        if (!res) {
            memcpy(&dev->dd_config[ch], conf, sizeof(dev->dd_config[ch]));
        }
    }

	return res;
}

int dma_transfer(struct dma_dev *dev, int ch, struct dma_req *req) {
    int res;

    if (!dev || !dev->dd_ops) {
		return -EINVAL;
	}

    if (dev->dd_ops->do_config) {
        res = dev->dd_ops->do_transfer(dev, ch, req);
    }

	return res;
}

int dma_activate(struct dma_dev *dev, uint32_t ch_mask) {
    int res;

    if (!dev || !dev->dd_ops) {
		return -EINVAL;
	}

    if (dev->dd_ops->do_activate) {
        res = dev->dd_ops->do_activate(dev, ch_mask);
    }

	return res;
}

int dma_get_type(struct dma_dev *dev, char *type) {
    int res;

    if (!dev || !dev->dd_ops) {
		return -EINVAL;
	}

    if (dev->dd_ops->do_get_type) {
        res = dev->dd_ops->do_get_type(dev, type);
    }

	return res;
}
