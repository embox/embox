/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

#include <drivers/i2c/i2c.h>

static void print_usage(void) {
	printf("Usage: i2cdump -h\n");
	printf("Usage: i2cdump [-r first-last] [-y] i2cbus address [mode]\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

int main(int argc, char **argv) {
	int opt;
	long busn = 0;
	long chip_addr = 0;
	int i = 1;
	uint8_t fr = 0, lr = 0;
	char *endp;
	uint16_t buf;
	int mode = 1;

	while (-1 != (opt = getopt(argc, argv, "hyr:"))) {
		i++;
		switch (opt) {
		case 'y':
			break;
		case 'r':
			fr = strtol(optarg, &endp, 0);
			if (endp[0] != '-' ){
				print_error();
				return 0;
			}
			endp++;
			lr = strtol(endp, NULL, 0);
			break;
		case '?':
		case 'h':
			print_usage();
			return 0;
		default:
			print_error();
			return 0;
		}
	}
	busn = strtol(argv[i++], NULL, 0);
	chip_addr = strtol(argv[i], NULL, 0);
	printf("regs(0x%x-0x%x) bus(%d) chip(0x%x)", fr, lr, (int) busn, (unsigned )chip_addr);
	if (i < (argc - 1)) {
		mode = 2;
	}
	printf("\n");
	if (0 < i2c_bus_read(busn, chip_addr, (uint8_t *)&buf, mode)) {
		printf("res (%x)\n", (unsigned) buf);
	} else {
		printf("can't read\n");
	}

	return 0;
}
