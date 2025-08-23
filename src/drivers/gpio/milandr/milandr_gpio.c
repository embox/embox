/**
 * @file
 * @brief
 *
 * @author  Andrew Bursian
 * @date    21.02.2023
 */

#include <stdint.h>

#include <drivers/gpio.h>
#include <framework/mod/options.h>
#include <hal/reg.h>


#define GPIO_CHIP_ID     OPTION_GET(NUMBER, gpio_chip_id)
#define GPIO_PINS_NUMBER 16

#define GPIOA ((volatile struct gpio_reg *)0x50080000)
#define GPIOB ((volatile struct gpio_reg *)0x50088000)
#define GPIOC ((volatile struct gpio_reg *)0x50090000)
#define GPIOD ((volatile struct gpio_reg *)0x500F0000)

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

static inline volatile struct gpio_reg *milandr_gpio_get_gpio_port(
    unsigned int port) {
	switch (port) {
	case 0:
		return GPIOA;
	case 1:
		return GPIOB;
	case 2:
		return GPIOC;
		break;
	case 3:
		return GPIOD;
		break;
	default:
		return NULL;
	}

	return 0;
}

static int milandr_gpio_setup_mode(unsigned int port, gpio_mask_t pins,
    uint32_t mode) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = milandr_gpio_get_gpio_port(port);
	if (gpio_reg == NULL) {
		return -1;
	}



	if (mode & GPIO_MODE_IN) {
		gpio_reg->ANALOG |= pins;
	}

	if (mode & GPIO_MODE_IN_PULL_UP) {
		gpio_reg->PULL |= pins;
	}

	if (mode & GPIO_MODE_OUT) {
		gpio_reg->OE |= pins;
	}


	if (mode & GPIO_MODE_ALT_SECTION) {
		/* Enable ALTFUNC */
		gpio_reg->FUNC |= pins;
#if 0
		for (int i = 0; i < GPIO_PINS_NUMBER; i++) {
			if (pins & (1 << i)) {
				uint32_t alt = GPIO_MODE_ALT_GET(mode);

			}
		}
#endif
	}

	return 0;
}

static void milandr_gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	volatile struct gpio_reg *gpio_reg;

	gpio_reg = milandr_gpio_get_gpio_port(port);
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

	gpio_reg = milandr_gpio_get_gpio_port(port);
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

