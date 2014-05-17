/**
 * @file
 * @brief OMAP3 GPIO driver
 *
 * @date 1.11.13
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <kernel/printk.h>
#include <drivers/pins.h>
#include <drivers/gpio.h>
#include "omap3_gpio.h"

EMBOX_UNIT_INIT(gpio_init);

static struct gpio omap_gpio[GPIO_MODULE_CNT];

struct gpio *gpio_by_num(int num_port) {

	return &omap_gpio[num_port - 1];
}

static pin_mask_t gpio_get_pin_changed(struct gpio *gpio) {
	return (int) gpio_reg_read(gpio->base, GPIO_IRQSTATUS1);
}

int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode) {
	uint32_t l;

	assert(gpio);

	/* check if mode is incorrect */
	if (((mode & GPIO_MODE_OUT_SECTION) && (mode & GPIO_MODE_IN_SECTION)) ||
		((mode & GPIO_MODE_IN_INT_EN) && (mode & GPIO_MODE_IN_INT_DIS))) {
		return -1;
	}

	if (mode & GPIO_MODE_INPUT) {
		gpio_reg_write(gpio->base, GPIO_OE, mask);
	} else if (mode & GPIO_MODE_OUTPUT) {
		l = gpio_reg_read(gpio->base, GPIO_OE);
		gpio_reg_write(gpio->base, GPIO_OE, l & ~mask);
	}

	/* interrupt disable */
	if (mode & GPIO_MODE_IN_INT_DIS) {
		/* clear front and level detect register, if you need */
		if (mode & GPIO_MODE_INT_MODE_RISING) {
			l = gpio_reg_read(gpio->base, GPIO_RISINGDETECT);
			gpio_reg_write(gpio->base, GPIO_RISINGDETECT, l & ~mask);
		}

		if (mode & GPIO_MODE_INT_MODE_FALLING) {
			l = gpio_reg_read(gpio->base, GPIO_FALLINGDETECT);
			gpio_reg_write(gpio->base, GPIO_FALLINGDETECT, l & ~mask);
		}

		if (mode & GPIO_MODE_INT_MODE_LEVEL0) {
			l = gpio_reg_read(gpio->base, GPIO_LEVELDETECT0);
			gpio_reg_write(gpio->base, GPIO_LEVELDETECT0, l & ~mask);
		}

		if (mode & GPIO_MODE_INT_MODE_LEVEL1) {
			l = gpio_reg_read(gpio->base, GPIO_LEVELDETECT1);
			gpio_reg_write(gpio->base, GPIO_LEVELDETECT1, l & ~mask);
		}
		/* clear oe flags */
		l = gpio_reg_read(gpio->base, GPIO_IRQENABLE1);
		gpio_reg_write(gpio->base, GPIO_IRQENABLE1, l & ~mask);

		return 0;
	}

	/* set interrupt mode, if needed */
	if (mode & GPIO_MODE_INT_MODE_RISING) {
		gpio_reg_write(gpio->base, GPIO_RISINGDETECT, mask);
	}

	if (mode & GPIO_MODE_INT_MODE_FALLING) {
		gpio_reg_write(gpio->base, GPIO_FALLINGDETECT, mask);
	}

	if (mode & GPIO_MODE_INT_MODE_LEVEL0) {
		gpio_reg_write(gpio->base, GPIO_LEVELDETECT0, mask);
	}

	if (mode & GPIO_MODE_INT_MODE_LEVEL1) {
		gpio_reg_write(gpio->base, GPIO_LEVELDETECT1, mask);
	}

	/* enable interrupt */
	if (mode & GPIO_MODE_IN_INT_EN) {
		gpio_reg_write(gpio->base, GPIO_IRQENABLE1, mask);
	}

	return 0;
}

void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level) {
	uint32_t l;
	assert(gpio);
	assert((mask & ~((1 << 16) - 1)) == 0);

	if (level) {
		l = gpio_reg_read(gpio->base, GPIO_DATAOUT);
		gpio_reg_write(gpio->base, GPIO_DATAOUT, l | mask);
	} else {
		l = gpio_reg_read(gpio->base, GPIO_CLEARDATAOUT);
		gpio_reg_write(gpio->base, GPIO_CLEARDATAOUT, l | mask);
	}
}

gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask){
	assert(gpio);
	return mask & (gpio_reg_read(gpio->base, GPIO_DATAIN));
}


static int gpio_pin_number (gpio_mask_t mask) {
	int pin_nr;

	for(pin_nr = 0; pin_nr < N_PINS; pin_nr++) {
		if (mask & (1 << pin_nr)) {
			break;
		}
	}

	if (!(mask & (1 << pin_nr))) {
		return -1;
	}

	return pin_nr;
}

irq_return_t irq_gpio_handler(unsigned int irq_nr, void *data) {
	int i;
	int changed;
	struct gpio *gpio;

	gpio = &omap_gpio[GPIO_NUM_BY_IRQ(irq_nr) - 1];

	if (NULL == gpio) {
		return IRQ_NONE;
	}

	changed = gpio_get_pin_changed(gpio);

/* TODO we can calculate the number on a given mask rather than brute force */
	for (i = 0; i < N_PINS; i++) {
		if (changed & gpio->pin[i].mask) {
			gpio->pin[i].handler(irq_nr, gpio->pin[i].data);
			gpio_reg_write(gpio->base, GPIO_IRQSTATUS1,
					changed & gpio->pin[i].mask);
			gpio_reg_write(gpio->base, GPIO_IRQSTATUS2,
					changed & gpio->pin[i].mask);
		}
	}

	return IRQ_HANDLED;
}

int gpio_pin_irq_attach(struct gpio *gpio, gpio_mask_t mask,
		irq_handler_t pin_handler, int mode, void *data) {
	int pin_nr;
	assert(gpio);

	if (-1 == (pin_nr = gpio_pin_number(mask))) {
		return -1;
	}
	gpio->pin[pin_nr].handler = pin_handler;
	gpio->pin[pin_nr].mask = (1 << pin_nr);
	gpio->pin[pin_nr].data = data;
	gpio_settings(gpio, (1 << pin_nr),
			mode | GPIO_MODE_INPUT | GPIO_MODE_IN_INT_EN);

	return 0;
}

int gpio_pin_irq_detach(struct gpio *gpio, gpio_mask_t mask,
		irq_handler_t pin_handler, int mode) {
	int pin_nr;
	assert(gpio);

	if (-1 == (pin_nr = gpio_pin_number(mask))) {
		return -1;
	}

	gpio->pin[pin_nr].handler = NULL;
	gpio_settings(gpio, (1 << pin_nr), mode | GPIO_MODE_IN_INT_DIS);

	return 0;
}

static int gpio_init(void) {
	//uint32_t rev;
	int i, ret;
	struct gpio *gpio;
	char str [255];

	for(i = 0; i < GPIO_MODULE_CNT; i++) {

		gpio = &omap_gpio[i];

		memset(gpio, 0, sizeof(struct gpio));

		gpio->base = GPIO_BASE_ADDRESS(i + 1);
		gpio->gpio_nr = i;
		/*
		rev = gpio_reg_read(gpio->base, GPIO_REVISION);
		 printk("maj = %d, min = %d\n",
				GPIO_REVISION_MAJOR(rev), GPIO_REVISION_MINOR(rev)); */

		*str = 0;
		sprintf(str,"OMAP_GPIO%d", i);

		if (0 != (ret = irq_attach(GPIO_IRQ(i + 1),
				irq_gpio_handler, 0, gpio, str))) {
			return ret;
		}
	}

	return 0;
}
