/**
 * @file
 * @brief read from memory
 *
 * @date 13.02.09
 * @author Alexey Fomin
 * @author Anton Kozlov
 * 	- access_type added
 */

#include <inttypes.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define DEFAULT_LENGTH 0x4

#if (__WORDSIZE == 32)
#define PRINT_POINTER_WIDTH "8"
#elif (__WORDSIZE == 64)
#define PRINT_POINTER_WIDTH "16"
#endif

enum access_type {
	MEM_AT_CHAR,
	MEM_AT_SHORT,
	MEM_AT_LONG,
};

static const char *fmts[] = {
	"0x%02lx  ",
	"0x%04lx  ",
	"0x%0" PRINT_POINTER_WIDTH "lx  ",
};

static const size_t aalign[] = {
	sizeof(char),
	sizeof(short),
	sizeof(long),
};

static void print_usage(void) {
	printf("Usage: mem [-h] [-l -s -c] addr [-n count]\n");
}

static int parse_option(char *optarg, int opt, long unsigned int *number) {
	char *endptr;

	if ((optarg == NULL) || (*optarg == '\0')) {
		printf("mem -%c: option expected\n", opt);
		print_usage();
		return -EINVAL;
	}

	*number = strtoul(optarg, &endptr, 0);
	if (*endptr != '\0') {
		printf("mem -%c: invalid option: %s\n", opt, optarg);
		print_usage();
		return -EINVAL;
	}

	return 0;
}

int main(int argc, char **argv) {
	int opt, ret;
	void *address;
	unsigned long val;
	size_t length = DEFAULT_LENGTH;
	size_t align;
	bool length_passed = false;
	enum access_type at = MEM_AT_LONG;

	while (-1 != (opt = getopt(argc, argv, "n:hcsl"))) {
		switch (opt) {
		case 'n':
			length_passed = true;
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

	errno = 0;
	if (length_passed) {
		address = (void *) strtoul(argv[argc - 3], 0, 0);
	} else  {
		address = (void *) strtoul(argv[argc - 1], 0, 0);
	}

	if (errno != 0) {
		printf("Failed to get address!\n");
		print_usage();
		return -EINVAL;
	}

	if ((uintptr_t) address & ((1 << at) - 1)) {
		printf("address is not aligned to selected mem access\n");
		return -EINVAL;
	}

	align = (uintptr_t) address & 0xF;
	/*address = (unsigned int *) ((int) address & ~(sizeof(address) - 1));*/
	/*length = (length + sizeof(address) - 1) / sizeof(address);*/
	while (length--) {
		if (0 == (((uintptr_t)address - align) & 0xF)) {
			printf("\n0x%0" PRIxPTR ":\t", (uintptr_t) address);
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
