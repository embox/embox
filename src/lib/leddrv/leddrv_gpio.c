/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.10.24
 */

#include <errno.h>
#include <stdbool.h>

#include <drivers/gpio/gpio.h>
#include <embox/unit.h>
#include <util/log.h>

#include <config/board_config.h>

#include "leddrv.h"

#define LED_DESC_VALUE(a, b) {CONF_##a##_GPIO_PORT, 1 << CONF_##a##_GPIO_PIN}, b
#define LED_DESC_LIST        MACRO_FOREACH(LED_DESC_VALUE, CONF_LED_LIST)

struct led_desc {
	int port;
	int pin;
};

static const struct led_desc leds[LEDDRV_LED_N] = {LED_DESC_LIST};

EMBOX_UNIT_INIT(leddrv_init);

static int leddrv_init(void) {
	int err;
	int i;

	for (i = 0; i < LEDDRV_LED_N; i++) {
		err = gpio_setup_mode(leds[i].port, leds[i].pin, GPIO_MODE_OUT);
		if (err) {
			log_error("gpio init failed");
			return err;
		}
		gpio_set(leds[i].port, leds[i].pin, GPIO_PIN_LOW);
	}

	return 0;
}

int leddrv_set(unsigned int nr) {
	if (nr >= LEDDRV_LED_N) {
		return -EINVAL;
	}

	gpio_set(leds[nr].port, leds[nr].pin, GPIO_PIN_HIGH);

	return 0;
}

int leddrv_clr(unsigned int nr) {
	if (nr >= LEDDRV_LED_N) {
		return -EINVAL;
	}

	gpio_set(leds[nr].port, leds[nr].pin, GPIO_PIN_LOW);

	return 0;
}

void leddrv_get_states(bool states[LEDDRV_LED_N]) {
	int i;

	for (i = 0; i < LEDDRV_LED_N; i++) {
		states[i] = !!gpio_get(leds[i].port, leds[i].pin);
	}
}

void leddrv_set_states(bool states[LEDDRV_LED_N]) {
	int i;

	for (i = 0; i < LEDDRV_LED_N; i++) {
		gpio_set(leds[i].port, leds[i].pin,
		    states[i] ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
	}
}
