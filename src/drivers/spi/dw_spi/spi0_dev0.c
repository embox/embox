/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 20.06.2025
 */

#include <util/macro.h>

#include <drivers/spi.h>

#define SPI_BUS_NUM       0
#define SPI_DEV_NAME      MACRO_CONCAT(MACRO_CONCAT(spi_,SPI_BUS_NUM),_)

SPI_DEV_DEF(MACRO_CONCAT(SPI_DEV_NAME,0), NULL, NULL, SPI_BUS_NUM);
