/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 08.12.24
 */

#include <assert.h>
#include <endian.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include <asm/addrspace.h>
#include <drivers/spi.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <util/log.h>
#include <util/macro.h>

#define SC64_SPI_BASE_ADDR       OPTION_GET(NUMBER, base_addr)
#define SC64_SPI_TX_BUF_SIZE     OPTION_GET(NUMBER, tx_buf_size)
#define SC64_SPI_RX_BUF_SIZE     OPTION_GET(NUMBER, rx_buf_size)
#define SC64_SPI_DEFAULT_CLK_DIV OPTION_GET(NUMBER, default_clk_div)

#define SC64_SPI_STORE(reg, val) \
	REG32_STORE(SC64_SPI_BASE_ADDR + reg, htole32(val))

#define SC64_SPI_LOAD(reg) le32toh(REG32_LOAD(SC64_SPI_BASE_ADDR + reg))

#define SC64_SPI_CTRL        0x00 /* (R/W) */
#define SC64_SPI_MODE        0x04 /* (R) */
#define SC64_SPI_CS          0x08 /* (R/W) */
#define SC64_SPI_STAT        0x0c /* (R/W) */
#define SC64_SPI_CPU_CONF    0x10 /* (R/W) */
#define SC64_SPI_TIME        0x14 /* (R) */
#define SC64_SPI_IRQ_EN      0x18 /* (R/W) */
#define SC64_SPI_DMA_CONF    0x1c /* (R/W) */
#define SC64_SPI_INSTR_MODE  0x20 /* (R/W) */
#define SC64_SPI_INSTR_CS    0x24 /* (R/W) */
#define SC64_SPI_INSTR_LEN   0x28 /* (R/W) */
#define SC64_SPI_INSTR_PARAM 0x2c /* (R/W) */
#define SC64_SPI_TXADDR_LO   0x30 /* (R/W) */
#define SC64_SPI_TXADDR_HI   0x34 /* (R/W) */
#define SC64_SPI_RXADDR_LO   0x38 /* (R/W) */
#define SC64_SPI_RXADDR_HI   0x3c /* (R/W) */
#define SC64_SPI_AXI_ID      0x40 /* (R/W) */
#define SC64_SPI_VERSION     0x48 /* (R) */
#define SC64_SPI_CPU_TIME    0x4c /* (R/W) */

#define SC64_SPI_CTRL_GPIO_MODE (1 << 24)
#define SC64_SPI_CTRL_SSN0      (1 << 25)
#define SC64_SPI_CTRL_SSN1      (1 << 26)
#define SC64_SPI_CTRL_SSN2      (1 << 27)
#define SC64_SPI_CTRL_SSN3      (1 << 28)
#define SC64_SPI_CTRL_SCK       (1 << 29)
#define SC64_SPI_CTRL_MOSI      (1 << 30)

#define SC64_SPI_DMA_CONF_START (1 << 0)
#define SC64_SPI_DMA_CONF_RESET (1 << 1)
#define SC64_SPI_DMA_CONF_WRITE (1 << 31)

#define SC64_SPI_INSTR_PARAM_CS (1 << 0)
#define SC64_SPI_INSTR_PARAM_IE (1 << 1)
#define SC64_SPI_INSTR_PARAM_TX (1 << 2)
#define SC64_SPI_INSTR_PARAM_RX (1 << 3)

#define SC64_SPI_CS_OFF_ALL 0xf
#define SC64_SPI_CS_OFF(n)  (1 << (0x3 & (n)))

#define SC64_SPI_STAT_DONE  (1 << 16)
#define SC64_SPI_STAT_RESET 0xffffffff

#define SC64_SPI_INSTR_MODE_CPHA       (1 << 0)
#define SC64_SPI_INSTR_MODE_CPOL       (1 << 1)
#define SC64_SPI_INSTR_MODE_LSB        (1 << 2)
#define SC64_SPI_INSTR_MODE_BAUD(baud) ((0xff & (baud)) << 3)

#define SC64_SPI_SPPR_MAX 0xf
#define SC64_SPI_SPR_MAX  0xf

#define SC64_SPI_MAX_LEN 0x10000

struct sc64_spi_dev {
	int cs; /* current CS */
	uint8_t tx_buf[SC64_SPI_TX_BUF_SIZE];
	uint8_t rx_buf[SC64_SPI_RX_BUF_SIZE];
};

static int sc64_spi_get_instr_baudrate(int clk_div) {
	int sppr;
	int spr;

	for (sppr = 0; sppr <= SC64_SPI_SPPR_MAX; sppr++) {
		for (spr = 0; spr <= SC64_SPI_SPR_MAX; spr++) {
			if (clk_div == (2 + sppr * (1 << (spr + 1)))) {
				return (sppr << 4) | spr;
			}
		}
	}

	return -1;
}

