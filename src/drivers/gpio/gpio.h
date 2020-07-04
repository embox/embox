/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include <kernel/irq.h>

typedef volatile unsigned long gpio_mask_t;

#define GPIO_MODE_IN_SECTION          0x000000FF
#define GPIO_MODE_OUT_SECTION         0x0000FF00
#define GPIO_MODE_INT_SECTION         0xFF000000

#ifndef GPIO_MODE_INPUT
#define GPIO_MODE_INPUT             (1 << 0)
#endif

#ifndef GPIO_MODE_IN_PULL_UP
#define GPIO_MODE_IN_PULL_UP        (1 << 1)
#endif

#ifndef GPIO_MODE_IN_PULL_DOWN
#define GPIO_MODE_IN_PULL_DOWN      (1 << 2)
#endif

#ifndef GPIO_MODE_IN_SCHMITT
#define GPIO_MODE_IN_SCHMITT        (1 << 3)
#endif

#ifndef GPIO_MODE_OUTPUT
#define GPIO_MODE_OUTPUT            (1 << 16)
#endif

#ifndef GPIO_MODE_OUT_PUSH_PULL
#define GPIO_MODE_OUT_PUSH_PULL     (1 << 15)
#endif

#ifndef GPIO_MODE_OUT_OPEN_DRAIN
#define GPIO_MODE_OUT_OPEN_DRAIN    (1 << 14)
#endif

#ifndef GPIO_MODE_OUT_ALTERNATE
#define GPIO_MODE_OUT_ALTERNATE     (1 << 13)
#endif

/* Alternate function number. Bits: 17 ... 20 */
#define GPIO_GET_ALTERNATE(mode) (((mode) & 0x1e0000) >> 17)
#define GPIO_ALTERNATE(af) (((af) << 17) & 0x1e0000)

#ifndef GPIO_MODE_IN_INT_EN
#define GPIO_MODE_IN_INT_EN         (1 << 31)
#endif

#ifndef GPIO_MODE_INT_MODE_RISING
#define GPIO_MODE_INT_MODE_RISING   (1 << 30)
#endif

#ifndef GPIO_MODE_INT_MODE_FALLING
#define GPIO_MODE_INT_MODE_FALLING  (1 << 29)
#endif

#ifndef GPIO_MODE_INT_MODE_LEVEL0
#define GPIO_MODE_INT_MODE_LEVEL0   (1 << 28)
#endif

#ifndef GPIO_MODE_INT_MODE_LEVEL1
#define GPIO_MODE_INT_MODE_LEVEL1   (1 << 27)
#endif

#ifndef GPIO_MODE_IN_INT_DIS
#define GPIO_MODE_IN_INT_DIS        (1 << 25)
#endif

#define GPIO_PORT(port)  ((port) & 0xf)
#define GPIO_CHIP(port) (((port) & 0xf0) >> 4)

#define GPIO_CHIP0 (0 << 4)
#define GPIO_CHIP1 (1 << 4)

#define GPIO_PORT_A 0
#define GPIO_PORT_B 1
#define GPIO_PORT_C 2
#define GPIO_PORT_D 3
#define GPIO_PORT_E 4
#define GPIO_PORT_F 5
#define GPIO_PORT_G 6
#define GPIO_PORT_H 7
#define GPIO_PORT_I 8
#define GPIO_PORT_J 9
#define GPIO_PORT_K 10

#define GPIO_PIN_LOW  0
#define GPIO_PIN_HIGH 1

/* Each GPIO port must be (GPIO_CHIP_N | GPIO_PORT_M).
 * Be default, GPIO_PORT_N is default system GPIO controller number 0 */

extern int gpio_setup_mode(unsigned short port, gpio_mask_t pins, int mode);

extern void gpio_set(unsigned short port, gpio_mask_t pins, char level);

/* Toggles (inverts) specified pins, that is levels will be changed
 * 1 -> 0 and 0 -> 1 */
extern void gpio_toggle(unsigned short port, gpio_mask_t pins);

extern gpio_mask_t gpio_get(unsigned short port, gpio_mask_t pins);

extern int gpio_irq_attach(unsigned short port, gpio_mask_t pins,
		irq_handler_t pin_handler, void *data);

#endif /* DRIVERS_GPIO_H_ */
