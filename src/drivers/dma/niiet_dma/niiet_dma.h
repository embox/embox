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

#include <drivers/dma.h>

#if 0
extern int niiet_dma_init(struct dma_dev *dev);

extern int niiet_dma_config(struct dma_dev *dev, int ch, struct dma_config *conf);

extern int niiet_dma_transfer(struct dma_dev *dev, int ch, struct dma_req *req);
extern int niiet_dma_get_type(struct dma_dev *dev, char *type);

extern int niiet_dma_activate(struct dma_dev *dev, uint32_t ch);
extern int niiet_dma_wait(int ch);
#endif

#endif /* DRIVERS_DMA_NIIET_DMA_H_ */
