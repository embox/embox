/**
 * @file
 * @brief Based on project/stm32f4_iocontrol/cmds/libleddrv_ll.c
 *
 * @author  Alexander Kalmuk
 * @date    20.08.2019
 */

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lib/leddrv.h>

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
		}
		else {
			break;
		}
	}

	argv[argv_cnt] = NULL;

	return argv_cnt;
}

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
	char *new_argv[4];
	const char *action;

	printf("HTTP/1.1 %d %s\r\n"
	       "Content-Type: %s\r\n"
	       "Connection: close\r\n"
	       "\r\n",
	    200, "OK", "text/plain");

	ccwrp_query_to_argv(new_argv, 4);

	action = new_argv[0];

	if (action) {
		if (0 == strcmp(action, "set")) {
			return leddrv_set(atoi(new_argv[1]));
		}
		else if (0 == strcmp(action, "clr")) {
			return leddrv_clr(atoi(new_argv[1]));
		}
		else if (0 == strcmp(action, "serialize_states")) {
			return led_serialize();
		}
	}

	return -EINVAL;
}
