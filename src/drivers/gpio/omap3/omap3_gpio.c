/**
 * @file
 * @brief OMAP3 GPIO driver
 *
 * @date 1.11.13
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <drivers/gpio.h>
#include <drivers/pins.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <kernel/printk.h>

#include "omap3_gpio.h"

#define GPIO_CHIP_ID OPTION_GET(NUMBER, gpio_chip_id)

EMBOX_UNIT_INIT(gpio_init);

static const struct gpio_chip omap3_gpio_chip;

static pin_mask_t gpio_get_pin_changed(unsigned int port) {
	return (int)gpio_reg_read(GPIO_BASE_ADDRESS(port + 1), GPIO_IRQSTATUS1);
}

static int omap3_gpio_setup_mode(unsigned int port, gpio_mask_t mask,
    uint32_t mode) {
	uint32_t l;
	uint32_t gpio_base = GPIO_BASE_ADDRESS(port + 1);

	/* check if mode is incorrect */
	if (((mode & GPIO_MODE_OUT_SECTION) && (mode & GPIO_MODE_IN_SECTION))
	    || ((mode & GPIO_MODE_IN_INT_EN) && (mode & GPIO_MODE_IN_INT_DIS))) {
		return -1;
	}

	if (mode & GPIO_MODE_IN) {
		gpio_reg_write(gpio_base, GPIO_OE, mask);
	}
	else if (mode & GPIO_MODE_OUT) {
		l = gpio_reg_read(gpio_base, GPIO_OE);
		gpio_reg_write(gpio_base, GPIO_OE, l & ~mask);
	}

	/* interrupt disable */
	if (mode & GPIO_MODE_IN_INT_DIS) {
		/* clear front and level detect register, if you need */
		if (mode & GPIO_MODE_INT_MODE_RISING) {
			l = gpio_reg_read(gpio_base, GPIO_RISINGDETECT);
			gpio_reg_write(gpio_base, GPIO_RISINGDETECT, l & ~mask);
		}

		if (mode & GPIO_MODE_INT_MODE_FALLING) {
			l = gpio_reg_read(gpio_base, GPIO_FALLINGDETECT);
			gpio_reg_write(gpio_base, GPIO_FALLINGDETECT, l & ~mask);
		}

		if (mode & GPIO_MODE_INT_MODE_LEVEL0) {
			l = gpio_reg_read(gpio_base, GPIO_LEVELDETECT0);
			gpio_reg_write(gpio_base, GPIO_LEVELDETECT0, l & ~mask);
		}

		if (mode & GPIO_MODE_INT_MODE_LEVEL1) {
			l = gpio_reg_read(gpio_base, GPIO_LEVELDETECT1);
			gpio_reg_write(gpio_base, GPIO_LEVELDETECT1, l & ~mask);
		}
		/* clear oe flags */
		l = gpio_reg_read(gpio_base, GPIO_IRQENABLE1);
		gpio_reg_write(gpio_base, GPIO_IRQENABLE1, l & ~mask);

		return 0;
	}

	/* set interrupt mode, if needed */
	if (mode & GPIO_MODE_INT_MODE_RISING) {
		gpio_reg_write(gpio_base, GPIO_RISINGDETECT, mask);
	}

	if (mode & GPIO_MODE_INT_MODE_FALLING) {
		gpio_reg_write(gpio_base, GPIO_FALLINGDETECT, mask);
	}

	if (mode & GPIO_MODE_INT_MODE_LEVEL0) {
		gpio_reg_write(gpio_base, GPIO_LEVELDETECT0, mask);
	}

	if (mode & GPIO_MODE_INT_MODE_LEVEL1) {
		gpio_reg_write(gpio_base, GPIO_LEVELDETECT1, mask);
	}

	/* enable interrupt */
	if (mode & GPIO_MODE_IN_INT_EN) {
		gpio_reg_write(gpio_base, GPIO_IRQENABLE1, mask);
	}

	return 0;
}

static void omap3_gpio_set(unsigned int port, gpio_mask_t mask, int level) {
	uint32_t l;
	uint32_t gpio_base = GPIO_BASE_ADDRESS(port + 1);

	assert((mask & ~((1 << 16) - 1)) == 0);

	if (level) {
		l = gpio_reg_read(gpio_base, GPIO_DATAOUT);
		gpio_reg_write(gpio_base, GPIO_DATAOUT, l | mask);
	}
	else {
		l = gpio_reg_read(gpio_base, GPIO_CLEARDATAOUT);
		gpio_reg_write(gpio_base, GPIO_CLEARDATAOUT, l | mask);
	}
}

gpio_mask_t omap3_gpio_get(unsigned int port, gpio_mask_t mask) {
	uint32_t gpio_base = GPIO_BASE_ADDRESS(port + 1);
	return mask & (gpio_reg_read(gpio_base, GPIO_DATAIN));
}

irq_return_t irq_gpio_handler(unsigned int irq_nr, void *data) {
	int changed;
	unsigned int port = GPIO_NUM_BY_IRQ(irq_nr) - 1;
	uint32_t gpio_base = GPIO_BASE_ADDRESS(port + 1);

	changed = gpio_get_pin_changed(port);
	gpio_reg_write(gpio_base, GPIO_IRQSTATUS1, changed);
	gpio_reg_write(gpio_base, GPIO_IRQSTATUS2, changed);

	gpio_handle_irq(&omap3_gpio_chip, port, changed);

	return IRQ_HANDLED;
}

static const struct gpio_chip omap3_gpio_chip = {
    .setup_mode = omap3_gpio_setup_mode,
    .get = omap3_gpio_get,
    .set = omap3_gpio_set,
    .nports = GPIO_MODULE_CNT,
    .chip_id = GPIO_CHIP_ID,
};

GPIO_CHIP_DEF(&omap3_gpio_chip);

static int gpio_init(void) {
	int err;
	int i;
	char str[32];

	for (i = 0; i < GPIO_MODULE_CNT; i++) {
		sprintf(str, "OMAP_GPIO%d", i);

		err = irq_attach(GPIO_IRQ(i + 1), irq_gpio_handler, 0, NULL, str);
		if (err) {
			return err;
		}
	}

	return 0;
}
