/**
 * @file
 * @brief Run program in modified environment or print current one
 *
 * @date 28.02.13
 * @author Ilia Vaprol
 *          - Initial implementation
 * @author Dmitry Danilov
 *          - Program invocation in modified environment
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	int opt;
	int clear_env = 0;
	int print_null = 0;
	
	while (-1 != (opt = getopt(argc, argv, "0ihu:"))) {
		switch (opt) {
			case '0':
				print_null = 1;
				break;
			case 'i':
				clear_env = 1;
				break;
			case 'u':
				if (-1 == unsetenv(optarg)) {
					perror("env: unsetenv");
					return -errno;
				}
				break;
			case '?':
				printf("Try '%s -h` for more information.\n", argv[0]);
				return -EINVAL;
			case 'h':
				printf("Usage: %s [OPTION]... [-] [NAME=VALUE]... [COMMAND [ARGS]...]\n", argv[0]);
				return 0;
			default:
				break;
		}
	}
	if (optind < argc && !strcmp(argv[optind], "-")) {
		clear_env = 1;
		++optind;
	}
	
	if (clear_env) {
		if (clearenv()) {
			perror("env: clearenv");
			return -errno;
		}
	}

	while ((optind < argc) && (NULL != strchr(argv[optind], '='))
			&& ('=' != argv[optind][0])) {
		if (putenv(argv[optind]) == -1) {
			perror("env: putenv");
			return -errno;
		}
		++optind;
	}
	
	/* difference from posix: execv return 0 and does not set errno on errors */
	if (optind < argc) {
		execv(argv[optind], argv+optind);
		printf("env: execv: No such command '%s'\n", argv[optind]);
		return -ENOENT;
	}
	
	if (environ)
		for (char **env = environ; *env; env++)
			printf("%s%c", *env, print_null ? '\0' : '\n');
	return 0;
}
