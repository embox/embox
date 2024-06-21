/**
 * @file
 * @brief read from memory
 *
 * @date 13.02.09
 * @author Alexey Fomin
 * @author Anton Kozlov
 * @author Eldar Abusalimov
 */

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_LENGTH 0x4

#if (LONG_BIT == 32)
#define PRINT_POINTER_WIDTH "8"
#elif (LONG_BIT == 64)
#define PRINT_POINTER_WIDTH "16"
#endif

enum access_type {
	MEM_AT_CHAR,
	MEM_AT_SHORT,
	MEM_AT_LONG,
};

enum operation_mode {
	MEM_MODE_READ,
	MEM_MODE_WRITE,
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
	printf("Usage: mem [-h] [-l -s -c] addr [-n count | -w value_to_write ]\n");
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
	bool access_type_passed = false;
	enum access_type at = MEM_AT_LONG;
	enum operation_mode mode = MEM_MODE_READ;

	if (argc == 1) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "n:hcslw:"))) {
		switch (opt) {
		case 'n':
			ret = parse_option(optarg, opt, (unsigned long int *)&length);
			if (ret != 0) {
				return ret;
			}
			break;

		case 'l':
			access_type_passed = true;
			at = MEM_AT_LONG;
			break;
		case 's':
			access_type_passed = true;
			at = MEM_AT_SHORT;
			break;
		case 'c':
			access_type_passed = true;
			at = MEM_AT_CHAR;
			break;
		case 'w':
			mode = MEM_MODE_WRITE;
			ret = parse_option(optarg, opt, (unsigned long int *)&val);
			if (ret != 0) {
				return ret;
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

	errno = 0;
	if (access_type_passed) {
		address = (void *)strtoul(argv[3], 0, 0);
	}
	else {
		address = (void *)strtoul(argv[1], 0, 0);
	}

	if (errno != 0) {
		printf("Failed to get address!\n");
		print_usage();
		return -EINVAL;
	}

	if ((uintptr_t)address & ((1 << at) - 1)) {
		printf("address is not aligned to selected mem access %p %08x\n",
		    address, (1 << at) - 1);
		return -EINVAL;
	}

	if (mode == MEM_MODE_WRITE) {
		switch (at) {
		default:
		case MEM_AT_LONG:
			*(unsigned long *)address = val;
			break;
		case MEM_AT_SHORT:
			*(unsigned short *)address = val;
			break;
		case MEM_AT_CHAR:
			*(unsigned char *)address = val;
			break;
		}
	}
	else {
		align = (uintptr_t)address & 0xF;
		while (length--) {
			if (0 == (((uintptr_t)address - align) & 0xF)) {
				printf("\n0x%0" PRIxPTR ":\t", (uintptr_t)address);
			}

			switch (at) {
			default:
			case MEM_AT_LONG:
				val = *(unsigned long *)address;
				break;
			case MEM_AT_SHORT:
				val = *(unsigned short *)address;
				break;
			case MEM_AT_CHAR:
				val = *(unsigned char *)address;
				break;
			}

			printf(fmts[at], val);

			address += aalign[at];
		}
		printf("\n");
	}

	return 0;
}
