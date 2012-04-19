/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    07.04.2012
 */

#include <types.h>
#include <assert.h>

#define RCC_APB1RSTR  0x40021010
#define RCC_APB1PWR   0x10000000
#define RCC_APB2ENR   0x40021018
#define RCC_APB2GPIOC 0x00000010

#define RCC_APB2GPIOx 0x000001fc

#include <drivers/gpio.h>

#include <hal/reg.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(stm32_gpio_init);

static int stm32_gpio_init(void) {
	REG_STORE(RCC_APB1RSTR,RCC_APB1PWR);
	REG_STORE(RCC_APB2ENR,RCC_APB2GPIOx);
	return 0;
}

static void set_state(struct gpio *gpio, gpio_mask_t mask, int new_state) {
	volatile unsigned int *reg = &(gpio->crl);
	assert((new_state & ~(0xf)) == 0);
	for (int lo = 0; lo < 2; lo++) {
		unsigned int gpio_state = REG_LOAD(reg);
		for (int i = 0; i < 8; i++) {
			if (mask & (1 << i)) {
				gpio_state = (gpio_state & ~(0xf<< (i * 4))) | new_state << (4 * i);
			}
		}
		REG_STORE(reg, gpio_state);
		reg = &(gpio->crh);
		mask >>= 8;
	}

}

void gpio_conf_out(struct gpio *gpio, gpio_mask_t mask) {
	set_state(gpio, mask, 1);
}


void gpio_conf_in(struct gpio *gpio, gpio_mask_t mask) {
	set_state(gpio, mask, 4);
}

void gpio_out_set(struct gpio *gpio, gpio_mask_t mask) {
	gpio_conf_out(gpio, mask);
	assert((mask & ~((1 << 16) - 1)) == 0);
	REG_STORE(&(gpio->odr), mask);
}

gpio_mask_t gpio_out_get(struct gpio *gpio, gpio_mask_t mask) {
	return REG_LOAD(&(gpio->idr));
}

