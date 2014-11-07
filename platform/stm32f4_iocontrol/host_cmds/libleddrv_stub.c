
#include <string.h>
#include <stdio.h>
#include "libleddrv.h"

static unsigned char leddrv_leds_state[LEDDRV_LED_N];

int leddrv_getstates(unsigned char leds_state[LEDDRV_LED_N]) {
	memcpy(leds_state, leddrv_leds_state, sizeof(leds_state));
	return 0;
}

int leddrv_updatestates(unsigned char new_leds_state[LEDDRV_LED_N]) {
	int i;

	memcpy(leddrv_leds_state, new_leds_state, sizeof(leddrv_leds_state));

	for (i = 0; i < LEDDRV_LED_N; i++) {
		fprintf(stderr, "led(%03d)=%d\n", i, !!leddrv_leds_state[i]);
	}

	return 0;
}
