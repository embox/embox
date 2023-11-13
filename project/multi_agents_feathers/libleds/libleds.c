/**
 * @file
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

#include <drivers/gpio/gpio_driver.h>

#include <feather/libleds.h>

struct leddrv_pin_desc {
	int gpio; /**< port */
	int pin; /**< pin mask */
};

static const struct leddrv_pin_desc leds[] = {
	#include <leds_config.inc>
};

void libleds_init(void) {
	int i;
	for (i = 0; i < libleds_leds_quantity(); i++) {
		gpio_setup_mode(leds[i].gpio, leds[i].pin, GPIO_MODE_OUT);
	}
}

int libleds_leds_quantity(void) {
	return sizeof(leds)/sizeof(leds[0]);
}

void libleds_led_on(int led_num) {
	gpio_set(leds[led_num].gpio, leds[led_num].pin, GPIO_PIN_HIGH);
}

void libleds_led_off(int led_num) {
	gpio_set(leds[led_num].gpio, leds[led_num].pin,GPIO_PIN_LOW);
}
