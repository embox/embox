/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    10.11.2014
 */

#include <stdio.h>

#include <libleddrv.h>

#include "libleddrv_ll.h"

void leddrv_ll_init(void) {

}

void leddrv_ll_update(unsigned char leds_state[LEDDRV_LED_N]) {
	int i;
	for (i = 0; i < LEDDRV_LED_N; i++) {
		fprintf(stderr, "led(%03d)=%d\n", i, !!leds_state[i]);
	}
}

int leddrv_ll_error(int n) {
	return 0;
}
