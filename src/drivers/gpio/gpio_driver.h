/**
 * @file
 * @brief
 *
 * @author  Alexander Kalmuk
 * @date    11.02.2019
 */

#ifndef DRIVERS_GPIO_DRIVER_H_
#define DRIVERS_GPIO_DRIVER_H_

#include <drivers/gpio/gpio.h>

struct gpio_chip {
	int (*setup_mode)(unsigned char port, gpio_mask_t pins, int mode);
	void (*set)(unsigned char port, gpio_mask_t pins, char level);
	gpio_mask_t (*get)(unsigned char port, gpio_mask_t pins);
	unsigned char nports;
};

extern int gpio_register_chip(struct gpio_chip *gpio_chip, unsigned char chip_id);

/* Drivers should call this function when interrupts happened
 * on the specified pins */
extern void gpio_handle_irq(struct gpio_chip *gpio_chip, unsigned int irq_nr,
		unsigned char port, gpio_mask_t changed_pins);

#endif /* DRIVERS_GPIO_DRIVER_H_ */
