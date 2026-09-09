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

#define DMA_TYPE_MEM      "DMA_MEM"
#define DMA_TYPE_GPIO     "DMA_GPIO"
#define DMA_TYPE_UART     "DMA_UART"
#define DMA_TYPE_SPI      "DMA_SPI"
#define DMA_TYPE_I2C      "DMA_I2C"
#define DMA_TYPE_TMR      "DMA_TMR"

struct niiet_dma_req;

struct niiet_dma_req {
	uintptr_t dr_src;
	int       dr_src_width;
	int       dr_src_inc;
	int       dr_src_type;
	uintptr_t dr_dest;
	int       dr_dest_width;
	int       dr_dest_inc;
	int       dr_dest_type;
	int       dr_req_size;

	uint32_t  dr_flags;
	uint32_t  dr_status;
	int     (*dr_callback)(struct niiet_dma_req *req, void *data, int res);

};

extern int niiet_dma_init(uintptr_t label, int ch);

extern int niiet_dma_req(int ch, struct niiet_dma_req *conf);
extern int niiet_dma_get_type(char *type);

#endif /* DRIVERS_DMA_NIIET_DMA_H_ */
