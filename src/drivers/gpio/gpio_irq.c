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
#include <kernel/spinlock.h>
#include <lib/libds/array.h>
#include <lib/libds/dlist.h>
#include <mem/misc/pool.h>
#include <util/log.h>

#define GPIO_IRQ_POOL_SIZE OPTION_GET(NUMBER, irq_pool_size)

struct gpio_irq {
	const struct gpio_chip *chip;
	struct dlist_head link;
	gpio_irq_hnd_t handler;
	unsigned int port;
	gpio_mask_t pins;
	void *data;
};

static DLIST_DEFINE(gpio_irq_list);

static spinlock_t gpio_irq_list_lock = SPIN_STATIC_UNLOCKED;

POOL_DEF(gpio_irq_pool, struct gpio_irq, GPIO_IRQ_POOL_SIZE);

static struct gpio_irq *gpio_irq_find(const struct gpio_chip *chip,
    unsigned int port, gpio_mask_t pins) {
	struct gpio_irq *gpio_irq;

	dlist_foreach_entry(gpio_irq, &gpio_irq_list, link) {
		if ((gpio_irq->chip == chip) && (gpio_irq->port == port)
		    && (gpio_irq->pins & pins)) {
			return gpio_irq;
		}
	}

	return NULL;
}
int gpio_irq_attach(unsigned int port, gpio_mask_t pins, gpio_irq_hnd_t handler,
    void *data) {
	const struct gpio_chip *chip;
	struct gpio_irq *gpio_irq;

	chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		return -EINVAL;
	}

	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		return -EINVAL;
	}

	gpio_irq = gpio_irq_find(chip, port, pins);
	if (gpio_irq) {
		return -EBUSY;
	}

	gpio_irq = pool_alloc(&gpio_irq_pool);
	if (!gpio_irq) {
		log_error("Cannot allocate gpio irq handler");
		return -ENOMEM;
	}

	gpio_irq->handler = handler;
	gpio_irq->data = data;
	gpio_irq->chip = chip;
	gpio_irq->port = port;
	gpio_irq->pins = pins;

	SPIN_IPL_PROTECTED_DO(&gpio_irq_list_lock,
	    dlist_add_next(dlist_head_init(&gpio_irq->link), &gpio_irq_list));

	return 0;
}

int gpio_irq_detach(unsigned int port, gpio_mask_t pins) {
	const struct gpio_chip *chip;
	struct gpio_irq *gpio_irq;

	chip = gpio_get_chip(GPIO_CHIP(port));
	if (!chip) {
		return -EINVAL;
	}

	port = GPIO_PORT(port);
	if (port >= chip->nports) {
		return -EINVAL;
	}

	gpio_irq = gpio_irq_find(chip, port, pins);
	if (!gpio_irq) {
		return -EINVAL;
	}

	SPIN_IPL_PROTECTED_DO(&gpio_irq_list_lock, dlist_del(&gpio_irq->link));
	pool_free(&gpio_irq_pool, gpio_irq);

	return 0;
}

void gpio_handle_irq(const struct gpio_chip *chip, unsigned int port,
    gpio_mask_t pins) {
	struct gpio_irq *gpio_irq;
	int i;

	for (i = 0; pins; i++, pins >>= 1) {
		if (pins & 0x1) {
			gpio_irq = gpio_irq_find(chip, port, (1 << i));
			if (gpio_irq) {
				gpio_irq->handler(gpio_irq->data);
			}
		}
	}
}
