/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 20.06.2025
 */

#include <util/macro.h>

#include <drivers/spi.h>

#define SPI_BUS_NUM        2
#define SPI_DEV_NUM        0
#define SPI_DEV_NAME       MACRO_CONCAT(MACRO_CONCAT(spi_,SPI_BUS_NUM),_)
#define SPI_DEV_FULL_NAME  MACRO_CONCAT(SPI_DEV_NAME,SPI_DEV_NUM)

SPI_DEV_DEF(SPI_DEV_FULL_NAME, NULL, NULL, SPI_BUS_NUM, SPI_DEV_NUM, NULL);
