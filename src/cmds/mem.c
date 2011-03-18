/**
 * @file
 * @brief read from memory
 *
 * @date 13.02.09
 * @author Alexey Fomin
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

EMBOX_CMD(exec);

#define DEFAULT_LENGTH 0x100

static void print_usage(void) {
	printf("Usage: mem [-h] -a addr [-n bytes]\n");
}

static int parse_option(char *optarg, int opt, long int *number) {
	char *endptr;

	if ((optarg == NULL) || (*optarg == '\0')) {
		printf("wmem -%c: option expected\n", opt);
		print_usage();
		return -1;
	}

	*number = strtol(optarg, &endptr, 0);
	if (*endptr != '\0') {
		printf("wmem -%c: invalid option: %s\n", opt, optarg);
		print_usage();
		return -1;
	}

	return 0;
}

static int exec(int argc, char **argv) {
	bool a_flag = false;
	int opt, i;
	unsigned int *address;
	size_t length = DEFAULT_LENGTH;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:n:h"))) {
		switch (opt) {
		case 'a':
			if (0 != parse_option(optarg, opt, (long int *) &address)) {
				return -1;
			}
			a_flag = true;
			break;

		case 'n':
			if (0 != parse_option(optarg, opt, (long int *) &length)) {
				return -1;
			}
			break;

		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (!a_flag) {
		printf("mem: address required\n");
		print_usage();
		return -1;
	}

	address = (unsigned int *) ((int) address & ~(sizeof(address) - 1));
	length = (length + sizeof(address) - 1) / sizeof(address);
	i = 0;
	while (length--) {
		if (i-- == 0) {
			i = 3;
			printf("\n0x%08x:\t", (unsigned int) address);
		}
		printf("0x%08x  ", *address++);
	}
	printf("\n");

	return 0;
}
