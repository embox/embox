/**
 * @file
 * @brief OMAP3 GPIO driver
 *
 * @date 1.11.13
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <drivers/pins.h>
#include <drivers/gpio.h>
#include "omap3_gpio.h"

EMBOX_UNIT_INIT(gpio_init);

#define N_PINS 8 * (sizeof(int))

struct handler_item_t {
	int mask;
	pin_handler_t handler;
};

pin_mask_t gpio_get_pin_changed(struct gpio *gpio) {
	return (int) gpio_reg_read(gpio, GPIO_IRQSTATUS1);
}

int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode) {
	uint32_t l;

	assert(gpio);

	if ((mode & GPIO_MODE_OUT_SECTION) &&
		(mode & GPIO_MODE_IN_SECTION)) { /* mode is incorrect */
		return -1;
	}

	if (mode & GPIO_MODE_INPUT) {
		gpio_reg_write(gpio, GPIO_OE, mask);
	} else if (mode & GPIO_MODE_OUTPUT) {
		l = gpio_reg_read(gpio, GPIO_OE);
		gpio_reg_write(gpio, GPIO_OE, l & ~mask);
	}

	if (mode & GPIO_MODE_IN_INTERRUPT) {
		gpio_reg_write(gpio, GPIO_IRQENABLE1, mask);
		gpio_reg_write(gpio, GPIO_IRQENABLE2, mask);
	}

	return 0;
}

void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level) {
	uint32_t l;
	assert(gpio);
	assert((mask & ~((1 << 16) - 1)) == 0);

	if(level) {
		l = gpio_reg_read(gpio, GPIO_DATAOUT);
		gpio_reg_write(gpio, GPIO_DATAOUT, l | mask);
	} else {
		l = gpio_reg_read(gpio, GPIO_CLEARDATAOUT);
		gpio_reg_write(gpio, GPIO_CLEARDATAOUT, l | mask);
	}
}

gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask){
	assert(gpio);
	return mask & (gpio_reg_read(gpio, GPIO_DATAIN));
}

static struct handler_item_t handlers[N_PINS];

static int n_handler = 0;

irq_return_t irq_pin_handler(unsigned int irq_nr, void *data) {
	int i;
	int current;
	int changed;
	struct gpio *gpio;

	gpio = GPIO_6; /*FIXME */

	current = gpio_get_level(gpio, ~0);
	changed = gpio_get_pin_changed(gpio);

	if (irq_nr == 34) {
		printk(">>> irq_pin_handler\n");
	}

	for (i = 0; i < n_handler; i++) {
		if (changed & handlers[i].mask) {
			handlers[i].handler(handlers[i].mask & current, handlers[i].mask);
		}
	}

	gpio_reg_write(gpio, GPIO_IRQSTATUS1, 1 << 16);
	gpio_reg_write(gpio, GPIO_IRQSTATUS2, 1 << 16);

	return IRQ_HANDLED;
}

static int gpio_init(void) {
	uint32_t rev;
	struct gpio *gpio;

	gpio = GPIO_6; /*FIXME */

	rev = gpio_reg_read(gpio, GPIO_REVISION);

	printk("maj = %d, min = %d\n", GPIO_REVISION_MAJOR(rev), GPIO_REVISION_MINOR(rev));

#if 0
	for (i = 0; i < GPIO_MODULE_CNT; i++) {
		irq_attach(GPIO_IRQ(i), irq_pin_handler, 0, NULL, "OMAP3 GPIO pins");
		gpio_reg_write(i, GPIO_IRQENABLE1, mask);
	}
#endif

	gpio_reg_write(gpio, GPIO_RISINGDETECT, (1 << 16));
	gpio_reg_write(gpio, GPIO_FALLINGDETECT, 0);
	gpio_reg_write(gpio, GPIO_LEVELDETECT0, 0);
	gpio_reg_write(gpio, GPIO_LEVELDETECT1, 0);
	return irq_attach(GPIO_IRQ(6), irq_pin_handler, 0, NULL, "OMAP6 GPIO6 pins");
}

struct gpio *gpio_base_addr(int port) {

	return (struct gpio *) GPIO_BASE_ADDRESS(port);
}

int gpio_set_input_monitor(struct gpio *gpio, int mask,
		void *pin_handler) {
	assert(gpio);

	handlers[n_handler].handler = pin_handler;
	handlers[n_handler].mask = mask;
	n_handler++;
	gpio_settings(gpio, mask, GPIO_MODE_INPUT);
	gpio_settings(gpio, mask, GPIO_MODE_IN_INTERRUPT);

	return 0;
}
