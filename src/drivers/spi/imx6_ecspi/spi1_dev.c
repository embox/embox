/**
 * @file
 * @brief
 * @author Andrew Bursian
 * @version
 * @date 20.06.2023
 */

#include <util/macro.h>

#include <drivers/spi.h>

#define SPI_DEV_NAME      spi_bus_1_

SPI_DEV_DEF(MACRO_CONCAT(SPI_DEV_NAME,1), NULL, NULL, 1);

