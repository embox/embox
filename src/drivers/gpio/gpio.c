/**
 * @file gpio.c
 * @brief
 * @author Alexander Kalmuk
 * @date 8.02.2018
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include <drivers/gpio/gpio.h>
#include <drivers/gpio/gpio_driver.h>
#include <kernel/lthread/lthread.h>
#include <lib/libds/array.h>
#include <lib/libds/bit.h>
#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#define GPIO_CHIPS_COUNT  OPTION_GET(NUMBER, gpio_chips_count)
#define GPIO_IRQS_COUNT   OPTION_GET(NUMBER, gpio_irqs_count)
#define GPIO_HND_PRIORITY OPTION_GET(NUMBER, gpio_hnd_prio)

ARRAY_SPREAD_DECLARE(struct gpio_chip *, __gpio_chip_registry);

static struct gpio_chip *gpio_chip_registry[GPIO_CHIPS_COUNT];

int gpio_register_chip(struct gpio_chip *chip, unsigned char chip_id) {
	assert(chip);

	if (chip_id >= GPIO_CHIPS_COUNT) {
		log_error("GPIO chip id=%d exceeds max id=%d\n", chip_id,
		    GPIO_CHIPS_COUNT);
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
	if ((GPIO_CHIPS_COUNT - 1) < chip_nr) {
		return NULL;
	}
	if (NULL != gpio_chip_registry[chip_nr]) {
		return gpio_chip_registry[chip_nr];
	}

	if (ARRAY_SPREAD_SIZE(__gpio_chip_registry) != 0) {
		return __gpio_chip_registry[0];
	}
	return NULL;
}

int gpio_setup_mode(unsigned short port, gpio_mask_t pins, uint32_t mode) {
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));

	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return -EINVAL;
	}
	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		log_error("port (%d) is out of range (nports=%d)", port,
		    chip->nports - 1);
		return -EINVAL;
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
		log_error("port (%d) is out of range (nports=%d)", port,
		    chip->nports - 1);
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
		log_error("port (%d) is out of range (nports=%d)", port,
		    chip->nports - 1);
		return -EINVAL;
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
		log_error("port (%d) is out of range (nports=%d)", port,
		    chip->nports - 1);
		return;
	}
	assert(chip->get && chip->set);
	state = chip->get(port, pins);
	chip->set(port, state, 0);         /* Down all pins that were HIGH */
	chip->set(port, ~state & pins, 1); /* Up all pins that were LOW */
}

#if (GPIO_IRQS_COUNT > 0)
#define DO_IPL_LOCKED(job)       \
	{                            \
		ipl_t _ipl = ipl_save(); \
		job;                     \
		ipl_restore(_ipl);       \
	}

struct gpio_irq_handler {
	struct gpio_chip *chip;
	unsigned char port;
	uint32_t pin;
	void *data;
	void (*handler)(void *);
	struct dlist_head link;
};

static int gpio_lthread_irq_hnd(struct lthread *self);
static LTHREAD_DEF(gpio_lthread, gpio_lthread_irq_hnd, GPIO_HND_PRIORITY);

POOL_DEF(gpio_irq_pool, struct gpio_irq_handler, GPIO_IRQS_COUNT);

static DLIST_DEFINE(gpio_irq_list);
static DLIST_DEFINE(gpio_pending_irq_list);

int gpio_irq_attach(unsigned short port, gpio_mask_t pin,
    void (*pin_handler)(void *), void *data) {
	struct gpio_irq_handler *gpio_hnd;
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));

	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return -EINVAL;
	}
	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		log_error("port (%d) is out of range (nports=%d)", port,
		    chip->nports - 1);
		return -EINVAL;
	}
	gpio_hnd = pool_alloc(&gpio_irq_pool);
	if (!gpio_hnd) {
		log_error("chip=%d, gpio_hnd=%p", GPIO_CHIP(port), gpio_hnd);
		return -ENOMEM;
	}

	gpio_hnd->chip = chip;
	gpio_hnd->port = port;
	gpio_hnd->pin = pin;
	gpio_hnd->data = data;
	gpio_hnd->handler = pin_handler;

	DO_IPL_LOCKED(
	    dlist_add_next(dlist_head_init(&gpio_hnd->link), &gpio_irq_list););

	return 0;
}

int gpio_irq_detach(unsigned short port, uint32_t pin) {
	struct gpio_irq_handler *gpio_hnd;
	struct gpio_chip *chip = gpio_get_chip(GPIO_CHIP(port));

	if (!chip) {
		log_error("Chip not found, chip=%d", GPIO_CHIP(port));
		return -EINVAL;
	}

	port = GPIO_PORT(port);

	dlist_foreach_entry_safe(gpio_hnd, &gpio_irq_list, link) {
		if ((gpio_hnd->chip == chip) && (gpio_hnd->port == port)
		    && (gpio_hnd->pin == pin)) {
			DO_IPL_LOCKED(dlist_del(&gpio_hnd->link));
			pool_free(&gpio_irq_pool, gpio_hnd);

			return 0;
		}
	}

	return -1;
}

static int gpio_lthread_irq_hnd(struct lthread *self) {
	struct gpio_irq_handler *gpio_hnd;

	dlist_foreach_entry_safe(gpio_hnd, &gpio_pending_irq_list, link) {
		gpio_hnd->handler(gpio_hnd->data);

		/* Move handler back to gpio list. */
		DO_IPL_LOCKED(dlist_del(&gpio_hnd->link);
		              dlist_add_prev(&gpio_hnd->link, &gpio_irq_list););
	}

	return 0;
}

void gpio_handle_irq(struct gpio_chip *chip, uint8_t port, gpio_mask_t pins) {
	struct gpio_irq_handler *gpio_hnd;
	int pin;
	int pending = 0;

	bit_foreach(pin, pins) {
		dlist_foreach_entry_safe(gpio_hnd, &gpio_irq_list, link) {
			if ((gpio_hnd->chip == chip) && (gpio_hnd->port == port)
			    && (gpio_hnd->pin == (1 << pin))) {
				/* Move handler to pending list to be processed in lthread. */
				DO_IPL_LOCKED(
				    dlist_del(&gpio_hnd->link);
				    dlist_add_prev(&gpio_hnd->link, &gpio_pending_irq_list););

				pending = 1;
			}
		}
	}

	if (pending) {
		lthread_launch(&gpio_lthread);
	}
}
#endif
