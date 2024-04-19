/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.07.2012
 */

#ifndef DRIVERS_GPIO_H_
#define DRIVERS_GPIO_H_

#include <stdint.h>

#include <sys/cdefs.h>

typedef volatile unsigned long gpio_mask_t;

#define GPIO_MODE_IN_SECTION          0x000000FF
#define GPIO_MODE_OUT_SECTION         0x0000FF00
/* Alternate function number. Bits: 17 ... 22 */
#define GPIO_MODE_INT_SECTION         0xFF000000

/* 4 .. 7 port number*/
#define GPIO_PORT(port)  ((port) & 0xf)
#define GPIO_CHIP(port) (((port) & 0xf0) >> 4)

#define GPIO_CHIP0 (0 << 4)
#define GPIO_CHIP1 (1 << 4)

/* #define GPIO_MODE_IN_SECTION          0x000000FF */
#define GPIO_MODE_IN                (1 << 0)
#define GPIO_MODE_IN_PULL_UP        (1 << 1)
#define GPIO_MODE_IN_PULL_DOWN      (1 << 2)
#define GPIO_MODE_IN_SCHMITT        (1 << 3)

/* #define GPIO_MODE_OUT_SECTION         0x0000FF00 */
#define GPIO_MODE_OUT               (1 << 15)
#define GPIO_MODE_OUT_PUSH_PULL     (1 << 14)
#define GPIO_MODE_OUT_OPEN_DRAIN    (1 << 13)
#define GPIO_MODE_OUT_ALTERNATE     (1 << 12)
#define GPIO_MODE_VDD_LEVEL         (1 << 11)


/* Alternate function number. Bits: 17 ... 22 */
#define GPIO_GET_ALTERNATE(mode) (((mode) & 0x7e0000) >> 17)
#define GPIO_ALTERNATE(af) (((af) << 17) & 0x7e0000)

#define GPIO_MODE_INT_SECTION         0xFF000000
#define GPIO_MODE_IN_INT_EN         (1u << 31)
#define GPIO_MODE_INT_MODE_RISING   (1u << 30)
#define GPIO_MODE_INT_MODE_FALLING  (1u << 29)
#define GPIO_MODE_INT_MODE_LEVEL0   (1u << 28)
#define GPIO_MODE_INT_MODE_LEVEL1   (1u << 27)
#define GPIO_MODE_IN_INT_DIS        (1u << 26)
#define GPIO_MODE_INT_MODE_RISING_FALLING \
	(GPIO_MODE_INT_MODE_RISING | GPIO_MODE_INT_MODE_FALLING)


#define GPIO_PORT_A     0
#define GPIO_PORT_B     1
#define GPIO_PORT_C     2
#define GPIO_PORT_D     3
#define GPIO_PORT_E     4
#define GPIO_PORT_F     5
#define GPIO_PORT_G     6
#define GPIO_PORT_H     7
#define GPIO_PORT_I     8
#define GPIO_PORT_J     9
#define GPIO_PORT_K    10

#define GPIO_PIN_LOW    0
#define GPIO_PIN_HIGH   1

__BEGIN_DECLS
/* Each GPIO port must be (GPIO_CHIP_N | GPIO_PORT_M).
 * Be default, GPIO_PORT_N is default system GPIO controller number 0 */

extern int gpio_setup_mode(unsigned short port, gpio_mask_t pins, int mode);

extern void gpio_set(unsigned short port, gpio_mask_t pins, char level);

/* Toggles (inverts) specified pins, that is levels will be changed
 * 1 -> 0 and 0 -> 1 */
extern void gpio_toggle(unsigned short port, gpio_mask_t pins);

extern gpio_mask_t gpio_get(unsigned short port, gpio_mask_t pins);

extern int gpio_irq_attach(unsigned short port, uint32_t pin,
		void (*pin_handler)(void *), void *data);

extern int gpio_irq_detach(unsigned short port, uint32_t pin);
__END_DECLS

#endif /* DRIVERS_GPIO_H_ */
