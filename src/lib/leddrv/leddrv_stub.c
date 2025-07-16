/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.10.24
 */

#include <errno.h>
#include <stdbool.h>

#include <util/log.h>

#include "leddrv.h"

static bool leds[LEDDRV_LED_N];

int leddrv_led_on(unsigned int nr) {
	if (nr >= LEDDRV_LED_N) {
		return -EINVAL;
	}

	leds[nr] = true;
	log_info("LED[%i] := 1", nr);

	return 0;
}

int leddrv_led_off(unsigned int nr) {
	if (nr >= LEDDRV_LED_N) {
		return -EINVAL;
	}

	leds[nr] = false;
	log_info("LED[%i] := 0", nr);

	return 0;
}

void leddrv_get_states(bool states[LEDDRV_LED_N]) {
	int i;

	for (i = 0; i < LEDDRV_LED_N; i++) {
		states[i] = leds[i];
	}
}

void leddrv_set_states(bool states[LEDDRV_LED_N]) {
	int i;

	for (i = 0; i < LEDDRV_LED_N; i++) {
		leds[i] = states[i];
		log_info("LED[%i] := %i", i, (int)states[i]);
	}
}
