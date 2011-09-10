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
#include <framework/example/self.h>
#include <lib/linenoise.h>

EMBOX_EXAMPLE(line_noise_run);

#define BUF_SIZE 128
static int line_noise_run(int argc, char **argv) {
	char buf[BUF_SIZE];
	linenoiseHistoryAdd("history1");
	linenoiseHistoryAdd("history2");
	linenoise("enter string> ", buf, BUF_SIZE);

	printf("You've entered %s.\n", buf);

	return ENOERR;
}

