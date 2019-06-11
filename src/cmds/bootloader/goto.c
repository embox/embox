/**
 * @file go.c
 * @brief Execute code from memory at specified address
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 10.06.2019
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <hal/ipl.h>

static void print_help(char **argv) {
	assert(argv);
	assert(argv[0]);
	printf("Usage: %s [[-i] ADDRESS]\n", argv[0]);

	return;
}

int main(int argc, char **argv) {
	int opt;
	void (*f)(void);

	if (argc < 2 || argc > 3) {
		print_help(argv);
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "i"))) {
		switch (opt) {
		case 'i':
			ipl_save();
			break;
		}
	}

	f = (void *) ((uintptr_t)strtoll(argv[argc - 1], 0, 0));

	f();

	return 0;
}
