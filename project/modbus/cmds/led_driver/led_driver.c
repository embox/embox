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

#include <libleddrv.h>

static void cldrv_serialize(unsigned char arr[LEDDRV_LED_N]) {
	int i;
	printf("[");
	for (i = 0; i < LEDDRV_LED_N - 1; ++i) {
		printf("%d,", !!arr[i]);
	}
	printf("%d]\n", !!arr[LEDDRV_LED_N - 1]);
}

static int cldrv_states(void) {
	unsigned char led_states[LEDDRV_LED_N];
	leddrv_getstates(led_states);
	cldrv_serialize(led_states);
	return 0;
}

static int cldrv_errors(void) {
	unsigned char led_errors[LEDDRV_LED_N];
	leddrv_geterrors(led_errors);
	cldrv_serialize(led_errors);
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
			return cldrv_states();
		} else if (0 == strcmp(action, "serialize_errors")) {
			return cldrv_errors();
		}
	}

	return -EINVAL;
}
