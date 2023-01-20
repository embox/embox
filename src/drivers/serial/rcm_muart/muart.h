/**
 * @file
 *
 * @date Jan 17, 2023
 * @uthor Anton Bondarev
 */

#ifndef SRC_DRIVERS_SERIAL_RCM_MUART_MUART_H_
#define SRC_DRIVERS_SERIAL_RCM_MUART_MUART_H_

#include <stdint.h>

#include "muart_dma.h"

#define MUART_1_STOP_BIT         0
#define MUART_15_STOP_BIT        1
#define MUART_2_STOP_BIT         2

#define MUART_WORD_LEN_5         0
#define MUART_WORD_LEN_6         1
#define MUART_WORD_LEN_7         2
#define MUART_WORD_LEN_8         3

#define MUART_CTRL_ENABLE           (1 << 0)
#define MUART_CTRL_LOOP_MODE        (1 << 1)
#define MUART_CTRL_APB_FIFO_MODE    (1 << 2)
#define MUART_CTRL_RS485_MODE       (1 << 3)
#define MUART_CTRL_RTS_EN           (1 << 4)
#define MUART_CTRL_CTS_MODE         (1 << 5)
#define MUART_CTRL_POL_MODE         (1 << 6)
#define MUART_CTRL_PARITY           (1 << 7)
#define MUART_CTRL_ODD_BIT          (1 << 8)
#define MUART_CTRL_ODD_MODE         (1 << 9)
#define MUART_CTRL_STOP_BITS(num)   (num << 10)
#define MUART_CTRL_WORD_LEN(len)    (len << 12)

#define MUART_IRQ_RX                ( 1 << 0)
#define MUART_IRQ_TX                ( 1 << 1)
#define MUART_IRQ_RX_TIMEOUT        ( 1 << 2)
#define MUART_IRQ_STOP_ERR          ( 1 << 3)
#define MUART_IRQ_PAR_ERR           ( 1 << 4)
#define MUART_IRQ_BREAK_ERR         ( 1 << 5)
#define MUART_IRQ_RX_OVER_ERR       ( 1 << 6)
#define MUART_IRQ_TX_OVER_ERR       ( 1 << 7)
#define MUART_IRQ_TX_TIMEOUT        ( 1 << 8)

struct muart_fifo_state_reg {
	uint16_t rx_num; /*  Receive FIFO state. */
	uint16_t tx_num; /* Transmit FIFO state. */
};


struct muart_regs {
	uint32_t id;                                   /* 0x000 */
	uint32_t version;                              /* 0x004 */
	uint32_t sw_rst;                               /* 0x008 */
	uint32_t reserve_1;                            /* 0x00C */
	uint32_t gen_status;                           /* 0x010 */
	const struct muart_fifo_state_reg fifo_status; /* 0x014 */
	uint32_t status;                               /* 0x018 */
	uint32_t reserve_2;                            /* 0x01C */
	uint32_t dtrans;                               /* 0x020 */
	uint32_t reserve_3;                            /* 0x024 */
	uint32_t drec;                                 /* 0x028 */
	uint32_t reserve_4;                            /* 0x02C */
	uint32_t bdiv;                                 /* 0x030 */
	uint32_t reserve_5;                            /* 0x034 */
	uint32_t reserve_6;                            /* 0x038 */
	uint32_t reserve_7;                            /* 0x03C */
	struct muart_fifo_state_reg fifowm;            /* 0x040 */
	uint32_t ctrl;                                 /* 0x044 */
	uint32_t mask;                                 /* 0x048 */
	uint32_t rxtimeout;                            /* 0x04C */
	uint32_t reserve_8;                            /* 0x050 */
	uint32_t txtimeout;                            /* 0x054 */
	uint32_t reserve_58[42];

	/********** DMAs *****************/
	struct muart_dma_regs TxDmaSet;        /*!<   DMA     0100 */
	struct muart_dma_regs RxDmaSet;        /*!<   DMA     0200 */
};

#endif /* SRC_DRIVERS_SERIAL_RCM_MUART_MUART_H_ */
