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
	printf("Usage: i2cset -h\n");
	printf("Usage: i2cget [-y] i2cbus chip-address [data-address [mode]]\n");
	printf("Example: i2cget -y 2 0x24 w\n");
	printf("\tRead a 16-bit word from I2C device at 7-bit address 0x24 on bus 2\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

int main(int argc, char **argv) {
	int opt;
	int i = 1;
	long busn = 0;
	long chip_addr = 0;
	long data_addr = 0;
	uint16_t buf;
	int mode = 1;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "hy"))) {
		switch (opt) {
		case 'y':
			i++;
			break;
		case '?':
		case 'h':
			print_usage();
			return 0;
		default:
			print_error();
			return -EINVAL;
		}
	}

	busn = strtol(argv[i++], NULL, 0);
	chip_addr = strtol(argv[i++], NULL, 0);
	printf("bus(%d) chip(0x%x)", (int) busn, (unsigned) chip_addr);

	if (i < (argc - 1)) {
		data_addr = strtol(argv[i++], NULL, 0);
		printf(" data_addr(%x)", (unsigned) data_addr);
	}
	if (i < (argc)) {
		switch(argv[i][0]){
		case 'b':
			mode = 1;
			break;
		case 'w':
			mode = 2;
			break;
		default:
			printf("\nwrong parameter [mode] choose 'b' (byte) 'w' 16-bit word\n");
			return -EINVAL;
		}
	}
	printf(" length(%x)", (unsigned )mode);
	printf("\n");
	if (0 < i2c_bus_read(busn, chip_addr, (uint8_t *)&buf, mode)) {
		printf("res (%x)\n", (unsigned) buf);
	} else {
		printf("can't read\n");
	}
	return 0;
}
