/**
 * @file
 * @brief Writes memory word at the specified address.
 *
 * @date 20.02.2009
 * @author Alexey Fomin
 *          - Initial implementation
 * @author Eldar Abusalimov
 *          - Reviewing and refactoring
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: wmem [-h] -a addr -v value");
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
	bool a_flag = false, v_flag = false;
	int opt;
	volatile unsigned int *address;
	unsigned int value;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:v:h"))) {
		switch (opt) {
		case 'a':
			if(0 != parse_option(optarg, opt, (long int *) &address)) {
				return -1;
			}
			a_flag = true;
			break;

		case 'v':
			if(0 != parse_option(optarg, opt, (long int *) &value)) {
				return -1;
			}
			v_flag = true;
			break;

		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (!a_flag || !v_flag) {
		printf("wmem: both -a and -v options required!\n");
		print_usage();
		return -1;
	}

	*address = value;

	return 0;
}
