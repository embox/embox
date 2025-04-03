/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.04.2012
 */

#include <assert.h>
#include <stdint.h>

#include <drivers/gpio.h>
#include <embox/unit.h>
#include <hal/reg.h>

#include "stm32_gpio_vl.h"

#define RCC_APB1RSTR  0x40021010
#define RCC_APB1PWR   0x10000000
#define RCC_APB2ENR   0x40021018
#define RCC_APB2GPIOC 0x00000010

#define RCC_APB2GPIOx 0x000001fc
#define RCC_APB2AFIO  0x00000001

#define STM32_GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)

EMBOX_UNIT_INIT(stm32_gpio_init);

static const struct gpio_chip stm32_gpio_chip;

static int stm32_gpio_init(void) {
	REG_ORIN(RCC_APB1RSTR, RCC_APB1PWR);
	REG_ORIN(RCC_APB2ENR, RCC_APB2GPIOx);
	REG_ORIN(RCC_APB2ENR, RCC_APB2AFIO);
	return 0;
}

static void set_state(struct stm32_gpio *gpio, gpio_mask_t mask,
    int new_state) {
	gpio_mask_t tmask = mask;
	assert(gpio);
	assert((new_state & ~(0xf)) == 0);
	for (int lo = 0; lo < 2; lo++) {
		uint32_t gpio_state = REG32_LOAD(&(gpio->crl));
		for (int i = 0; i < 8; i++) {
			if (tmask & 0x01) {
				gpio_state = (gpio_state & ~(0xf << (i * 4)))
				             | (new_state << (4 * i));
			}
			tmask >>= 1;
		}
		REG32_STORE(&(gpio->crl), gpio_state);
	}
}

static int stm32_gpio_setup_mode(unsigned int port, gpio_mask_t mask,
    uint32_t mode) {
	int mode_val = 0;
	struct stm32_gpio *gpio = STM32_GPIO(port);

	assert(gpio);

	if ((mode & GPIO_MODE_OUT_SECTION)
	    && (mode & GPIO_MODE_IN_SECTION)) { /* mode is incorrect */
		return -1;
	}

	if (mode & GPIO_MODE_IN) {
		mode_val = 4;
		/* mask inputs flag only without GPIO_MODE_INPUT */
		if ((mode & GPIO_MODE_IN_SECTION) & ~GPIO_MODE_IN) {
			mode_val = 8;

			if (mode & GPIO_MODE_IN_PULL_UP) {
				REG32_STORE(&(gpio->bsrr), mask);
			}

			if (mode & GPIO_MODE_IN_PULL_DOWN) {
				REG32_STORE(&(gpio->brr), mask);
			}
		}
	}
	else if (mode & GPIO_MODE_OUT) {
		mode_val = 3;

		if (mode & GPIO_MODE_ALT_SECTION) {
			mode_val |= 8;
		}

		if (mode & GPIO_MODE_OUT_OPEN_DRAIN) {
			mode_val |= 4;
		}
	}

	set_state(gpio, mask, mode_val);

	return 0;
}

static void stm32_gpio_set(unsigned int port, gpio_mask_t mask, int level) {
	struct stm32_gpio *gpio = STM32_GPIO(port);

	assert(gpio);
	assert((mask & ~((1 << 16) - 1)) == 0);

	if (level) {
		REG32_STORE(&(gpio->bsrr), mask);
	}
	else {
		REG32_STORE(&(gpio->brr), mask);
	}
}

static gpio_mask_t stm32_gpio_get(unsigned int port, gpio_mask_t mask) {
	struct stm32_gpio *gpio = STM32_GPIO(port);
	assert(gpio);
	return mask & REG32_LOAD(&(gpio->idr));
}

static const struct gpio_chip stm32_gpio_chip = {
    .setup_mode = stm32_gpio_setup_mode,
    .get = stm32_gpio_get,
    .set = stm32_gpio_set,
    .nports = GPIO_PORT_NUM,
    .chip_id = STM32_GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&stm32_gpio_chip);
