/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.11.2014
 */

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "libleddrv.h"

static int cldrv_serialize(void) {
	unsigned char led_states[LEDDRV_LED_N];
	int i;

	leddrv_getstates(led_states);


	printf("[");
	for (i = 0; i < LEDDRV_LED_N - 1; ++i) {
		printf("%d,", !!led_states[i]);
	}
	printf("%d]\n", led_states[LEDDRV_LED_N - 1]);
	return 0;
}

int main(int argc, char *argv[]) {
	const char *action = argv[1];

	if (action) {
		if (0 == strcmp(action, "init")) {
			return leddrv_init();
		} else if (0 == strcmp(action, "set")) {
			return leddrv_set(atoi(argv[2]));
		} else if (0 == strcmp(action, "clr")) {
			return leddrv_clr(atoi(argv[2]));
		} else if (0 == strcmp(action, "serialize_states")) {
			return cldrv_serialize();
		}
	}

	return -EINVAL;
}
