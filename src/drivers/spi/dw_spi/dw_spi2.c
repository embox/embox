/**
 * @file dw_spi.c
 * @brief Designware SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 19.12.2019
 */
#include <errno.h>

#include <util/macro.h>

#include <drivers/common/memory.h>
#include <drivers/spi.h>

#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>

#include <drivers/gpio.h>

#include "dw_spi.h"

extern struct spi_controller_ops dw_spic_ops;
extern int dw_spi_init(struct dw_spi_priv *dw_spi, uintptr_t base_addr, int spi_nr);

#define DW_SPI_BASE               OPTION_GET(NUMBER, base_addr)

#define SPI_BUS_NUM               2

#define SPI_BUS_NAME              MACRO_CONCAT(dw_spi,SPI_BUS_NUM)
#define DW_SPI_CTRL_STRUCT_NAME   MACRO_CONCAT(dw_spi_priv,SPI_BUS_NUM)

static struct dw_spi_priv DW_SPI_CTRL_STRUCT_NAME;

PERIPH_MEMORY_DEFINE(SPI_BUS_NAME, DW_SPI_BASE, 0x100);

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &dw_spic_ops, &DW_SPI_CTRL_STRUCT_NAME, SPI_BUS_NUM);

EMBOX_UNIT_INIT(dw_spi_module_init);
static int dw_spi_module_init(void) {
	dw_spi_init(&DW_SPI_CTRL_STRUCT_NAME, DW_SPI_BASE, SPI_BUS_NUM);

	return 0;
}
