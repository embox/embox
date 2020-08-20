/**
 * @file
 * @brief
 *
 * @date    26.07.2020
 * @author  Alexander Kalmuk
 */

#ifndef DRIVERS_DMA_DMA_H_
#define DRIVERS_DMA_DMA_H_

#include <stdint.h>

extern int dma_config(int dma_chan);
extern int dma_transfer(int dma_chan, uint32_t dst, uint32_t src, int words);
extern int dma_in_progress(int dma_chan);

#endif /* DRIVERS_DMA_DMA_H_ */
