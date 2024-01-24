/**
 * @file
 * @brief Based on platform/stm32f4_iocontrol/cmds/libleddrv_ll.c
 *
 * @author  Alexander Kalmuk
 * @date    20.08.2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <lib/libds/array.h>
#include <drivers/gpio/gpio.h>

#include <led_driver_lib.h>

struct led_desc {
	int gpio; /**< port */
	int pin; /**< pin mask */
};

static const struct led_desc leds[] = {
	#include <leds_config.inc>
};

#define LEDS_N ARRAY_SIZE(leds)

int led_driver_init(void) {
	int i;
	for (i = 0; i < LEDS_N; i++) {
		if (0 != gpio_setup_mode(leds[i].gpio, leds[i].pin,
				GPIO_MODE_OUT)) {
			return -EINVAL;
		}
	}
	return 0;
}

int led_driver_set(unsigned int nr) {
	if (nr >= LEDS_N) {
		return -EINVAL;
	}
	gpio_set(leds[nr].gpio, leds[nr].pin, GPIO_PIN_HIGH);
	return 0;
}

int led_driver_clear(unsigned int nr) {
	if (nr >= LEDS_N) {
		return -EINVAL;
	}
	gpio_set(leds[nr].gpio, leds[nr].pin, GPIO_PIN_LOW);
	return 0;
}

int led_driver_get_states(unsigned char *states, size_t n) {
	int i;
	n = n > LEDS_N ? LEDS_N : n;
	for (i = 0; i < n; i++) {
		states[i] = !!gpio_get(leds[i].gpio, leds[i].pin);
	}
	return 0;
}

int led_driver_set_states(const unsigned char *states, size_t n) {
	int i;
	n = n > LEDS_N ? LEDS_N : n;
	for (i = 0; i < n; i++) {
		gpio_set(leds[i].gpio, leds[i].pin,
			states[i] ? GPIO_PIN_HIGH : GPIO_PIN_LOW);
	}
	return 0;
}

static void __led_driver_serialize(unsigned char arr[LEDS_N]) {
	int i;
	printf("[");
	for (i = 0; i < LEDS_N - 1; i++) {
		printf("%d,", !!arr[i]);
	}
	printf("%d]\n", !!arr[LEDS_N - 1]);
}

int led_driver_serialize(void) {
	unsigned char led_states[LEDS_N];
	led_driver_get_states(led_states, LEDS_N);
	__led_driver_serialize(led_states);
	return 0;
}
