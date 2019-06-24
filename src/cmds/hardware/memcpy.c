/**
 * @file
 * @brief Copy memory
 *
 * @date 24.06.19
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static void print_usage(void) {
	printf("Usage: memcpy [-h] <dest> <src> <n>\n");
}

int main(int argc, char **argv) {
	void *src, *dest;
	unsigned int n;
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (argc != 4) {
		print_usage();
		return -1;
	}

	n    = strtoll(argv[argc - 1], NULL, 0);
	src  = (void *) ((uintptr_t) strtoll(argv[argc - 2], NULL, 0));
	dest = (void *) ((uintptr_t) strtoll(argv[argc - 3], NULL, 0));

	memcpy(dest, src, n);

	return 0;
}
