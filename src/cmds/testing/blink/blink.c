/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 28.10.24
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <lib/leddrv.h>

int main(int argc, char *argv[]) {
	int i, j;
	bool states[LEDDRV_LED_N];

	if (LEDDRV_LED_N <= 0) {
		printf("error: LEDs not found\n");
		return -ENOTSUP;
	}

	memset(states, false, sizeof(states));
	leddrv_set_states(states);

	for (i = 0; i < 3; i++) {
		for (j = 0; j < LEDDRV_LED_N; j++) {
			leddrv_led_on(j);
			sleep(1);
			leddrv_led_off(j);
		}
		sleep(1);
	}

	return 0;
}
