/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.11.2014
 */

#include "libleddrv.h"

#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <libleddrv_ll.h>

static unsigned char leddrv_leds_state[LEDDRV_LED_N];

static inline void leddrv_ll_do_update(void) {
	leddrv_ll_update(leddrv_leds_state);
}

int leddrv_init(void) {
	leddrv_ll_init();

	/* default all on state */
	memset(leddrv_leds_state, 0, sizeof(leddrv_leds_state));
	leddrv_ll_do_update();

	return 0;
}

static inline int leddrv_check(unsigned int led_n) {
	return led_n >= LEDDRV_LED_N ? -EINVAL: 0;
}

int leddrv_set(unsigned int led_n) {
	int err;

	if ((err = leddrv_check(led_n))) {
		return err;
	}

	leddrv_leds_state[led_n] = true;
	leddrv_ll_do_update();

	return 0;
}

int leddrv_clr(unsigned int led_n) {
	int err;

	if ((err = leddrv_check(led_n))) {
		return err;
	}

	leddrv_leds_state[led_n] = false;
	leddrv_ll_do_update();

	return 0;
}

int leddrv_getstates(unsigned char leds_state[LEDDRV_LED_N]) {
	memcpy(leds_state, leddrv_leds_state, sizeof(leddrv_leds_state));
	return 0;
}

int leddrv_updatestates(unsigned char new_leds_state[LEDDRV_LED_N]) {
	memcpy(leddrv_leds_state, new_leds_state, sizeof(leddrv_leds_state));
	leddrv_ll_do_update();
	return 0;
}

int leddrv_geterrors(unsigned char leds_error[LEDDRV_LED_N]) {
	int i;
	for (i = 0; i < LEDDRV_LED_N; ++i) {
		leds_error[i] = !!leddrv_ll_error(i);
	}
	return 0;
}
