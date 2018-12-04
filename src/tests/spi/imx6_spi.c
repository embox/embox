/**
 * @file imx6_spi.c
 * @brief Basic test for SPI-flash JEDEC read operation
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 14.06.2017
 */

#include <drivers/spi.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("Basic i.MX6 SPI flash test");

#define SPI_FLASH_BUS 0
#define SPI_FLASH_CS  0

TEST_CASE("Read JEDEC ID") {
	struct spi_device *dev;
	uint8_t _spi_out[] = {0x9f, 0x9f, 0x9f, 0x9f}; /* JEDEC ID */
	uint8_t _spi_in[4];
	int res;

	dev = spi_dev_by_id(SPI_FLASH_BUS);
	test_assert(dev);

	dev->flags |= SPI_CS_ACTIVE | SPI_CS_INACTIVE;

	res = spi_select(dev, SPI_FLASH_CS);
	test_assert(res == 0);

	res = spi_transfer(dev, _spi_out, _spi_in, 4);
	test_assert(res == 0 &&
	            _spi_in[0] == 0x00 && _spi_in[1] == 0xbf &&
	            _spi_in[2] == 0x25 && _spi_in[3] == 0x41);
}
