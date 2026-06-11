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
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <kernel/irq.h>
#include <util/field.h>
#include <util/log.h>

#include "mcp2515.h"

#define CAN_DEV_ID OPTION_GET(NUMBER, dev_id)
#define SPI_BUS    OPTION_GET(NUMBER, spi_bus)
#define SPI_CS     OPTION_GET(NUMBER, spi_cs)

/* CANCTRL register value after reset */
#define MCP2515_CANCTRL_DEFAULT 0x87

#define TXB_BUF_SIZE 14

static struct spi_device *spi_bus_dev;

static uint8_t spi_txbuf[TXB_BUF_SIZE];

static void mcp2515_cmd(uint8_t cmd) {
	uint8_t ibuf[4];

	ibuf[0] = cmd;

	spi_transfer(spi_bus_dev, ibuf, NULL, 1);
}

static void mcp2515_reg_store(uint8_t reg, uint8_t val) {
	uint8_t ibuf[4];

	ibuf[0] = MCP2515_WRITE;
	ibuf[1] = reg;
	ibuf[2] = val;

	spi_transfer(spi_bus_dev, ibuf, NULL, 3);
}

static uint8_t mcp2515_reg_load(uint8_t reg) {
	uint8_t ibuf[4];
	uint8_t obuf[4];

	ibuf[0] = MCP2515_READ;
	ibuf[1] = reg;

	spi_transfer(spi_bus_dev, ibuf, obuf, 3);

	return obuf[2];
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
	mcp2515_cmd(MCP2515_RESET);

	/* Wait for mcp2515 to restart */
	usleep(10000);
}

static int mcp2515_open(struct can_dev *can) {
	uint8_t reg;

	reg = mcp2515_reg_load(MCP2515_RXB0CTRL);
	reg = FIELD_SET(reg, MCP2515_RXBnCTRL_RXM, MCP2515_RXBnCTRL_RXM_VALID);
	reg |= MCP2515_RXBnCTRL_BUKT;
	mcp2515_reg_store(MCP2515_RXB0CTRL, reg);

	reg = mcp2515_reg_load(MCP2515_RXB1CTRL);
	reg = FIELD_SET(reg, MCP2515_RXBnCTRL_RXM, MCP2515_RXBnCTRL_RXM_VALID);
	mcp2515_reg_store(MCP2515_RXB1CTRL, reg);

	mcp2515_reg_store(MCP2515_RXM0SIDH, 0);
	mcp2515_reg_store(MCP2515_RXM0SIDL, 0);
	mcp2515_reg_store(MCP2515_RXM0EID8, 0);
	mcp2515_reg_store(MCP2515_RXM0EID0, 0);

	mcp2515_reg_store(MCP2515_RXM1SIDH, 0);
	mcp2515_reg_store(MCP2515_RXM1SIDL, 0);
	mcp2515_reg_store(MCP2515_RXM1EID8, 0);
	mcp2515_reg_store(MCP2515_RXM1EID0, 0);

	mcp2515_reg_orin(MCP2515_RXM0SIDL, MCP2515_RXFnSIDL_EXIDE);
	mcp2515_reg_orin(MCP2515_RXM1SIDL, MCP2515_RXFnSIDL_EXIDE);

	/* Enable normal mode */
	reg = mcp2515_reg_load(MCP2515_CANCTRL);
	reg = FIELD_SET(reg, MCP2515_CANCTRL_REQOP, MCP2515_CANCTRL_REQOP_NORM);
	mcp2515_reg_store(MCP2515_CANCTRL, reg);

	usleep(1000);

	return 0;
}

static void mcp2515_close(struct can_dev *can) {
	uint8_t reg;

	/* Enable sleep mode */
	reg = mcp2515_reg_load(MCP2515_CANCTRL);
	reg = FIELD_SET(reg, MCP2515_CANCTRL_REQOP, MCP2515_CANCTRL_REQOP_SLEEP);
	mcp2515_reg_store(MCP2515_CANCTRL, reg);
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
	// int i;

	frame = (const struct can_frame *)data;

	spi_txbuf[MCP2515_TXB_CTRL] = MCP2515_LOAD_TX0;
	spi_txbuf[MCP2515_TXB_DLC] = frame->len;

	if (frame->can_id & CAN_RTR_FLAG) {
		spi_txbuf[MCP2515_TXB_DLC] |= MCP2515_TXB_DLC_RTR;
	}

	if (frame->can_id & CAN_EFF_FLAG) {
		spi_txbuf[MCP2515_TXB_EID0] = (frame->can_id & 0xff);
		spi_txbuf[MCP2515_TXB_EID8] = (frame->can_id & 0xff00) >> 8;
		spi_txbuf[MCP2515_TXB_SIDL] = (frame->can_id & 0x30000) >> 16;
		spi_txbuf[MCP2515_TXB_SIDL] |= (frame->can_id & 0x1c0000) >> 13;
		spi_txbuf[MCP2515_TXB_SIDL] |= MCP2515_RXFnSIDL_EXIDE;
		spi_txbuf[MCP2515_TXB_SIDH] = (frame->can_id & 0x1fe00000) >> 21;
	}
	else {
		spi_txbuf[MCP2515_TXB_SIDL] = (frame->can_id & 0x007) << 5;
		spi_txbuf[MCP2515_TXB_SIDH] = (frame->can_id & 0x7f8) >> 3;
	}

	memcpy(&spi_txbuf[MCP2515_TXB_DATA], frame->data, frame->len);

	spi_transfer(spi_bus_dev, spi_txbuf, NULL, TXB_BUF_SIZE);
	mcp2515_cmd(MCP2515_RTS_TX0);

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
	}

	return 0;
}
