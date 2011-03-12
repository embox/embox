/**
 * @file
 * @brief Execute image file.
 *
 * @date 02.07.2009
 * @author Sergey Kuzmin
 */

#include <embox/cmd.h>

#include <getopt.h>
#include <hal/interrupt.h>
#include <hal/ipl.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: [-h] [-a addr]\n");
}

typedef void (*goto_t)(void) __attribute__ ((noreturn));

static void go_to(void *addr) {
	interrupt_nr_t interrupt_nr;

	printf("Going to 0x%08X\n", (unsigned int) addr);

	ipl_disable();
	for (interrupt_nr = 0; interrupt_nr < INTERRUPT_NRS_TOTAL; ++interrupt_nr) {
		interrupt_disable(interrupt_nr);
	}

	((goto_t) addr)();
}

static int exec(int argc, char **argv) {
	int opt;
	void *addr;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:h"))) {
		switch (opt) {
		case 'a':
			if ((optarg == NULL) || (*optarg == '\0')) {
				printf("goto: address expected\n");
				return -1;
			}
			if (sscanf(optarg, "0x%x", &addr) > 0) {
				go_to(addr);
				return 0;
			}
			printf("goto: invalid value \"%s\", hex value expected.", optarg);
			return -1;
		case '?':
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	return 0;
}
