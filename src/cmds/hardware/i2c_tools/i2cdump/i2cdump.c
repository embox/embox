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
#include <string.h>
#include <unistd.h>

#include <drivers/i2c/i2c.h>
#include <util/pretty_print.h>

static void print_usage(void) {
	printf("Usage: i2cdump -h\n");
	printf("Usage: i2cdump [-r first-last] [-y] i2cbus address [mode]\n");
}

static void print_error(void) {
	printf("Wrong parameters\n");
}

static void dump_buf(uint8_t *buf, int off, int len) {
	char print_buf[128];
	int row_len = pretty_print_row_len();

	while (len > 0) {
		if (len < row_len) {
			row_len = len;
		}

		pretty_print_row(buf, row_len, print_buf);
		printf("0x%02X: %s\n", off, print_buf);
		len -= row_len;
		buf += row_len;
		off += row_len;
	}
}

int main(int argc, char **argv) {
	int opt;
	long busn = 0;
	long chip_addr = 0;
	int i = 1, j;
	int fr = 0x00, lr = 0xff;
	uint8_t buf[0x100];
	char *endp;
	int mode = 1;

	if (argc == 1) {
		print_usage();
		return 0;
	}

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

			i++;
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

	if (fr < 0 || lr < 0 || fr > 0xFF || lr > 0xFF) {
		printf("Register range should fit in [0x00-0xFF]\n");
		return -EINVAL;
	}

	printf("regs(0x%x-0x%x) bus(%d) chip(0x%x)", fr, lr, (int) busn, (unsigned )chip_addr);
	if (i < (argc - 1)) {
		mode = 2;
	}
	printf("\n");

	memset(buf, 0, sizeof(buf));

	for (j = fr; j <= lr; j += mode) {
		uint8_t reg = (uint8_t) j;

		if (0 > i2c_bus_write(busn, chip_addr, &reg, sizeof(reg))) {
			printf("Failed at %d-th byte\n", j);
			lr = j - 1;
			break;
		}

		if (0 > i2c_bus_read(busn, chip_addr, &buf[j - fr], mode)) {
			printf("can't read\n");
		}
	}

	dump_buf(buf, fr, lr - fr + 1);

	return 0;
}
