/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.11.2014
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <lib/libds/array.h>

#include <framework/mod/options.h>

#define CCWRP_ALLOWED_CMD_NAMES OPTION_STRING_GET(allowed_cmds)

static int ccwrp_query_to_argv(char *query, char **argv, unsigned int argv_len) {
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

#if OPTION_GET(BOOLEAN, cmds_check)
static int ccwrp_security_check(int argc, char *argv[]) {
	const char *allowed_cmds = CCWRP_ALLOWED_CMD_NAMES;
	const char *p_cmd, *p_e_cmd;

	p_cmd = allowed_cmds;
	while (*p_cmd) {
		p_cmd += strspn(p_cmd, " ");
		p_e_cmd = strchrnul(p_cmd, ' ');
		if (0 == strncmp(argv[0], p_cmd, p_e_cmd - p_cmd) &&
				argv[0][p_e_cmd - p_cmd] == '\0') {
			return 0;
		}

		p_cmd = p_e_cmd;
	}

	return 1;
}
#else
static int ccwrp_security_check(int argc, char *argv[]) {
	return 0;
}
#endif

int main(int argc, char *argv[]) {
	char *query = getenv("QUERY_STRING");
	char *new_argv[16];
	int new_argc;

	printf("HTTP/1.1 %d %s\r\n"
		"Content-Type: %s\r\n"
		"Connection: close\r\n"
		"\r\n", 200, "OK", "text/plain");

	new_argc = ccwrp_query_to_argv(query, new_argv, ARRAY_SIZE(new_argv));
	if (new_argc && 0 == ccwrp_security_check(new_argc, new_argv)) {
		return execv(new_argv[0], new_argv);
	}

	return 0;
}
