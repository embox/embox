/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 13.08.2022
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <drivers/clk.h>

#include "niiet_dma.h"
#include "niiet_dma_regs.h"

#include <config/board_config.h>

#include <framework/mod/options.h>

#define BASE_ADDR (0x50000000UL)

#define DMA ((volatile struct niiet_dma_regs *)BASE_ADDR)

int niiet_dma_get_type(char *type) {
    int num;

    if (0 == strncmp(type, DMA_TYPE_MEM, sizeof(DMA_TYPE_MEM) - 1)) {
		return MEMORY_DMA_REQUESTOR_IDX;
	}
	if (0 == strncmp(type, DMA_TYPE_GPIO, sizeof(DMA_TYPE_GPIO) - 1)) {
		num = type[sizeof(DMA_TYPE_GPIO) - 1] - 'A';
		return GPIO0_DMA_REQUESTOR_IDX + num;
	}
	if (0 == strncmp(type, DMA_TYPE_UART, sizeof(DMA_TYPE_UART) - 1)) {
		num = type[sizeof(DMA_TYPE_UART) - 1] - '0';
		return UART0_DMA_REQUESTOR_IDX + num;
	}
	if (0 == strncmp(type, DMA_TYPE_TMR, sizeof(DMA_TYPE_TMR) - 1)) {
		//num = DMA[sizeof(DMA_TYPE_TMR) - 1] - '0';
		num = atoi(&type[sizeof(DMA_TYPE_TMR) - 1]);
		return TMR0_DMA_REQUESTOR_IDX + num;
	}
	if (0 == strncmp(type, DMA_TYPE_SPI, sizeof(DMA_TYPE_SPI) - 1)) {
		num = type[sizeof(DMA_TYPE_SPI) - 1] - '0';
		return SPI0_DMA_REQUESTOR_IDX + num;
	}
	if (0 == strncmp(type, DMA_TYPE_I2C, sizeof(DMA_TYPE_I2C) - 1)) {
		num = type[sizeof(DMA_TYPE_I2C) - 1] - '0';
		return I2C0_DMA_REQUESTOR_IDX + num;
	}

	return -ENOSUPP;
}

int niiet_dma_init(uintptr_t label) {
	(void)label;

	clk_enable("DMA");

	return 0;
}

int niiet_dma_req(int ch, struct niiet_dma_req *req) {
	DMA->CH_ENABLE &= ~((uint32_t)(1) << ch);
	DMA->CH[0].STATIC0 = DMA_CH_STATIC0_RD_TOKENS(0x1);
	DMA->CH[0].STATIC1 = DMA_CH_STATIC1_WR_TOKENS(0x1);
	DMA->CH[0].STATIC4 = 0;

	DMA->CH_ENABLE |= ((uint32_t)(1) << ch);
	DMA->CH[0].STATIC0 |= DMA_CH_STATIC0_RD_BURST_MAX(req->dr_src_width)
	                      | DMA_CH_STATIC0_RD_INCR(req->dr_src_inc ? 1 : 0);
	DMA->CH[0].STATIC1 |= DMA_CH_STATIC1_WR_BURST_MAX(req->dr_dest_width)
	                      | DMA_CH_STATIC1_WR_INCR(req->dr_dest_inc ? 1 : 0);

	DMA->CH[0].STATIC4 = DMA_CH_STATIC4_RD_PER_NUM(req->dr_src_type)
	                     | DMA_CH_STATIC4_WR_PER_NUM(req->dr_dest_type);
	DMA->CH[0].SRC_PTR = req->dr_src;
	DMA->CH[0].DST_PTR = req->dr_dest;
	DMA->CH[0].NDTL = DMA_CH_NDTL_BUFFER_SIZE(req->dr_req_size);

	DMA->CH[0].CONFIG = DMA_CH_CONFIG_CMD_LAST(1) | DMA_CH_CONFIG_CMD_SET_INT(1);
	DMA->CH[0].INT_ENABLE = DMA_CH_INT_ENABLE_CH_END(1);

	DMA->CH[0].CH_ACTIVE = 1;
	DMA->CH[0].CH_START = 1;

	return 0;
}
