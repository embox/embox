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

static int ccwrp_query_to_argv(char **argv, unsigned int argv_len) {
	char *query = getenv("QUERY_STRING");
	unsigned int argv_cnt = 0;
	char *query_to_parse = query;

	while (argv_cnt < argv_len) {
		char *arg = strchr(query_to_parse, '=');
		if (arg) {
			argv[argv_cnt++] = arg + 1;
			query_to_parse = strchrnul(arg + 1, '&');
			if (*query_to_parse) {
				*query_to_parse++ = '\0';
			}
		} else {
			break;
		}
	}

	argv[argv_cnt] = NULL;

	return argv_cnt;
}

int main(int argc, char *argv[]) {
	char *new_argv[4];
	const char *action;
	int nr, i;
	char buf[32];
	char *pbuf = &buf[0];

	printf("HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n", 200, "OK", "text/plain");


	ccwrp_query_to_argv(new_argv, 4);

	action = new_argv[0];
	if (action) {
		if (0 == strcmp(action, "set")) {
			nr = atoi(new_argv[1]);
			leds[nr] = 1;
		} else if (0 == strcmp(action, "clr")) {
			nr = atoi(new_argv[1]);
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
