/**
 * @file gpio.c
 * @brief
 * @author Alexander Kalmuk
 * @date 8.02.2018
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/gpio.h>
#include <lib/libds/array.h>
#include <util/log.h>

ARRAY_SPREAD_DEF(const struct gpio_chip *, __gpio_chip_registry);

const struct gpio_chip *gpio_get_chip(unsigned int chip_id) {
	const struct gpio_chip *chip;

	array_spread_foreach(chip, __gpio_chip_registry) {
		if (chip->chip_id == chip_id) {
			return chip;
		}
	}

	log_error("gpio chip not found, chip=%d", chip_id);

	return NULL;
}

int gpio_setup_mode(unsigned int port, gpio_mask_t pins, uint32_t mode) {
	const struct gpio_chip *chip;

	chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		return -EINVAL;
	}

	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		return -EINVAL;
	}

	assert(chip->setup_mode);
	return chip->setup_mode(port, pins, mode);
}

void gpio_set(unsigned int port, gpio_mask_t pins, int level) {
	const struct gpio_chip *chip;

	chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		return;
	}

	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		return;
	}

	assert(chip->set);
	chip->set(port, pins, level);
}

gpio_mask_t gpio_get(unsigned int port, gpio_mask_t pins) {
	const struct gpio_chip *chip;

	chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		return 0;
	}

	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		return 0;
	}

	assert(chip->get);
	return chip->get(port, pins);
}

void gpio_toggle(unsigned int port, gpio_mask_t pins) {
	gpio_mask_t state;

	state = gpio_get(port, pins);
	gpio_set(port, state, GPIO_PIN_LOW);
	gpio_set(port, ~state & pins, GPIO_PIN_HIGH);
}
