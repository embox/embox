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

#include <kernel/irq.h>

#include <drivers/clk.h>
#include <drivers/niiet_dma.h>

#include "niiet_dma_regs.h"

#include <config/board_config.h>

#include <framework/mod/options.h>


#define DMA_DEV_ID     0

#define CONF_DMA                  MACRO_CONCAT(CONF_DMA,DMA_DEV_ID)

#define DMA_BASE_ADDR        MACRO_CONCAT(CONF_DMA,_REGION_BASE_ADDR)
#define DMA_CLK_NAME         MACRO_CONCAT(CONF_DMA,_CLK_ENABLE)

#define DMA ((volatile struct niiet_dma_regs *)DMA_BASE_ADDR)

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

static inline int niiet_dma_ch_to_irq(int ch) {
	switch(ch) {
#if defined CONF_DMA0_IRQ_CHAN0
		case 0:
			return CONF_DMA0_IRQ_CHAN0;
#endif /* CONF_DMA0_IRQ_CHAN0 */
#if defined CONF_DMA0_IRQ_CHAN1
		case 1:
			return CONF_DMA0_IRQ_CHAN1;
#endif /* CONF_DMA0_IRQ_CHAN1 */
#if defined CONF_DMA0_IRQ_CHAN2
		case 2:
			return CONF_DMA0_IRQ_CHAN2;
#endif /* CONF_DMA0_IRQ_CHAN2 */
#if defined CONF_DMA0_IRQ_CHAN3
		case 3:
			return CONF_DMA0_IRQ_CHAN3;
#endif /* CONF_DMA0_IRQ_CHAN3 */
#if defined CONF_DMA0_IRQ_CHAN4
		case 4:
			return CONF_DMA0_IRQ_CHAN4;
#endif /* CONF_DMA0_IRQ_CHAN4 */
#if defined CONF_DMA0_IRQ_CHAN5
		case 5:
			return CONF_DMA0_IRQ_CHAN5;
#endif /* CONF_DMA0_IRQ_CHAN5 */
#if defined CONF_DMA0_IRQ_CHAN6
		case 6:
			return CONF_DMA0_IRQ_CHAN6;
#endif /* CONF_DMA0_IRQ_CHAN6 */
#if defined CONF_DMA0_IRQ_CHAN7
		case 7:
			return CONF_DMA0_IRQ_CHAN7;
#endif /* CONF_DMA0_IRQ_CHAN7 */
#if defined CONF_DMA0_IRQ_CHAN8
		case 8:
			return CONF_DMA0_IRQ_CHAN8;
#endif /* CONF_DMA0_IRQ_CHAN8 */
#if defined CONF_DMA0_IRQ_CHAN9
		case 9:
			return CONF_DMA0_IRQ_CHAN9;
#endif /* CONF_DMA0_IRQ_CHAN9 */
#if defined CONF_DMA0_IRQ_CHAN10
		case 10:
			return CONF_DMA0_IRQ_CHAN10;
#endif /* CONF_DMA0_IRQ_CHAN10 */
#if defined CONF_DMA0_IRQ_CHAN11
		case 11:
			return CONF_DMA0_IRQ_CHAN11;
#endif /* CONF_DMA0_IRQ_CHAN11 */
#if defined CONF_DMA0_IRQ_CHAN12
		case 12:
			return CONF_DMA0_IRQ_CHAN12;
#endif /* CONF_DMA0_IRQ_CHAN12 */
#if defined CONF_DMA0_IRQ_CHAN13
		case 13:
			return CONF_DMA0_IRQ_CHAN13;
#endif /* CONF_DMA0_IRQ_CHAN13 */
#if defined CONF_DMA0_IRQ_CHAN14
		case 14:
			return CONF_DMA0_IRQ_CHAN14;
#endif /* CONF_DMA0_IRQ_CHAN14 */
#if defined CONF_DMA0_IRQ_CHAN15
		case 15:
			return CONF_DMA0_IRQ_CHAN15;
#endif /* CONF_DMA0_IRQ_CHAN15 */
#if defined CONF_DMA0_IRQ_CHAN16
		case 16:
			return CONF_DMA0_IRQ_CHAN16;
#endif /* CONF_DMA0_IRQ_CHAN16 */
#if defined CONF_DMA0_IRQ_CHAN17
		case 17:
			return CONF_DMA0_IRQ_CHAN17;
#endif /* CONF_DMA0_IRQ_CHAN17 */
#if defined CONF_DMA0_IRQ_CHAN18
		case 18:
			return CONF_DMA0_IRQ_CHAN18;
#endif /* CONF_DMA0_IRQ_CHAN18 */
#if defined CONF_DMA0_IRQ_CHAN19
		case 19:
			return CONF_DMA0_IRQ_CHAN19;
#endif /* CONF_DMA0_IRQ_CHAN19 */
#if defined CONF_DMA0_IRQ_CHAN20
		case 20:
			return CONF_DMA0_IRQ_CHAN20;
#endif /* CONF_DMA0_IRQ_CHAN20 */
#if defined CONF_DMA0_IRQ_CHAN21
		case 21:
			return CONF_DMA0_IRQ_CHAN21;
#endif /* CONF_DMA0_IRQ_CHAN21 */
#if defined CONF_DMA0_IRQ_CHAN22
		case 22:
			return CONF_DMA0_IRQ_CHAN22;
#endif /* CONF_DMA0_IRQ_CHAN22 */
#if defined CONF_DMA0_IRQ_CHAN23
		case 23:
			return CONF_DMA0_IRQ_CHAN23;
#endif /* CONF_DMA0_IRQ_CHAN23 */
#if defined CONF_DMA0_IRQ_CHAN24
		case 24:
			return CONF_DMA0_IRQ_CHAN24;
#endif /* CONF_DMA0_IRQ_CHAN24 */
#if defined CONF_DMA0_IRQ_CHAN25
		case 25:
			return CONF_DMA0_IRQ_CHAN25;
#endif /* CONF_DMA0_IRQ_CHAN25 */
#if defined CONF_DMA0_IRQ_CHAN26
		case 26:
			return CONF_DMA0_IRQ_CHAN26;
#endif /* CONF_DMA0_IRQ_CHAN26 */
#if defined CONF_DMA0_IRQ_CHAN27
		case 27:
			return CONF_DMA0_IRQ_CHAN27;
#endif /* CONF_DMA0_IRQ_CHAN27 */
#if defined CONF_DMA0_IRQ_CHAN28
		case 28:
			return CONF_DMA0_IRQ_CHAN28;
#endif /* CONF_DMA0_IRQ_CHAN28 */
#if defined CONF_DMA0_IRQ_CHAN29
		case 29:
			return CONF_DMA0_IRQ_CHAN29;
#endif /* CONF_DMA0_IRQ_CHAN29 */
#if defined CONF_DMA0_IRQ_CHAN30
		case 30:
			return CONF_DMA0_IRQ_CHAN30;
#endif /* CONF_DMA0_IRQ_CHAN30 */
#if defined CONF_DMA0_IRQ_CHAN31
		case 31:
			return CONF_DMA0_IRQ_CHAN31;
#endif /* CONF_DMA0_IRQ_CHAN31 */
		default:
			return -1;
	}
	return -1;
}

struct niiet_dma_priv {
	struct niiet_dma_req *req[CONF_DMA0_MISC_CHAN_NUM];
};

static struct niiet_dma_priv niiet_dma_priv;

static irq_return_t niiet_dma_irq_handler(unsigned int irq_num, void *dev_id) {
	return 0;
}

int niiet_dma_init(uintptr_t label, int ch) {
	int irq;
	int res;

	(void)label;

	clk_enable(DMA_CLK_NAME());
	irq = niiet_dma_ch_to_irq(ch);
	if (irq == -1) {
		return -1;
	}

	res = irq_attach(irq, niiet_dma_irq_handler,/* IF_SHARESUP */0, &niiet_dma_priv, "dma");
	if (0 != res) {
		return res;
	}

	return 0;
}

int niiet_dma_req(int ch, struct niiet_dma_req *req) {
	niiet_dma_priv.req[ch] = req;

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
