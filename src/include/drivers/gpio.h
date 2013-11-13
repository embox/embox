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

struct gpio;

typedef __gpio_mask_t gpio_mask_t;

#define GPIO_MODE_IN_SECTION     0x00FF
#define GPIO_MODE_INPUT          (1 << 0)
#define GPIO_MODE_IN_PULL_UP     (1 << 1)
#define GPIO_MODE_IN_PULL_DOWN   (1 << 2)
#define GPIO_MODE_IN_SCHMITT     (1 << 3)

#define GPIO_MODE_OUT_SECTION    0xFF00
#define GPIO_MODE_OUTPUT         (1 << 16)
#define GPIO_MODE_OUT_PUSH_PULL  (1 << 15)
#define GPIO_MODE_OUT_OPEN_DRAIN (1 << 14)
#define GPIO_MODE_OUT_ALTERNATE  (1 << 13)

#define GPIO_MODE_IN_INTERRUPT   (1 << 31)

extern int gpio_settings(struct gpio *gpio, gpio_mask_t mask, int mode);

extern void gpio_set_level(struct gpio *gpio, gpio_mask_t mask, char level);

extern gpio_mask_t gpio_get_level(struct gpio *gpio, gpio_mask_t mask);

extern struct gpio *gpio_base_addr(int port);

extern int gpio_set_input_monitor(struct gpio *gpio, int mask,
		void *pin_handler);

#endif /* DRIVERS_GPIO_H_ */