static int sc64_spi_init(struct spi_controller *dev) {
	struct sc64_spi_dev *spi;

	spi = (struct sc64_spi_dev *)dev->priv;

	assert(spi);

	spi->cs = -1;

	SC64_SPI_STORE(SC64_SPI_DMA_CONF, SC64_SPI_DMA_CONF_RESET);
	SC64_SPI_STORE(SC64_SPI_IRQ_EN, 0);
	SC64_SPI_STORE(SC64_SPI_CTRL, 0);
	SC64_SPI_STORE(SC64_SPI_CS, SC64_SPI_CS_OFF_ALL);
	SC64_SPI_STORE(SC64_SPI_STAT, SC64_SPI_STAT_RESET);

	return 0;
}

static int sc64_spi_select(struct spi_controller *dev, int cs) {
	struct sc64_spi_dev *spi;
	uint32_t instr_mode;
	int baud;
	int div;

	spi = (struct sc64_spi_dev *)dev->priv;

	assert(spi);

	div = SPI_CS_DIVSOR_GET(dev->flags);
	if (!div) {
		div = SC64_SPI_DEFAULT_CLK_DIV;
	}

	baud = sc64_spi_get_instr_baudrate(div);
	if (baud < 0) {
		log_error("invalid clock divisor: %i", div);
		return -1;
	}

	if ((cs < 0) || (cs > 3)) {
		log_error("invalid CS: %i", cs);
		return -1;
	}

	spi->cs = cs;

	instr_mode = SC64_SPI_INSTR_MODE_BAUD(baud);
	if (dev->flags & SPI_CS_CPHA) {
		instr_mode |= SC64_SPI_INSTR_MODE_CPHA;
	}
	if (dev->flags & SPI_CS_CPOL) {
		instr_mode |= SC64_SPI_INSTR_MODE_CPOL;
	}

	SC64_SPI_STORE(SC64_SPI_CS, SC64_SPI_CS_OFF(cs));
	SC64_SPI_STORE(SC64_SPI_INSTR_MODE, instr_mode);

	return 0;
}

static int sc64_spi_transfer(struct spi_controller *dev, uint8_t *inbuf,
    uint8_t *outbuf, int count) {
	struct sc64_spi_dev *spi;
	uintptr_t txaddr;
	uintptr_t rxaddr;
	uint32_t param;

	spi = (struct sc64_spi_dev *)dev->priv;

	assert(spi);

	if ((spi->cs < 0) || (spi->cs > 3)) {
		log_error("invalid CS: %i", spi->cs);
		return -1;
	}

	if (!inbuf && !outbuf) {
		return -1;
	}

	if (dev->flags & SPI_CS_ACTIVE) {
		SC64_SPI_STORE(SC64_SPI_INSTR_CS, spi->cs);
	}

	txaddr = KSEG1ADDR(spi->tx_buf);
	rxaddr = KSEG1ADDR(spi->rx_buf);
	param = SC64_SPI_INSTR_PARAM_CS | SC64_SPI_INSTR_PARAM_IE;

	if (inbuf) {
		memcpy((void *)txaddr, inbuf, count);
		param |= SC64_SPI_INSTR_PARAM_TX;
	}

	if (outbuf) {
		param |= SC64_SPI_INSTR_PARAM_RX;
	}

	SC64_SPI_STORE(SC64_SPI_INSTR_PARAM, param);
	SC64_SPI_STORE(SC64_SPI_TXADDR_LO, PHYSADDR(txaddr));
	SC64_SPI_STORE(SC64_SPI_TXADDR_HI, 0);
	SC64_SPI_STORE(SC64_SPI_RXADDR_LO, PHYSADDR(rxaddr));
	SC64_SPI_STORE(SC64_SPI_RXADDR_HI, 0);
	SC64_SPI_STORE(SC64_SPI_INSTR_LEN, count);
	SC64_SPI_STORE(SC64_SPI_DMA_CONF, SC64_SPI_DMA_CONF_WRITE);

	while (!(SC64_SPI_LOAD(SC64_SPI_STAT) & SC64_SPI_STAT_DONE)) {
		usleep(1);
	}

	SC64_SPI_STORE(SC64_SPI_STAT, SC64_SPI_STAT_RESET);

	if (outbuf) {
		memcpy(outbuf, (void *)rxaddr, count);
	}

	if (dev->flags & SPI_CS_INACTIVE) {
		SC64_SPI_STORE(SC64_SPI_CS, SC64_SPI_CS_OFF(spi->cs));
	}

	return 0;
}

struct spi_controller_ops sc64_spi_ops = {
    .init = sc64_spi_init,
    .select = sc64_spi_select,
    .transfer = sc64_spi_transfer,
};

struct sc64_spi_dev sc64_spi_dev0;

SPI_CONTROLLER_DEF(sc64_spi0, &sc64_spi_ops, &sc64_spi_dev0, 0);

#define SPI_BUS_NUM        0
#define SPI_DEV_NUM        0
#define SPI_DEV_NAME       MACRO_CONCAT(MACRO_CONCAT(spi_,SPI_BUS_NUM),_)
#define SPI_DEV_FULL_NAME  MACRO_CONCAT(SPI_DEV_NAME,SPI_DEV_NUM)

SPI_DEV_DEF(SPI_DEV_FULL_NAME, NULL, NULL, SPI_BUS_NUM, SPI_DEV_NUM, NULL);
