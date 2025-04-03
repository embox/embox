/**
 * @file
 * @brief Enable JTAG pins
 *
 * @date 27.06.2021
 * @author kpishere
 */

#include <unistd.h>
#include <stdio.h>
#include <drivers/gpio.h>
#include <drivers/jtag/jtag.h>

static void print_usage(void) {
	printf("USAGE:\njtag [<option>]\n"
		" option: 0 (default)\n"
		"    jtag 0 -- Pins 22-TRST, 4-TDI, 5-TDO, 6-RTCK, 12-TMS, 13-TCK\n"
		"    jtag 1 -- Pins 22-TRST, 23-RTCK, 24-TDO, 25-TCK, 26-TDI, 27-TMS\n"
		"    jtag 2 -- Pins 4-TDI, 5-TDO, 6-RTCK, 12-TMS, 13-TCK\n");
}

int main(int argc, char **argv) {
	int opt;

	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			printf("Unknown option\n");
			print_usage();
			return 0;
		}
	}

	if (argc < 2) {
		opt = 0;
	} else {
		opt = argv[1][0] - '0';
	}

	if(gpio_jtag(opt) == 0) {
		printf("Opt %d\n", opt);
	} else {
		print_usage();
	}

	return 0;
}

