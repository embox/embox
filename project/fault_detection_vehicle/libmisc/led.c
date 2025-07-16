/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#include "led.h"
#include <drivers/gpio.h>
#define PIN_MASK(n) (1 << n)
void led_init(struct led *led, int port, int pin) {
	led->port = port;
	led->pin = pin;
	gpio_setup_mode(led->port, PIN_MASK(led->pin), GPIO_MODE_OUT);
}

void led_on(struct led *led) {
	gpio_set(led->port, PIN_MASK(led->pin), GPIO_PIN_HIGH);
}

void led_off(struct led *led) {
	gpio_set(led->port, PIN_MASK(led->pin), GPIO_PIN_LOW);
}
