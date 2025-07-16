/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#ifndef PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_
#define PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_

#include <stdint.h>

#include <drivers/gpio.h>
#include <config/board_config.h>

struct led {
	uint16_t port;
	uint16_t pin;
};

extern void led_init(struct led *led, int port, int pin);
extern void led_on(struct led *led);
extern void led_off(struct led *led);

#endif /* PLATFORM_STM32F3_SENSORS_LIBMISC_LED_H_ */
