
#include <stdio.h>
#include "libleddrv.h"

static unsigned char leddrv_leds_state[LEDDRV_LED_N];

unsigned char *leddrv_getstates(void) {
	return leddrv_leds_state;
}

void leddrv_updatestates(void) {
	int i;

	for (i = 0; i < LEDDRV_LED_N; i++) {
		fprintf(stderr, "led(%03d)=%d\n", i, !!leddrv_leds_state[i]);
	}
}
