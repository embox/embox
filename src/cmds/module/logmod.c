/**
 * @file logmod.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 24.03.2020
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <framework/mod/api.h>
#include <framework/mod/integrity.h>
#include <framework/mod/types.h>
#include <util/log.h>

static void print_usage(void) {
	printf("Usage: logmod [name [level]]\n");
}

int main(int argc, char **argv) {
	const struct mod *mod;
	struct logger *logger;
	int mod_found = 0;
	int opt;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case '?':
			break;
		default:
			return -EINVAL;
		}
	}

	mod_foreach(mod) {
		if (!strstr(mod_name(mod), argv[1])) {
			continue;
		}

		mod_found = 1;

		logger = mod->logger;

		if (logger == NULL) {
			printf("Logger is NULL\n");
			break;
		}

		if (argc == 3) {
			logger->logging.level = atoi(argv[2]);
		}

		printf("%s log level is %d\n", argv[1], logger->logging.level);
	}

	if (!mod_found) {
		printf("Module %s not found!\n", argv[1]);
	}

	return 0;
}
