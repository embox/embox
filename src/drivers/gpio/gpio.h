/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include <module/embox/driver/gpio/api.h>
#include <kernel/irq.h>

struct gpio;

typedef __gpio_mask_t gpio_mask_t;

#define GPIO_MODE_IN_SECTION          0x000000FF
#define GPIO_MODE_OUT_SECTION         0x0000FF00
#define GPIO_MODE_INT_SECTION         0xFF000000

#define GPIO_MODE_INPUT             (1 << 0)
#define GPIO_MODE_IN_PULL_UP        (1 << 1)
#define GPIO_MODE_IN_PULL_DOWN      (1 << 2)
#define GPIO_MODE_IN_SCHMITT        (1 << 3)

#define GPIO_MODE_OUTPUT           (1 << 16)
#define GPIO_MODE_OUT_PUSH_PULL    (1 << 15)
#define GPIO_MODE_OUT_OPEN_DRAIN   (1 << 14)
#define GPIO_MODE_OUT_ALTERNATE    (1 << 13)

#define GPIO_MODE_IN_INT_EN        (1 << 31)
#define GPIO_MODE_INT_MODE_RISING  (1 << 30)
#define GPIO_MODE_INT_MODE_FALLING (1 << 29)
#define GPIO_MODE_INT_MODE_LEVEL0  (1 << 28)
#define GPIO_MODE_INT_MODE_LEVEL1  (1 << 27)
#define GPIO_MODE_IN_INT_DIS       (1 << 25)

extern struct gpio *gpio_by_num(int num_port);

extern int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode);

extern void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level);

extern gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask);

extern int gpio_pin_irq_attach(struct gpio *gpio, gpio_mask_t mask,
		irq_handler_t pin_handler, int mode, void *data);

extern int gpio_pin_irq_detach(struct gpio *gpio, gpio_mask_t mask,
		irq_handler_t pin_handler, int mode);

#endif /* DRIVERS_GPIO_H_ */
