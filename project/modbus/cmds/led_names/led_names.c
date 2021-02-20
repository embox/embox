/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.02.2015
 */

#include <led_names.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>


static int lednames_serialize(void) {
	int i;
	printf("[");
	for (i = 0; i < LEDNAME_N; ++i) {
		printf("\"%s\"", led_names[i]);
		if (i < LEDNAME_N - 1) {
			printf(",");
		}
	}
	printf("]\n");
	return 0;
}

static int lednames_store(int n, int name_len) {
	char buf[LEDNAME_MAX];

	if (!((0 <= n) && (n < LEDNAME_N))) {
		return -ERANGE;
	}

	if (name_len > LEDNAME_MAX) {
		name_len = LEDNAME_MAX;
	}

	if (0 > read(STDIN_FILENO, buf, name_len)) {
		return -errno;
	}

	strncpy(led_names[n], buf, name_len);
	led_names[n][name_len] = '\0';
	return 0;
}

int main(int argc, char *argv[]) {
	const char *action = argv[1];

	if (action) {
		if (0 == strcmp(action, "serialize")) {
			return lednames_serialize();
		} else if (0 == strcmp(action, "store")) {
			return lednames_store(atoi(argv[2]), atoi(getenv("CONTENT_LENGTH")));
		}
	}

	return -EINVAL;
}
