/**
 * @file imx6_ecspi.h
 * @brief Interface for i.MX6 SPI
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 12.06.2017
 */

#ifndef IMX6_ECSPI_H_
#define IMX6_ECSPI_H_

#include <stdint.h>

#define SPI_CS_ACTIVE   (1 << 0)
#define SPI_CS_INACTIVE (1 << 1)

int imx6_ecspi_transfer(uint8_t *inbuf, uint8_t *outbuf, int count, int flags);
int imx6_ecspi_select(int bus, int cs);

#endif /* IMX6_ECSPI_H_ */
