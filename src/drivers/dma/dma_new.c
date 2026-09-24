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
        dev->dd_status[ch_mask] |= DMA_CH_STATUS_BUSY;
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

int dma_init(struct dma_dev *dev) {
    int res;

    if (!dev || !dev->dd_ops) {
		return -EINVAL;
	}

    if (dev->dd_ops->do_init) {
        res = dev->dd_ops->do_init(dev);
    }

	return res;
}

int dma_complite(struct dma_dev *dev, struct dma_req *req, int res) {

    if (req && req->dr_callback) {
        req->dr_callback(req, req->dr_data, res);
    }

    dev->dd_status[req->dr_chan] &= ~DMA_CH_STATUS_BUSY;

    return 0;
}

uint32_t dma_get_status(struct dma_dev *dev, int ch) {
    return dev->dd_status[ch];
}

uint32_t dma_is_ch_busy(struct dma_dev *dev, int ch) {
    return (dev->dd_status[ch] & DMA_CH_STATUS_BUSY) ? 1 : 0;
}
