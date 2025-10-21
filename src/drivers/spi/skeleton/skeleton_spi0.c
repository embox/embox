/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */

#include <util/macro.h>

#include <drivers/spi.h>

#include <embox/unit.h>

#include "skeleton_spi.h"

#define SPI0_BASE_ADDR	  (uintptr_t)0x0
#define SPI_BUS_NUM       0

#define SPI_BUS_NAME      MACRO_CONCAT(spi_bus_,SPI_BUS_NUM)

EMBOX_UNIT_INIT(skeleton_spi0_init);

static struct skeleton_spi skeleton_spi0 = {
	.instance = SPI0_BASE_ADDR,
};

static int skeleton_spi0_init(void) {
	skeleton_spi_init(&skeleton_spi0);

	return 0;
}

SPI_CONTROLLER_DEF(SPI_BUS_NAME, &skeleton_spic_ops, &skeleton_spi0, SPI_BUS_NUM);
