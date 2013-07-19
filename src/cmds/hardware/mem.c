/**
 * @file
 * @brief read from memory
 *
 * @date 13.02.09
 * @author Alexey Fomin
 * @author Anton Kozlov
 * 	- access_type added
 */

#include <embox/cmd.h>

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

EMBOX_CMD(exec);

#define DEFAULT_LENGTH 0x4

enum access_type {
	MEM_AT_CHAR,
	MEM_AT_SHORT,
	MEM_AT_LONG,
};

static const char *fmts[] = {
	"0x%02lx  ",
	"0x%04lx  ",
	"0x%08lx  ",
};

static const size_t aalign[] = {
	sizeof(char),
	sizeof(short),
	sizeof(long),
};

static void print_usage(void) {
	printf("Usage: mem [-h] [-l -s -c] -a addr [-n count]\n");
}

static int parse_option(char *optarg, int opt, long unsigned int *number) {
	char *endptr;

	if ((optarg == NULL) || (*optarg == '\0')) {
		printf("wmem -%c: option expected\n", opt);
		print_usage();
		return -EINVAL;
	}

	*number = strtoul(optarg, &endptr, 0);
	if (*endptr != '\0') {
		printf("wmem -%c: invalid option: %s\n", opt, optarg);
		print_usage();
		return -EINVAL;
	}

	return 0;
}

static int exec(int argc, char **argv) {
	bool a_flag = false;
	int opt, i, ret;
	void *address;
	unsigned long val;
	size_t length = DEFAULT_LENGTH;
	enum access_type at = MEM_AT_LONG;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:n:hcsl"))) {
		switch (opt) {
		case 'a':
			ret = parse_option(optarg, opt, (unsigned long int *) &address);
			if (ret != 0) {
				return ret;
			}
			a_flag = true;
			break;

		case 'n':
			ret = parse_option(optarg, opt, (unsigned long int *) &length);
			if (ret != 0) {
				return ret;
			}
			break;

		case 'l':
			at = MEM_AT_LONG;
			break;
		case 's':
			at = MEM_AT_SHORT;
			break;
		case 'c':
			at = MEM_AT_CHAR;
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
		return -EINVAL;
	}

	if ((int) address & ((1 << at) - 1)) {
		printf("address is not aligned to selected mem access\n");
		return -EINVAL;
	}

	/*address = (unsigned int *) ((int) address & ~(sizeof(address) - 1));*/
	/*length = (length + sizeof(address) - 1) / sizeof(address);*/
	i = 0;
	while (length--) {
		if (i-- == 0) {
			i = 3;
			printf("\n0x%08x:\t", (unsigned int) address);
		}

		switch (at) {
		default:
		case MEM_AT_LONG:
			val = *(unsigned long *) address;
			break;
		case MEM_AT_SHORT:
			val = *(unsigned short *) address;
			break;
		case MEM_AT_CHAR:
			val = *(unsigned char *) address;
			break;
		}

		printf(fmts[at], val);

		address += aalign[at];
	}
	printf("\n");

	return 0;
}
