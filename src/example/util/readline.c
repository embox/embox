/**
 * @file
 *
 * @brief Does readline
 *
 * @date 10.09.2011
 * @author Anton Kozlov
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <framework/example/self.h>
#include <lib/linenoise.h>

EMBOX_EXAMPLE(line_noise_run);

#define COMPLS_COUNT 2
const char *compls[] = { "hell", "hello world" };

static int compl(char *buf, char *out_buf) {
	int buf_len = strlen(buf);
	int ret = 0;
	for (size_t i = 0; i < COMPLS_COUNT; i++) {
		if (strncmp(buf, compls[i], buf_len) == 0) {
			strcpy(out_buf, compls[i]);
			out_buf += strlen(compls[i]) + 1;
			ret++;
		}
	}
	return ret;
}

#define BUF_SIZE 128
static int line_noise_run(int argc, char **argv) {
	char buf[BUF_SIZE];
	struct hist h;
	linenoise_history_init(&h);
	linenoise_history_add("history1", &h);
	linenoise_history_add("history2", &h);
	linenoise("enter string> ", buf, BUF_SIZE, &h, (compl_callback_t) compl);

	printf("You've entered %s.\n", buf);

	return ENOERR;
}
