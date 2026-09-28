/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#ifndef DRIVERS_DMA_NIIET_DMA_H_
#define DRIVERS_DMA_NIIET_DMA_H_


#include <stdint.h>

#include <config/board_config.h>

#include <drivers/dma.h>

struct dma_req;

struct niiet_dma_priv {
	struct dma_req *req[CONF_DMA0_MISC_CHAN_NUM];
};

#endif /* DRIVERS_DMA_NIIET_DMA_H_ */
