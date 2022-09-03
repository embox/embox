/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#include "led.h"
#include <drivers/gpio/gpio.h>

void led_init(struct led *led, int port, int pin) {
	led->port = port;
	led->pin = pin;
	gpio_setup_mode(led->port, led->pin, GPIO_MODE_OUTPUT);
}

void led_on(struct led *led) {
	gpio_set(led->port, led->pin, GPIO_PIN_HIGH);
}

void led_off(struct led *led) {
	gpio_set(led->port, led->pin, GPIO_PIN_LOW);
}
