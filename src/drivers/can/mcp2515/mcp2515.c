/**
 * @brief Stand-Alone CAN Controller With SPI Interface
 *
 * @author Aleksey Zhmulin
 * @date 08.06.26
 */

#include <errno.h>
#include <linux/can.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <drivers/can_dev.h>
#include <drivers/char_dev.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <util/field.h>
#include <util/log.h>

#include "mcp2515.h"

#define CAN_DEV_ID OPTION_GET(NUMBER, dev_id)

#define SPI_BUS OPTION_GET(NUMBER, spi_bus)
#define SPI_CS  OPTION_GET(NUMBER, spi_cs)

#define GPIO_IRQ_PORT OPTION_GET(NUMBER, gpio_irq_port)
#define GPIO_IRQ_PIN  OPTION_GET(NUMBER, gpio_irq_pin)

#define MCP2515_PROPSEG  OPTION_GET(NUMBER, propseg)
#define MCP2515_PHSEG1   OPTION_GET(NUMBER, phseg1)
#define MCP2515_PHSEG2   OPTION_GET(NUMBER, phseg2)
#define MCP2515_BITRATE  OPTION_GET(NUMBER, bitrate)
#define MCP2515_CLK_FREQ OPTION_GET(NUMBER, clk_freq)

/* Synchronization Jump Width */
#define MCP2515_SJW 1

/* Baud Rate Prescaler */
#define MCP2515_BRP                                                             \
	(uint8_t)((float)MCP2515_CLK_FREQ                                           \
	          / ((float)(MCP2515_PROPSEG + MCP2515_PHSEG1 + MCP2515_PHSEG2 + 1) \
	              * (float)(2 * MCP2515_BITRATE)))

/* CANCTRL register value after reset */
#define MCP2515_CANCTRL_DEFAULT 0x87

static struct spi_device *spi_bus_dev;

static void mcp2515_cmd(uint8_t cmd) {
	uint8_t ibuf[4];

	ibuf[0] = cmd;

	spi_transfer(spi_bus_dev, ibuf, NULL, 1);
}

static void mcp2515_regs_store(uint8_t addr, uint8_t *buf, size_t len) {
	uint8_t ibuf[16];

	ibuf[0] = MCP2515_WRITE;
	ibuf[1] = addr;

	memcpy(ibuf + 2, buf, len);

	spi_transfer(spi_bus_dev, ibuf, NULL, len + 2);
}

static void mcp2515_regs_load(uint8_t addr, uint8_t *buf, size_t len) {
	uint8_t obuf[16];
	uint8_t ibuf[4];

	ibuf[0] = MCP2515_READ;
	ibuf[1] = addr;

	spi_transfer(spi_bus_dev, ibuf, obuf, len + 2);

	memcpy(buf, obuf + 2, len);
}

static void mcp2515_reg_store(uint8_t addr, uint8_t val) {
	mcp2515_regs_store(addr, &val, 1);
}

static uint8_t mcp2515_reg_load(uint8_t addr) {
	uint8_t val;

	mcp2515_regs_load(addr, &val, 1);

	return val;
}

static inline void mcp2515_reg_orin(unsigned int reg, uint8_t mask) {
	return mcp2515_reg_store(reg, mcp2515_reg_load(reg) | mask);
}

static inline void mcp2515_reg_andin(unsigned int reg, uint8_t mask) {
	return mcp2515_reg_store(reg, mcp2515_reg_load(reg) & mask);
}

static inline void mcp2515_reg_clear(unsigned int reg, uint8_t mask) {
	return mcp2515_reg_store(reg, mcp2515_reg_load(reg) & ~mask);
}

