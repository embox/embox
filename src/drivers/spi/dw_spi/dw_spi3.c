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

#include <drivers/gpio.h>

#include "dw_spi.h"

extern struct spi_ops dw_spi_ops;

#define DW_SPI_BASE OPTION_GET(NUMBER, base_addr)

#define SPI_DEV_NAME     dw_spi

#if DW_SPI_BASE != 0
static struct dw_spi dw_spi3;
PERIPH_MEMORY_DEFINE(dw_spi3, DW_SPI_BASE, 0x100);
SPI_DEV_DEF(SPI_DEV_NAME, &dw_spi_ops, &dw_spi3, 3);
#endif

extern int dw_spi_init(struct dw_spi *dw_spi, uintptr_t base_addr, int spi_nr);
EMBOX_UNIT_INIT(dw_spi_module_init);
static int dw_spi_module_init(void) {
	dw_spi_init(&dw_spi3, DW_SPI_BASE, 3);

	return 0;
}
