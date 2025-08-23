/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    21.08.2025
 */

#include <stdint.h>
#include <errno.h>

#include <drivers/gpio.h>
#include <drivers/clk/mdr1206_rst_clk.h>

#include <config/board_config.h>

#include <framework/mod/options.h>
#include <hal/reg.h>


#define GPIO_CHIP_ID     OPTION_GET(NUMBER, gpio_chip_id)
#define GPIO_PINS_NUMBER 16

#define CONF_GPIO_PORT_A_CLK_ENABLE() "CLK_GPIOA"
#define CONF_GPIO_PORT_B_CLK_ENABLE() "CLK_GPIOB"
#define CONF_GPIO_PORT_C_CLK_ENABLE() "CLK_GPIOC"
#define CONF_GPIO_PORT_D_CLK_ENABLE() "CLK_GPIOD"

struct gpio_reg {
    volatile uint32_t RXTX;      /*!< 0x00 PORT Data Register */
    volatile uint32_t OE;        /*!< 0x04 PORT Output Enable Register */
    volatile uint32_t FUNC;      /*!< 0x08 PORT Function Register */
    volatile uint32_t ANALOG;    /*!< 0x0C PORT Analog Function Register */
    volatile uint32_t PULL;      /*!< 0x10 PORT Pull Up/Down Register */
    volatile uint32_t RESERVED0; /*!<Reserved */
    volatile uint32_t PWR;       /*!< 0x18 PORT Power Register */
    volatile uint32_t  RESERVED1; /*!<Reserved */
    volatile uint32_t SETTX;     /*!< 0х20 PORT Set Output Register */
    volatile uint32_t CLRTX;     /*!< 0x24 PORT Reset Output Register */
    volatile uint32_t  RDTX;     /*!< 0x28 PORT Read Output Register */
};

static const struct gpio_chip milandr_gpio_chip;

static inline volatile struct gpio_reg *milandr_gpio_get_gpio_reg(int port) {
	switch (port) {
	case 0:
		return ((volatile struct gpio_reg *)CONF_GPIO_PORT_A_REGION_BASE);
	case 1:
		return ((volatile struct gpio_reg *)CONF_GPIO_PORT_B_REGION_BASE);
	case 2:
		return ((volatile struct gpio_reg *)CONF_GPIO_PORT_C_REGION_BASE);
	case 3:
		return ((volatile struct gpio_reg *)CONF_GPIO_PORT_D_REGION_BASE);
	default:
		return NULL;
	}

	return NULL;
}

static inline char *milandr_gpio_get_gpio_clk(int port) {
	switch (port) {
	case 0:
		return CONF_GPIO_PORT_A_CLK_ENABLE();
	case 1:
		return CONF_GPIO_PORT_B_CLK_ENABLE();
	case 2:
		return CONF_GPIO_PORT_C_CLK_ENABLE();
		break;
	case 3:
		return CONF_GPIO_PORT_D_CLK_ENABLE();
		break;
	default:
		return NULL;
	}

	return NULL;
}

static int milandr_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	volatile struct gpio_reg *gpio_reg;
	uint32_t alt;
	char *clk_name;

	gpio_reg = milandr_gpio_get_gpio_reg(port);
	if (gpio_reg == NULL) {
		return -EINVAL;
	}

	clk_name = milandr_gpio_get_gpio_clk(port);
	if (clk_name == NULL)  {
		return -EINVAL;
	}

	/* Enable port */
	clk_enable(clk_name);

	if (mode & GPIO_MODE_IN) {
		gpio_reg->ANALOG |= pins; /* XXX*/
	}

	if (mode & GPIO_MODE_IN_PULL_UP) {
		gpio_reg->PULL |= pins;
	}

	if (mode & GPIO_MODE_OUT) {
		gpio_reg->OE |= pins;
	}


	if (mode & GPIO_MODE_ALT_SECTION) {
		alt = GPIO_MODE_ALT_GET(mode);
	} else {
		alt = 0;
	}
	for (int i = 0; i < GPIO_PINS_NUMBER; i++) {
		if (pins & (1 << i)) {
			gpio_reg->FUNC &= ~(0x3 << i * 2);
			gpio_reg->FUNC |= (alt << i * 2);
		}

	}
	return 0;
}

static void milandr_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = milandr_gpio_get_gpio_reg(port);
	if (gpio_reg == NULL) {
		return;
	}

	if (level) {
		gpio_reg->SETTX |= pins;
	}
	else {
		gpio_reg->CLRTX |= pins;
	}
}

static gpio_mask_t milandr_gpio_get(unsigned int port, gpio_mask_t pins) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = milandr_gpio_get_gpio_reg(port);
	if (gpio_reg == NULL) {
		return -1;
	}

	return gpio_reg->RDTX & pins;
}

static const struct gpio_chip milandr_gpio_chip = {
    .setup_mode = milandr_gpio_setup_mode,
    .get = milandr_gpio_get,
    .set = milandr_gpio_set,

    .nports = 4,

    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&milandr_gpio_chip);

