/**
 * @file dw_spi.c
 * @brief Designware SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 19.12.2019
 */
#include <errno.h>

#include <drivers/common/memory.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>

#include "dw_spi.h"

static uint32_t dw_spi_read(struct dw_spi *dw_spi, int offset) {
	return REG32_LOAD(dw_spi->base_addr + offset);
}

static void dw_spi_write(struct dw_spi *dw_spi, int offset, uint32_t val) {
	REG32_STORE(dw_spi->base_addr + offset, val);
}

int dw_spi_init(struct dw_spi *dw_spi, uintptr_t base_addr) {
	uint32_t reg;
	dw_spi->base_addr = base_addr;

	reg = dw_spi_read(dw_spi, DW_SPI_SPI_VERSION_ID);
	if (reg != DW_SPI_VERSION) {
		log_error("SPI Version mismatch!");
		return -1;
	}

	reg = dw_spi_read(dw_spi, DW_SPI_IDR);
	if (reg != DW_SPI_IDR_VAL) {
		log_error("SPI ID mismatch!");
		return -1;
	}

	dw_spi_write(dw_spi, DW_SPI_ENR, 0);

	/* Disable all interrupts */
	dw_spi_write(dw_spi, DW_SPI_IMR, 0);

	dw_spi_write(dw_spi, DW_SPI_BAUDR, 2);

	/* Turn on self-test mode */
	dw_spi_write(dw_spi, DW_SPI_CTRLR0, DW_SPI_CTRLR0_SRL);

	return 0;
}

static uint8_t dw_spi_transfer_byte(struct dw_spi *dw_spi, uint8_t val) {
	dw_spi_write(dw_spi, DW_SPI_DR, val);
	return dw_spi_read(dw_spi, DW_SPI_DR);
}

static int dw_spi_select(struct spi_device *dev, int cs) {
	struct dw_spi *dw_spi = dev->priv;

	if (cs < 0 || cs > 3) {
		log_error("Only cs=0..3 are avalable!");
		return -EINVAL;
	}

	dw_spi_write(dw_spi, DW_SPI_SER, 1 << cs);

	return 0;
}

static int dw_spi_transfer(struct spi_device *dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	struct dw_spi *dw_spi = dev->priv;
	uint8_t val;

	dw_spi_write(dw_spi, DW_SPI_ENR, 1);

	while (count--) {
		val = inbuf ? *inbuf++ : 0;
		log_debug("tx %02x", val);
		val = dw_spi_transfer_byte(dw_spi, val);

		log_debug("rx %02x", val);
		if (outbuf)
			*outbuf++ = val;
	}

	dw_spi_write(dw_spi, DW_SPI_ENR, 0);

	return 0;
}

struct spi_ops dw_spi_ops = {
	.select   = dw_spi_select,
	.transfer = dw_spi_transfer
};

#define DW_SPI0_BASE OPTION_GET(NUMBER, base_addr0)
#define DW_SPI1_BASE OPTION_GET(NUMBER, base_addr1)
#define DW_SPI2_BASE OPTION_GET(NUMBER, base_addr2)
#define DW_SPI3_BASE OPTION_GET(NUMBER, base_addr3)

#if DW_SPI0_BASE != 0
static struct dw_spi dw_spi0;
PERIPH_MEMORY_DEFINE(dw_spi0, DW_SPI0_BASE, 0x100);
SPI_DEV_DEF("dw_spi", &dw_spi_ops, &dw_spi0, 0);
#endif

#if DW_SPI1_BASE != 0
static struct dw_spi dw_spi1;
PERIPH_MEMORY_DEFINE(dw_spi1, DW_SPI1_BASE, 0x100);
SPI_DEV_DEF("dw_spi", &dw_spi_ops, &dw_spi1, 1);
#endif

#if DW_SPI2_BASE != 0
static struct dw_spi dw_spi2;
PERIPH_MEMORY_DEFINE(dw_spi2, DW_SPI2_BASE, 0x100);
SPI_DEV_DEF("dw_spi", &dw_spi_ops, &dw_spi2, 2);
#endif

#if DW_SPI3_BASE != 0
static struct dw_spi dw_spi3;
PERIPH_MEMORY_DEFINE(dw_spi3, DW_SPI3_BASE, 0x100);
SPI_DEV_DEF("dw_spi", &dw_spi_ops, &dw_spi3, 3);
#endif

EMBOX_UNIT_INIT(dw_spi_module_init);
static int dw_spi_module_init(void) {
#if DW_SPI0_BASE != 0
	dw_spi_init(&dw_spi0, DW_SPI0_BASE);
#endif
#if DW_SPI1_BASE != 0
	dw_spi_init(&dw_spi1, DW_SPI1_BASE);
#endif
#if DW_SPI2_BASE != 0
	dw_spi_init(&dw_spi2, DW_SPI2_BASE);
#endif
#if DW_SPI3_BASE != 0
	dw_spi_init(&dw_spi3, DW_SPI3_BASE);
#endif
	return 0;
}
