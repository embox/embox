/**
 * @file
 * @brief Stub to simulate LEDs
 *
 * @date 02.02.2020
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define LEDS_COUNT  4

static char leds[LEDS_COUNT];

int main(int argc, char *argv[]) {
	const char *action = argv[1];
	int nr, i;
	char buf[32];
	char *pbuf = &buf[0];

	if (action) {
		if (0 == strcmp(action, "set")) {
			nr = atoi(argv[2]);
			leds[nr] = 1;
		} else if (0 == strcmp(action, "clr")) {
			nr = atoi(argv[2]);
			leds[nr] = 0;
		} else if (0 == strcmp(action, "serialize_states")) {
			pbuf += sprintf(pbuf, "[");

			for (i = 0; i < LEDS_COUNT - 1; i++) {
				pbuf += sprintf(pbuf, "%d,", !!leds[i]);
			}

			pbuf += sprintf(pbuf, "%d]\n", !!leds[LEDS_COUNT - 1]);
			*pbuf = '\0';

			printf("%s", buf);
		}
	}

	return 0;
}
