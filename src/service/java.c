/**
 * @file
 * @brief
 *
 * @author  Nikita Shigarov
 * @date    06.04.2015
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <lib/libds/array.h>

#include <framework/mod/options.h>

static int java_query_to_argv(char *query, char **argv, unsigned int argv_len) {
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
	char *query = getenv("QUERY_STRING");
	char *new_argv[16];
	int new_argc;

	printf("HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n", 200, "OK", "text/plain");

	new_argc = java_query_to_argv(query, new_argv, ARRAY_SIZE(new_argv));
	if (new_argc >= 2) {

		char* args[argc+2];
		args[0] = "cldc_vm";
		args[1] = "-cp";
		args[2] = new_argv[0];
		args[3] = new_argv[1];
		return execv("cldc_vm", args);
	}

	return 0;
}