static void mcp2515_reset(struct can_dev *can) {
	uint8_t reg;

	mcp2515_cmd(MCP2515_RESET);

	/* Wait for mcp2515 to restart */
	usleep(10000);

	reg = mcp2515_reg_load(MCP2515_CNF1);
	reg = FIELD_SET(reg, MCP2515_CNF1_BRP, MCP2515_BRP - 1);
	reg = FIELD_SET(reg, MCP2515_CNF1_SJW, MCP2515_SJW - 1);
	mcp2515_reg_store(MCP2515_CNF1, reg);

	reg = mcp2515_reg_load(MCP2515_CNF2);
	reg = FIELD_SET(reg, MCP2515_CNF2_PROPSEG, MCP2515_PROPSEG - 1);
	reg = FIELD_SET(reg, MCP2515_CNF2_PHSEG1, MCP2515_PHSEG1 - 1);
	reg |= MCP2515_CNF2_SAM | MCP2515_CNF2_BTLMODE;
	mcp2515_reg_store(MCP2515_CNF2, reg);

	reg = mcp2515_reg_load(MCP2515_CNF3);
	reg = FIELD_SET(reg, MCP2515_CNF3_PHSEG2, MCP2515_PHSEG2 - 1);
	reg |= MCP2515_CNF3_SOF;
	mcp2515_reg_store(MCP2515_CNF3, reg);

	reg = mcp2515_reg_load(MCP2515_RXB0CTRL);
	reg = FIELD_SET(reg, MCP2515_RXB_CTRL_RXM, MCP2515_RXB_CTRL_RXM_VALID);
	reg |= MCP2515_RXB_CTRL_BUKT;
	mcp2515_reg_store(MCP2515_RXB0CTRL, reg);

	reg = mcp2515_reg_load(MCP2515_RXB1CTRL);
	reg = FIELD_SET(reg, MCP2515_RXB_CTRL_RXM, MCP2515_RXB_CTRL_RXM_VALID);
	mcp2515_reg_store(MCP2515_RXB1CTRL, reg);

	mcp2515_reg_store(MCP2515_RXM0SIDH, 0);
	mcp2515_reg_store(MCP2515_RXM0SIDL, 0);
	mcp2515_reg_store(MCP2515_RXM0EID8, 0);
	mcp2515_reg_store(MCP2515_RXM0EID0, 0);

	mcp2515_reg_store(MCP2515_RXM1SIDH, 0);
	mcp2515_reg_store(MCP2515_RXM1SIDL, 0);
	mcp2515_reg_store(MCP2515_RXM1EID8, 0);
	mcp2515_reg_store(MCP2515_RXM1EID0, 0);

	mcp2515_reg_orin(MCP2515_RXM0SIDL, MCP2515_RXB_SIDL_EXIDE);
	mcp2515_reg_orin(MCP2515_RXM1SIDL, MCP2515_RXB_SIDL_EXIDE);

	/* Enable normal mode */
	reg = mcp2515_reg_load(MCP2515_CANCTRL);
	reg = FIELD_SET(reg, MCP2515_CANCTRL_REQOP, MCP2515_CANCTRL_REQOP_NORM);
	mcp2515_reg_store(MCP2515_CANCTRL, reg);
}

static int mcp2515_open(struct can_dev *can) {
	// uint8_t reg;

	// /* Enable normal mode */
	// reg = mcp2515_reg_load(MCP2515_CANCTRL);
	// reg = FIELD_SET(reg, MCP2515_CANCTRL_REQOP, MCP2515_CANCTRL_REQOP_NORM);
	// mcp2515_reg_store(MCP2515_CANCTRL, reg);

	// usleep(1000);

	return 0;
}

static void mcp2515_close(struct can_dev *can) {
	// uint8_t reg;

	// /* Enable sleep mode */
	// reg = mcp2515_reg_load(MCP2515_CANCTRL);
	// reg = FIELD_SET(reg, MCP2515_CANCTRL_REQOP, MCP2515_CANCTRL_REQOP_SLEEP);
	// mcp2515_reg_store(MCP2515_CANCTRL, reg);
}

static void mcp2515_rxint(struct can_dev *can, int enable) {
	if (enable) {
		mcp2515_reg_orin(MCP2515_CANINTE,
		    MCP2515_CANINTF_RX0 | MCP2515_CANINTF_RX1);
	}
	else {
		mcp2515_reg_clear(MCP2515_CANINTE,
		    MCP2515_CANINTF_RX0 | MCP2515_CANINTF_RX1);
	}
}

