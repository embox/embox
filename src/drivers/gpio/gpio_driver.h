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
	unsigned int nports;
	unsigned int chip_id
};

/* Drivers should call this function when interrupts happened
 * on the specified pin. */
extern void gpio_handle_irq(struct gpio_chip *chip, uint8_t port,
    gpio_mask_t pins);

#include <lib/libds/array.h>
#define GPIO_CHIP_DEF(chip)                                         \
	ARRAY_SPREAD_DECLARE(struct gpio_chip *, __gpio_chip_registry); \
	ARRAY_SPREAD_ADD(__gpio_chip_registry, chip)

#endif /* DRIVERS_GPIO_DRIVER_H_ */
