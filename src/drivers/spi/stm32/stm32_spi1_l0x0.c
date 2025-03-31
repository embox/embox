/**
 * @file stm32_spi_l0x0.c
 * @brief
 * @author Vadim Deryabkin <Vadimatorikda@gmail.com>
 * @version
 * @date 20.02.2021
 */

#include <string.h>
#include <assert.h>

#include "stm32_spi_l0x0.h"

#include <drivers/spi.h>
#include <kernel/irq.h>
#include <util/log.h>
#include <drivers/gpio/gpio.h>

static struct stm32_spi stm32_spi1 = {0};

static int stm32_spi_setup(struct stm32_spi *dev, bool is_master) {
	dev->is_master = is_master;

	if (!dev->is_master) {
		log_error("SPI slave mode is not supported!");
		return -1;
	}

	set_spi1_pwr();

	// Doc: RM0451, 699/774
	static const uint32_t br =
#if OPTION_GET(NUMBER,dev) == 2
		0
#elif OPTION_GET(NUMBER,dev) == 4
		1
#elif OPTION_GET(NUMBER,dev) == 8
		2
#elif OPTION_GET(NUMBER,dev) == 16
		3
#elif OPTION_GET(NUMBER,dev) == 32
		4
#elif OPTION_GET(NUMBER,dev) == 64
		5
#elif OPTION_GET(NUMBER,dev) == 128
		6
#elif OPTION_GET(NUMBER,dev) == 256
		7
#else
#error "SPI param <<dev>> has not set!"
#endif
	;

	SPI1->CR1 = (OPTION_GET(NUMBER,spha) << 0) |
			(OPTION_GET(NUMBER,spol) << 1) |
			(1 << 2) | (br << 3);
	SPI1->CR2 = (1 << 2); // Without OVR flag.
	SPI1->CR1 |= (1 << 6);

	return 0;
}

int stm32_spi_init(struct stm32_spi *dev) {
	return stm32_spi_setup(dev, true);
}

static int stm32_spi_select(struct spi_device *dev, int cs) {
	return 0;
}

static int stm32_spi_set_mode(struct spi_device *dev, bool is_master) {
	struct stm32_spi *s = dev->priv;
	return stm32_spi_setup(s, is_master);
}

static int stm32_spi_transfer(struct spi_device *dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	uint32_t tx_byte = count;
	uint32_t rx_bute = count;

	while(tx_byte) {
		if (SPI1->SR & (1 << 0)) {
			*outbuf = SPI1->DR;
			outbuf++;
			rx_bute--;
		}

		while(!(SPI1->SR & (1 << 1))) {};
		SPI1->DR = *inbuf;
		inbuf++;
		tx_byte--;
	}

	while(rx_bute) {
		while (!(SPI1->SR & (1 << 0)));
		*outbuf = SPI1->DR;
		outbuf++;
		rx_bute--;
	}

	return 0;
}

struct spi_ops stm32_spi_ops = {
	.select   = stm32_spi_select,
	.set_mode = stm32_spi_set_mode,
	.transfer = stm32_spi_transfer
};

static int stm32_spi1_init(void) {
	gpio_setup_mode(OPTION_GET(NUMBER,port_mosi), 1 << OPTION_GET(NUMBER,pin_mosi), GPIO_MODE_ALT_SET(0));
	gpio_setup_mode(OPTION_GET(NUMBER,port_miso), 1 << OPTION_GET(NUMBER,pin_miso), GPIO_MODE_ALT_SET(0));
	gpio_setup_mode(OPTION_GET(NUMBER,port_sck), 1 << OPTION_GET(NUMBER,pin_sck), GPIO_MODE_ALT_SET(0));

	stm32_spi_init(&stm32_spi1);
	return 0;
};

#define SPI_DEV_NAME      stm32_spi_1

SPI_DEV_DEF(SPI_DEV_NAME, &stm32_spi_ops, &stm32_spi1, 1);
EMBOX_UNIT_INIT(stm32_spi1_init);
