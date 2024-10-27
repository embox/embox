/**
 * @file
 * @brief Based on project/stm32f4_iocontrol/cmds/libleddrv_ll.c
 *
 * @author  Alexander Kalmuk
 * @date    20.08.2019
 */

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lib/leddrv.h>

static int led_serialize(void) {
	int i;

	bool led_states[LEDDRV_LED_N];
	leddrv_get_states(led_states);

	printf("[");
	for (i = 0; i < LEDDRV_LED_N - 1; i++) {
		printf("%d,", (int)led_states[i]);
	}
	printf("%d]\n", (int)led_states[LEDDRV_LED_N - 1]);

	return 0;
}

int main(int argc, char *argv[]) {
	const char *action = argv[1];

	if (action) {
		if (0 == strcmp(action, "set")) {
			return leddrv_set(atoi(argv[2]));
		}
		else if (0 == strcmp(action, "clr")) {
			return leddrv_clr(atoi(argv[2]));
		}
		else if (0 == strcmp(action, "serialize_states")) {
			return led_serialize();
		}
	}

	return -EINVAL;
}
