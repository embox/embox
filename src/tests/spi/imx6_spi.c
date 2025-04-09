/**
 * @file imx6_spi.c
 * @brief Basic test for SPI-flash JEDEC read operation
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 14.06.2017
 */

#include <drivers/spi.h>

#include <framework/mod/options.h>
#include <embox/test.h>

EMBOX_TEST_SUITE("Basic SPI flash test");

#define SPI_FLASH_BUS    OPTION_GET(NUMBER,bus_num)
#define SPI_FLASH_CS     OPTION_GET(NUMBER,cs_num)

#define JEDEC_BYTE1     0xbf
#define JEDEC_BYTE2     0x25
#define JEDEC_BYTE3     0x41

TEST_CASE("Read JEDEC ID") {
	struct spi_device *dev;
	uint8_t buf_out[] = {0x9f, 0x0, 0x0, 0x0}; /* JEDEC ID */
	uint8_t buf_in[4];
	int res;

	dev = spi_dev_by_id(SPI_FLASH_BUS);
	test_assert(dev);

	dev->flags |= SPI_CS_ACTIVE | SPI_CS_INACTIVE;

	res = spi_select(dev, SPI_FLASH_CS);
	test_assert(res == 0);

	res = spi_transfer(dev, buf_out, buf_in, 4);
	test_assert(res == 0);
	test_assert(buf_in[1] == JEDEC_BYTE1);
	test_assert(buf_in[2] == JEDEC_BYTE2);
	test_assert(buf_in[3] == JEDEC_BYTE3);
}
