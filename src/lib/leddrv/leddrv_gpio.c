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

#if LEDDRV_LED_N > 0
#define LED_DESC_INIT(a, b)                               \
	{CONF_##a##_GPIO_PORT, 1 << CONF_##a##_GPIO_PIN,      \
	    {CONF_##a##_GPIO_LEVEL, !CONF_##a##_GPIO_LEVEL}}, \
	    b
#define LED_DESC_LIST MACRO_FOREACH(LED_DESC_INIT, CONF_LED_LIST)
#else
#define LED_DESC_LIST
#error "LEDs are not enabled in board.conf.h"
#endif

#define LED_ON  0
#define LED_OFF 1

struct led_desc {
	int port;
	int pin;
	char level[2];
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
		gpio_set(leds[i].port, leds[i].pin, leds[i].level[LED_OFF]);
	}

	return 0;
}

int leddrv_led_on(unsigned int nr) {
	if (nr >= LEDDRV_LED_N) {
		return -EINVAL;
	}

	gpio_set(leds[nr].port, leds[nr].pin, leds[nr].level[LED_ON]);

	return 0;
}

int leddrv_led_off(unsigned int nr) {
	if (nr >= LEDDRV_LED_N) {
		return -EINVAL;
	}

	gpio_set(leds[nr].port, leds[nr].pin, leds[nr].level[LED_OFF]);

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
		    leds[i].level[states[i] ? LED_ON : LED_OFF]);
	}
}
