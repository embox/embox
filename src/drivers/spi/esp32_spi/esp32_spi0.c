/**
 * @file
 * @brief ESP32-C5 SPI2 registered as Embox SPI bus 0.
 * @author Dmitrii Piliuk
 * @version
 * @date 28.10.2026
 */


#include <errno.h>

#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <hal/reg.h>

#include <config/board_config.h>
#include <drivers/gpio/esp32_gpio.h>

#include "esp32_spi.h"

EMBOX_UNIT_INIT(esp32_spi0_init);

static struct esp32_spi esp32_spi0 = {
	.base_addr = (uintptr_t)CONF_SPI0_REGION_BASE,
};

static int esp32_spi0_clock_init(void) {
	uintptr_t pcr_base = CONF_SPI0_REGION_PCR;
	unsigned int source = CONF_SPI0_CLK_DEF_SOURCE;
	unsigned int source_div = CONF_SPI0_CLK_DEF_SOURCE_DIV;
	unsigned int timeout;
	uint32_t reg;

	if (source > 2 || !source_div || source_div > 256) {
		return -EINVAL;
	}

	REG32_ORIN(pcr_base + ESP32_SPI_PCR_CONF, ESP32_SPI_PCR_CLK_EN);
	REG32_ORIN(pcr_base + ESP32_SPI_PCR_CONF, ESP32_SPI_PCR_RST_EN);
	REG32_CLEAR(pcr_base + ESP32_SPI_PCR_CONF, ESP32_SPI_PCR_RST_EN);
	for (timeout = 100000; timeout; timeout--) {
		if (REG32_LOAD(pcr_base + ESP32_SPI_PCR_CONF)
		    & ESP32_SPI_PCR_READY) {
			break;
		}
	}
	if (!timeout) {
		return -ETIMEDOUT;
	}

	REG32_CLEAR(pcr_base + ESP32_SPI_PCR_CLKM_CONF, ESP32_SPI_PCR_CLKM_EN);
	reg = REG32_LOAD(pcr_base + ESP32_SPI_PCR_CLKM_CONF);
	reg &= ~(ESP32_SPI_PCR_DIV_MASK | ESP32_SPI_PCR_SEL_MASK);
	reg |= (source_div - 1) << ESP32_SPI_PCR_DIV_SHIFT;
	reg |= source << ESP32_SPI_PCR_SEL_SHIFT;
	REG32_STORE(pcr_base + ESP32_SPI_PCR_CLKM_CONF, reg);
	REG32_ORIN(pcr_base + ESP32_SPI_PCR_CLKM_CONF, ESP32_SPI_PCR_CLKM_EN);

	return 0;
}

static int esp32_spi0_pin_init(unsigned int port, uint32_t pin,
    uint32_t mode, uint32_t route) {
	uint32_t type = ESP32_ROUTE_TYPE(route);
	uint32_t value = ESP32_ROUTE_VALUE(route);
	int ret;

	if (type == ESP32_ROUTE_IOMUX) {
		return gpio_setup_mode(port, 1U << pin,
		    mode | GPIO_MODE_ALT_SET(value));
	}
	if (type != ESP32_ROUTE_MATRIX_IN && type != ESP32_ROUTE_MATRIX_OUT) {
		return -EINVAL;
	}

	ret = gpio_setup_mode(port, 1U << pin,
	    mode | GPIO_MODE_ALT_SET(ESP32_IO_MUX_FUNC_GPIO));
	if (ret) {
		return ret;
	}
	if (type == ESP32_ROUTE_MATRIX_IN) {
		value |= GPIO_ROUTE_INPUT;
	}

	return gpio_setup_route(port, pin, value);
}

static int esp32_spi0_pins_init(void) {
	int ret;

	ret = esp32_spi0_pin_init(CONF_SPI0_PIN_CLK_PORT,
	    CONF_SPI0_PIN_CLK_NR,
	    CONF_SPI0_PIN_CLK_MODE, CONF_SPI0_PIN_CLK_AF);
	if (ret) {
		return ret;
	}

	ret = esp32_spi0_pin_init(CONF_SPI0_PIN_TX_PORT,
	    CONF_SPI0_PIN_TX_NR,
	    CONF_SPI0_PIN_TX_MODE, CONF_SPI0_PIN_TX_AF);
	if (ret) {
		return ret;
	}

	ret = esp32_spi0_pin_init(CONF_SPI0_PIN_RX_PORT,
	    CONF_SPI0_PIN_RX_NR,
	    CONF_SPI0_PIN_RX_MODE, CONF_SPI0_PIN_RX_AF);
	if (ret) {
		return ret;
	}

	/* Route hardware CS0 using the board configuration. */
	return esp32_spi0_pin_init(CONF_SPI0_PIN_CS_PORT,
	    CONF_SPI0_PIN_CS_NR,
	    CONF_SPI0_PIN_CS_MODE, CONF_SPI0_PIN_CS_AF);
}

static int esp32_spi0_init(void) {
	int ret;

	ret = esp32_spi0_clock_init();
	if (ret) {
		return ret;
	}

	ret = esp32_spi0_pins_init();
	if (ret) {
		return ret;
	}

	return esp32_spi_hw_init(&esp32_spi0, CONF_SPI0_CLK_DIV);
}

SPI_CONTROLLER_DEF(spibus0, &esp32_spic_ops, &esp32_spi0, 0, NULL);
