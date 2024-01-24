/*
 * gpio_repo.c
 *
 *  Created on: Feb 9, 2023
 *      Author: anton
 */


#include <lib/libds/array.h>

#include <drivers/gpio/gpio_driver.h>

ARRAY_SPREAD_DEF(struct gpio_chip *, __gpio_chip_registry);
