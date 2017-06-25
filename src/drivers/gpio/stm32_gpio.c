/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.04.2012
 */

#include <assert.h>

#define RCC_APB1RSTR  0x40021010
#define RCC_APB1PWR   0x10000000
#define RCC_APB2ENR   0x40021018
#define RCC_APB2GPIOC 0x00000010

#define RCC_APB2GPIOx 0x000001fc
#define RCC_APB2AFIO  0x00000001
#include <drivers/gpio.h>

#include <hal/reg.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(stm32_gpio_init);

static int stm32_gpio_init(void) {
	REG_ORIN(RCC_APB1RSTR,RCC_APB1PWR);
	REG_ORIN(RCC_APB2ENR,RCC_APB2GPIOx);
	REG_ORIN(RCC_APB2ENR,RCC_APB2AFIO);
	return 0;
}

static void set_state(struct gpio *gpio, gpio_mask_t mask, int new_state) {
	volatile unsigned int *reg = &(gpio->crl);
	gpio_mask_t tmask = mask;
	assert(gpio);
	assert((new_state & ~(0xf)) == 0);
	for (int lo = 0; lo < 2; lo++) {
		unsigned int gpio_state = REG_LOAD(reg);
		for (int i = 0; i < 8; i++) {
			if (tmask & 0x01) {
				gpio_state = (gpio_state & ~(0xf<< (i * 4))) | (new_state << (4 * i));
			}
			tmask >>= 1;
		}
		REG_STORE(reg, gpio_state);
		reg = &(gpio->crh);
	}

}

int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode) {
	int mode_val = 0;
	assert(gpio);

	if ((mode & GPIO_MODE_OUT_SECTION) &&
		(mode & GPIO_MODE_IN_SECTION)) { /* mode is incorrect */
		return -1;
	}

	if (mode & GPIO_MODE_INPUT) {

		mode_val = 4;
		/* mask inputs flag only without GPIO_MODE_INPUT */
		if ((mode & GPIO_MODE_IN_SECTION) & ~GPIO_MODE_INPUT) {
			mode_val = 8;

			if (mode & GPIO_MODE_IN_PULL_UP) {
				REG_STORE(&(gpio->bsrr), mask);
			}

			if (mode & GPIO_MODE_IN_PULL_DOWN) {
				REG_STORE(&(gpio->brr), mask);
			}
		}
	} else if (mode & GPIO_MODE_OUTPUT) {

		mode_val = 3;

		if (mode & GPIO_MODE_OUT_ALTERNATE) {
			mode_val |= 8;
		}

		if (mode & GPIO_MODE_OUT_OPEN_DRAIN) {
			mode_val |= 4;
		}
	}

	set_state(gpio, mask, mode_val);

	return 0;
}

void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level){
	assert(gpio);
	assert((mask & ~((1 << 16) - 1)) == 0);

	if(level) {
		REG_STORE(&(gpio->bsrr), mask);
	} else {
		REG_STORE(&(gpio->brr), mask);
	}
}

gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask){
	assert(gpio);
	return mask & REG_LOAD(&(gpio->idr));
}
