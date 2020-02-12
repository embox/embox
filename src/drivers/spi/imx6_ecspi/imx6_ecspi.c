/**
 * @file
 * @brief i.MX6 Enhanced Configurable SPI driver
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 12.06.2017
 */

#include <drivers/gpio/gpio.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <errno.h>
#include <assert.h>
#include <hal/reg.h>
#include <util/log.h>

#include "imx6_ecspi.h"

#define ECSPI_RXDATA(dev)                 ((dev)->base_addr + 0x00)
#define ECSPI_TXDATA(dev)                 ((dev)->base_addr + 0x04)
#define ECSPI_CONREG(dev)                 ((dev)->base_addr + 0x08)
# define ECSPI_CONREG_EN                  (1 << 0)
# define ECSPI_CONREG_XCH                 (1 << 2)
# define ECSPI_CONREG_BURST_LENGTH_OFFT   20
# define ECSPI_CONREG_BURST_LENGTH_MASK   0xFFF00000
# define ECSPI_CONREG_CHANNEL_SELECT_OFFT 18
# define ECSPI_CONREG_CHANNEL_SELECT_MASK 0x000C0000
#define ECSPI_CONFIGREG(dev)              ((dev)->base_addr + 0x0C)
#define ECSPI_INTREG(devv)                ((dev)->base_addr + 0x10)
#define ECSPI_DMAREG(dev)                 ((dev)->base_addr + 0x14)
#define ECSPI_STATREG(dev)                ((dev)->base_addr + 0x18)
# define ECSPI_STATREG_RR                 (1 << 3)
#define ECSPI_PERIODREG(dev)              ((dev)->base_addr + 0x1C)
#define ECSPI_TESTREG(dev)                ((dev)->base_addr + 0x20)
#define ECSPI_MSGDATA(dev)                ((dev)->base_addr + 0x40)

#define TIMEOUT_LIMIT		100000

#define SPI_FIFO_LEN		64

static const int imx6_ecspi_gpio_info[4][2] = /* Format is [gpio][port], more
                                                 details in IMX6DQRM.pdf at page 426 */
	{ {1, 30}, {2, 19}, {2, 24}, {2, 25} };

int imx6_ecspi_init(struct imx6_ecspi *dev) {
	/* Disable SPI block */
	REG32_CLEAR(ECSPI_CONREG(dev), ECSPI_CONREG_EN);
	/* Enable clocks */
	/* Put SPI out of reset */
	REG32_ORIN(ECSPI_CONREG(dev), ECSPI_CONREG_EN);
	/* IOMUX configuration */
	/* Configure ECSPI as a master */
	REG32_ORIN(ECSPI_CONREG(dev), 0xF << 4);
	/* Set burst length to 8 bits so we work with bytes */
	REG32_CLEAR(ECSPI_CONREG(dev), ECSPI_CONREG_BURST_LENGTH_MASK);
	REG32_ORIN(ECSPI_CONREG(dev), 7 << ECSPI_CONREG_BURST_LENGTH_OFFT);

	return 0;
}

static void imx6_ecspi_set_cs(struct imx6_ecspi *dev, int state) {
	int gpio_n = 0;
	int port = 0;

	assert(dev->cs <= 3);

	REG32_CLEAR(ECSPI_CONREG(dev), ECSPI_CONREG_CHANNEL_SELECT_MASK);
	REG32_ORIN(ECSPI_CONREG(dev), dev->cs << ECSPI_CONREG_CHANNEL_SELECT_OFFT);

	/* TODO config gpio in proper way for ECSPI2,3,4,5*/
	gpio_n = imx6_ecspi_gpio_info[dev->cs][0];
	port   = imx6_ecspi_gpio_info[dev->cs][1];

	gpio_setup_mode(gpio_n, 1 << port, GPIO_MODE_OUTPUT);
	gpio_set(gpio_n, 1 << port, state);
}

static uint8_t imx6_ecspi_transfer_byte(struct imx6_ecspi *dev, uint8_t val) {
	int timeout;

	REG32_STORE(ECSPI_TXDATA(dev), val);
	REG32_ORIN(ECSPI_CONREG(dev), ECSPI_CONREG_XCH);

	timeout = TIMEOUT_LIMIT;
	while ((REG32_LOAD(ECSPI_CONREG(dev)) & ECSPI_CONREG_XCH) && timeout--);

	if (timeout == 0)
		log_debug("transmit timeout");

	return REG32_LOAD(ECSPI_RXDATA(dev));
}

static int imx6_ecspi_select(struct spi_device *dev, int cs) {
	struct imx6_ecspi *priv = dev->priv;

	if (cs < 0 || cs > 3) {
		log_error("Only cs=0..3 are avalable!");
		return -EINVAL;
	}

	priv->cs = cs;

	return 0;
}

static int imx6_ecspi_transfer(struct spi_device *dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	struct imx6_ecspi *priv = dev->priv;
	uint8_t val;

	if (dev->flags & SPI_CS_ACTIVE) {
		imx6_ecspi_set_cs(priv, 1);
	}

	while (count--) {
		val = inbuf ? *inbuf++ : 0;
		log_debug("tx %02x", val);
		val = imx6_ecspi_transfer_byte(priv, val);

		log_debug("rx %02x", val);
		if (outbuf)
			*outbuf++ = val;
	}

	if (dev->flags & SPI_CS_INACTIVE) {
		imx6_ecspi_set_cs(priv, 0);
	}

	return 0;
}

struct spi_ops imx6_ecspi_ops = {
	.select   = imx6_ecspi_select,
	.transfer = imx6_ecspi_transfer
};
