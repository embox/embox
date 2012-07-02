/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include <types.h>

#include <module/embox/driver/gpio/api.h>

struct gpio;

typedef __gpio_mask_t gpio_mask_t;

#define GPIO_MODE_IN_PULL_UP (1 << 0)

#define GPIO_MODE_IN_PULL_DOWN (1 << 1)

#define GPIO_MODE_IN_SCHMITT (1 << 2)

extern int gpio_in(struct gpio *gpio, gpio_mask_t mask, int mode);

extern int gpio_out(struct gpio *gpio, gpio_mask_t mask, int mode);

extern void gpio_set(struct gpio *gpio, gpio_mask_t mask);

extern void gpio_clear(struct gpio *gpio, gpio_mask_t mask);

extern gpio_mask_t gpio_level(struct gpio *gpio, gpio_mask_t mask);

#endif /* DRIVERS_GPIO_H_ */

