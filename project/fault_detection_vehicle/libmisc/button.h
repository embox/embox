/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBMISC_BUTTON_H_
#define PLATFORM_STM32F3_SENSORS_LIBMISC_BUTTON_H_

#include <stdint.h>

struct button {
	uint16_t port;
	uint16_t pin;
};

extern void button_init(struct button *button, int port, int pin);
extern void button_wait_set(struct button * button);
extern void button_wait_reset(struct button * button);

#include <drivers/gpio.h>

#define STM_USER_BUTTON_PIN                  0
#define STM_USER_BUTTON_GPIO_PORT            GPIO_PORT_A
//#define USER_BUTTON_EXTI_IRQn            EXTI0_IRQn

#endif /* PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_ */

