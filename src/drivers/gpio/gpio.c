/**
 * @file gpio.c
 * @brief
 * @author Alexander Kalmuk
 * @date 8.02.2018
 */

#include <errno.h>
#include <assert.h>

#include <mem/misc/pool.h>
#include <util/dlist.h>
#include <util/log.h>

#include <drivers/gpio/gpio.h>
#include <drivers/gpio/gpio_driver.h>

#define GPIO_CHIPS_COUNT OPTION_GET(NUMBER,gpio_chips_count)
#define GPIO_IRQS_COUNT OPTION_GET(NUMBER,gpio_irqs_count)

struct gpio_irq_handler {
	struct gpio_chip *chip;
	unsigned char port;
	gpio_mask_t pins;
	void *data;
	irq_handler_t handler;
	struct dlist_head link;
};

static struct gpio_chip *gpio_chip_registry[GPIO_CHIPS_COUNT];

POOL_DEF(gpio_irq_pool, struct gpio_irq_handler, GPIO_IRQS_COUNT);
static DLIST_DEFINE(gpio_irq_list);

int gpio_register_chip(struct gpio_chip *chip, unsigned char chip_id) {
	assert(chip);

	if (chip_id >= GPIO_CHIPS_COUNT) {
		log_error("GPIO chip id=%d exceeds max id=%d\n",
			chip_id, GPIO_CHIPS_COUNT);
		return -1;
	}
	if (gpio_chip_registry[chip_id]) {
		log_error("GPIO chip id%d is registered already\n", chip_id);
		return -1;
	}
	gpio_chip_registry[chip_id] = chip;
	return 0;
}

static struct gpio_chip *gpio_get_chip(unsigned char chip_nr) {
	return chip_nr < GPIO_CHIPS_COUNT ? gpio_chip_registry[chip_nr] : NULL;
}

void gpio_handle_irq(struct gpio_chip *chip, unsigned int irq_nr,
		unsigned char port, gpio_mask_t changed_pins) {
	struct gpio_irq_handler *gpio_hnd;

	dlist_foreach_entry(gpio_hnd, &gpio_irq_list, link) {
		if ((gpio_hnd->chip == chip)
				&& (gpio_hnd->port == port)
				&& (gpio_hnd->pins & changed_pins)) {
			gpio_hnd->handler(irq_nr, gpio_hnd->data);
		}
	}
}

int gpio_setup_mode(unsigned short port, gpio_mask_t pins, int mode) {
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return -EACCES;
	}
	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		log_error("port (%d) > chip->nports (%d)",
			chip, port, chip->nports);
		return -EACCES;
	}
	assert(chip->setup_mode);
	return chip->setup_mode(port, pins, mode);
}

void gpio_set(unsigned short port, gpio_mask_t pins, char level) {
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return;
	}
	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		log_error("port (%d) > chip->nports (%d)",
			chip, port, chip->nports);
		return;
	}
	assert(chip->set);
	chip->set(port, pins, level);
}

gpio_mask_t gpio_get(unsigned short port, gpio_mask_t pins) {
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return -1;
	}
	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		log_error("port (%d) > chip->nports (%d)",
			chip, port, chip->nports);
		return -1;
	}
	assert(chip->get);
	return chip->get(port, pins);
}

void gpio_toggle(unsigned short port, gpio_mask_t pins) {
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));
	gpio_mask_t state;
	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return;
	}
	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		log_error("port (%d) > chip->nports (%d)",
			chip, port, chip->nports);
		return;
	}
	assert(chip->get && chip->set);
	state = chip->get(port, pins);
	chip->set(port, state, 0); /* Down all pins that were HIGH */
	chip->set(port, ~state & pins, 1); /* Up all pins that were LOW */
}

int gpio_irq_attach(unsigned short port, gpio_mask_t pins,
		irq_handler_t pin_handler, void *data) {
	struct gpio_irq_handler *gpio_hnd;
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return -EINVAL;
	}
	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		log_error("port (%d) > chip->nports (%d)",
			chip, port, chip->nports);
		return -EINVAL;
	}
	gpio_hnd = pool_alloc(&gpio_irq_pool);
	if (!gpio_hnd) {
		log_error("chip=%d, gpio_hnd=%p",
			GPIO_CHIP(port), gpio_hnd);
		return -ENOMEM;
	}
	dlist_add_next(dlist_head_init(&gpio_hnd->link), &gpio_irq_list);

	gpio_hnd->chip = chip;
	gpio_hnd->port = port;
	gpio_hnd->pins = pins;
	gpio_hnd->data = data;
	gpio_hnd->handler = pin_handler;

	return 0;
}
