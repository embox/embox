/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */

#include <stdint.h>
#include <string.h>
#include <drivers/spi.h>

#include <embox/unit.h>

#include "skeleton_spi.h"

#define SPI1_BASE_ADDR	  (uintptr_t)0x0
#define SPI_DEV_NAME      spi_bus_1

EMBOX_UNIT_INIT(skeleton_spi1_init);

static struct skeleton_spi skeleton_spi1 = {
	.instance = SPI1_BASE_ADDR,

};

static int skeleton_spi1_init(void) {
	skeleton_spi_init(&skeleton_spi1);

	return 0;
}

SPI_DEV_DEF(SPI_DEV_NAME, &skeleton_spi_ops, &skeleton_spi1, 1);