static int mcp2515_send(struct can_dev *can, const void *data) {
	const struct can_frame *frame;
	uint8_t frame_len;
	uint8_t txbuf[MCP2515_TXB_SIZE];

	frame = (const struct can_frame *)data;
	frame_len = frame->len;
	if (frame_len > 8) {
		frame_len = 8;
	}

	txbuf[MCP2515_TXB_CTRL] = 0;
	txbuf[MCP2515_TXB_DLC] = frame_len;

	if (frame->can_id & CAN_RTR_FLAG) {
		txbuf[MCP2515_TXB_DLC] |= MCP2515_TXB_DLC_RTR;
	}

	if (frame->can_id & CAN_EFF_FLAG) {
		txbuf[MCP2515_TXB_EID0] = (frame->can_id & 0xff);
		txbuf[MCP2515_TXB_EID8] = (frame->can_id & 0xff00) >> 8;
		txbuf[MCP2515_TXB_SIDH] = (frame->can_id & 0x1fe00000) >> 21;
		txbuf[MCP2515_TXB_SIDL] = (frame->can_id & 0x30000) >> 16;
		txbuf[MCP2515_TXB_SIDL] |= (frame->can_id & 0x1c0000) >> 13;
		txbuf[MCP2515_TXB_SIDL] |= MCP2515_TXB_SIDL_EXIDE;
	}
	else {
		txbuf[MCP2515_TXB_SIDH] = (frame->can_id & 0x7f8) >> 3;
		txbuf[MCP2515_TXB_SIDL] = (frame->can_id & 0x007) << 5;
	}

	memcpy(&txbuf[MCP2515_TXB_DATA], frame->data, frame_len);

	mcp2515_regs_store(MCP2515_TXB0CTRL, txbuf, MCP2515_TXB_SIZE);
	mcp2515_reg_store(MCP2515_TXB0CTRL, MCP2515_TXB_CTRL_TXREQ);

	return 0;
}

static const struct can_ops mcp2515_can_ops = {
    .reset = mcp2515_reset,
    .open = mcp2515_open,
    .close = mcp2515_close,
    .rxint = mcp2515_rxint,
    .send = mcp2515_send,
};

CAN_DEVICE_DEF(mcp2515_can_dev, &mcp2515_can_ops, NULL, CAN_DEV_ID);

static int mcp2515_receive(struct can_frame *frame) {
	uint8_t rxbuf[MCP2515_RXB_SIZE];
	uint8_t rxbuf_addr;
	uint8_t canintf;
	canid_t can_id;

	can_id = 0;

	canintf = mcp2515_reg_load(MCP2515_CANINTF);
	if (canintf & MCP2515_CANINTF_RX0) {
		rxbuf_addr = MCP2515_RXB0CTRL;
	}
	else if (canintf & MCP2515_CANINTF_RX1) {
		rxbuf_addr = MCP2515_RXB1CTRL;
	}
	else {
		return -1;
	}

	mcp2515_regs_load(rxbuf_addr, rxbuf, MCP2515_RXB_SIZE);

	frame->len = rxbuf[MCP2515_RXB_DLC] & 0xf;
	if (frame->len > 8) {
		frame->len = 8;
	}

	if (rxbuf[MCP2515_RXB_SIDL] & MCP2515_RXB_SIDL_EXIDE) {
		can_id |= rxbuf[MCP2515_RXB_EID0];
		can_id |= rxbuf[MCP2515_RXB_EID8] << 8;
		can_id |= rxbuf[MCP2515_RXB_SIDH] << 21;
		can_id |= FIELD_GET(rxbuf[MCP2515_RXB_SIDL], MCP2515_RXB_SIDL_EID) << 16;
		can_id |= FIELD_GET(rxbuf[MCP2515_RXB_SIDL], MCP2515_RXB_SIDL_SID) << 18;
		can_id |= CAN_EFF_FLAG;
	}
	else {
		can_id |= rxbuf[MCP2515_RXB_SIDH] << 3;
		can_id |= rxbuf[MCP2515_RXB_SIDL] >> 5;
	}

	if (rxbuf[MCP2515_RXB_CTRL] & MCP2515_RXB_CTRL_RXRTR) {
		can_id |= CAN_RTR_FLAG;
	}

	frame->can_id = can_id;

	memcpy(frame->data, &rxbuf[MCP2515_RXB_DATA], frame->len);

	return 0;
}

static int mcp2515_irq_handler(unsigned int irq_num, void *data) {
	log_error("irq happened");
	
	(void)mcp2515_receive;

	return 0;
}

EMBOX_UNIT_INIT(mcp2515_init);

static int mcp2515_init(void) {
	uint8_t reg;

	spi_bus_dev = spi_dev_by_id(SPI_BUS);
	spi_bus_dev->spid_flags |= SPI_CS_ACTIVE;
	spi_bus_dev->spid_flags |= SPI_CS_INACTIVE;

	spi_select(spi_bus_dev, SPI_CS);

	mcp2515_reset(&mcp2515_can_dev);

	reg = mcp2515_reg_load(MCP2515_CANCTRL);
	if (reg != MCP2515_CANCTRL_DEFAULT) {
		log_error("Invalid CANCTRL value");
		return -1;
	}

	return gpio_irq_attach(GPIO_IRQ_PORT, 1 << GPIO_IRQ_PIN,
	    mcp2515_irq_handler, &mcp2515_can_dev);
}
