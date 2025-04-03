/*
 *
 * @date 13.07.2015
 * @author Alex Kalmuk
 */

#include "button.h"
#include <drivers/gpio.h>

void button_init(struct button *button, int port, int pin) {
	button->port = port;
	button->pin = pin;
	gpio_setup_mode(button->port, button->pin, GPIO_MODE_IN);
}

void button_wait_set(struct button * button) {
	while(gpio_get(button->port, button->pin) == 0) {
		;
	}
}

void button_wait_reset(struct button * button) {
	while(gpio_get(button->port, button->pin) != 0) {
			;
		}
}
