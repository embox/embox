/**
 * @file
 *
 * @date Nov 16, 2018
 * @author Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <drivers/i2c/i2c.h>

static void print_usage(void) {
	printf("Usage: i2cdetect -h\n");
	printf("Usage: i2cdetect -l\n");
	printf("Usage: i2cdetect [-r] [-y] i2cbus\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

static void i2c_bus_list(void) {
	const struct i2c_bus *bus;
	int i;

	for (i = 0; i < I2C_BUS_MAX; i++) {
		bus = i2c_bus_get(i);
		if (bus) {
			printf("i2c_bus%d\n", i);
		}
	}
}

static void i2c_bus_scan(long busn, int read) {
	int i;
	uint8_t tmp;
	ssize_t res;

	printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
	printf("00:         ");
	for (i = 0x03; i < 0x77; i++) {
		if (0 == (i % 0x10)) {
			printf("\n%2X:", (unsigned)i);
		}
		if (read) {
			res = i2c_bus_read(busn, i, &tmp, 1);
		}
		else {
			res = i2c_bus_write(busn, i, &tmp, 0);
		}
		if (0 <= res) {
			printf(" %2X", (unsigned)i);
		}
		else {
			printf(" --");
		}
	}

	printf("\n");
}

int main(int argc, char **argv) {
	int opt;
	long busn = 0;
	long cmd = 0;

	while (-1 != (opt = getopt(argc, argv, "hlry:"))) {
		switch (opt) {
		case 'l':
			i2c_bus_list();
			return 0;
		case 'y':
			busn = strtol(optarg, NULL, 0);
			break;
		case 'r':
			cmd = 1;
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
	i2c_bus_scan(busn, cmd);

	return 0;
}
