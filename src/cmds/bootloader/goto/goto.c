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

#include <hal/context.h>

static void print_help(char **argv) {
	assert(argv);
	assert(argv[0]);
	printf("Usage: %s [[-i] ADDRESS [-s STACK_POINTER]]\n", argv[0]);

	return;
}

int main(int argc, char **argv) {
	int opt;
	void (*f)(void);
	void *sp = NULL;
	struct context prev_ctx, next_ctx;

	if (argc < 2) {
		print_help(argv);
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "his:"))) {
		switch (opt) {
		case 'h':
			print_help(argv);
			return 0;
		case 'i':
			ipl_save();
			break;
		case 's':
			sp = (void *) ((uintptr_t)strtoll(optarg, 0, 0));
			break;
		default:
			printf("goto: Unknown argument: %c\n", opt);
			print_help(argv);
			return 0;
		}
	}

	if (!sp) {
		f = (void *) ((uintptr_t)strtoll(argv[argc - 1], 0, 0));
		printf("Goto to addr 0x%x\n", (unsigned int) f);
		f();
		/* UNREACHABLE */
		return 0;
	}

	f = (void *) ((uintptr_t)strtoll(argv[argc - 3], 0, 0));
	printf("Goto to addr f = 0x%x, sp = 0x%x\n",
		(unsigned int) f, (unsigned int) sp);
	context_init(&next_ctx, CONTEXT_PRIVELEGED | CONTEXT_IRQDISABLE,
			f, sp);

	context_switch(&prev_ctx, &next_ctx);

	/* UNREACHABLE */
	return 0;
}
