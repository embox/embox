/**
 * @file
 * @brief Loads and executes an OS image.
 *
 * @date 08.08.12
 * @author Eldar Abusalimov
 */

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <drivers/irqctrl.h>
#include <hal/ipl.h>

static void print_usage(void) {
	printf("Usage: load [-h] -a addr\n");
}

static int parse_addr_option(char *optarg, int opt, char **addr_p) {
	char *endptr;
	long number;

	if (!optarg || !*optarg) {
		printf("load -%c: memory address expected\\n\", opt);\n", opt);
		goto inval;
	}

	number = strtol(optarg, &endptr, 0);
	if (*endptr != '\0') {
		printf("load -%c: invalid address option: %s\n", opt, optarg);
		goto inval;
	}

	*addr_p = (char *) number;
	return 0;

inval:
	print_usage();
	return -EINVAL;
}

static void _NORETURN load_and_run(char *image_vma, char *image_lma, size_t image_len) {
	unsigned int irq;
	void (*callme)(void) __attribute__ ((noreturn));

	printf("load: copying image into 0x%08x, from 0x%08x, size %d bytes\n",
			(unsigned int) image_vma, (unsigned int) image_lma, image_len);
	memcpy(image_vma, image_lma, image_len);

	printf("load: calling 0x%08x ...\n", (unsigned int) image_vma);

	ipl_disable();
	for (irq = 0; irq < IRQCTRL_IRQS_TOTAL; ++irq) {
		irqctrl_disable(irq);
	}

	callme = (typeof(callme)) image_vma;
	callme();
}

int main(int argc, char **argv) {
	extern char _load_image_start, _load_image_end;

	char *image_vma;
	char *image_lma = &_load_image_start;
	size_t image_len = &_load_image_end - &_load_image_start;

	int opt;
	int err;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "a:h"))) {
		switch (opt) {
		case 'a':
			if ((err = parse_addr_option(optarg, opt, &image_vma))) {
				return err;
			}
			load_and_run(image_vma, image_lma, image_len);
			break;
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
