/**
 * @file
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 20.12.2019
 */
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lib/libds/array.h>

#include <drivers/spi.h>

static void print_help(char **argv) {
	printf("Transfer bytes via SPI bus\n");
	printf("Usage:\n");
	printf("%s [-s] [-m] [-b] bus_number line_number data0 [data1 [data2 "
	       "[...]]]\n",
	    argv[0]);
	printf("%s [-l] [-h]\n", argv[0]);
	printf("\t -s - slave mode\n");
	printf("\t -m - master mode\n");
	printf("\t -f - full time cs\n");
	printf("\t -x - 16 bit output format\n");
	printf("\t -l - list SPI buses\n");
	printf("\t -h - this help\n");
}

ARRAY_SPREAD_DECLARE(struct spi_device *, __spi_device_registry);
ARRAY_SPREAD_DECLARE(struct spi_controller *, __spi_controller_registry);

static void list_spi_devices(void) {
	struct spi_device *dev;
	struct spi_controller *cntl;

	array_spread_foreach(cntl, __spi_controller_registry) {
		printf("Bus: %s\n", cntl->cdev.name);
	}

	array_spread_foreach(dev, __spi_device_registry) {	
		printf("device: %s\n", dev->cdev.name);
	}
}

int main(int argc, char **argv) {
	int spi_bus, spi_line;
	int ret, opt;
	bool set_mode = false, master_mode = false, full_time_cs = false,
	     format_16bit = false;
	struct spi_device *dev;

	while (-1 != (opt = getopt(argc, argv, "lhsmfx"))) {
		switch (opt) {
		case 's':
			set_mode = true;
			master_mode = false;
			break;
		case 'm':
			set_mode = true;
			master_mode = true;
			break;
		case 'f':
			full_time_cs = true;
			break;
		case 'x':
			format_16bit = true;
			break;
		case 'l':
			list_spi_devices();
			return 0;
		case 'h':
			print_help(argv);
			return 0;
		}
	}

	if (argc < 3 + optind) {
		print_help(argv);
		return 0;
	}

	spi_bus = strtol(argv[optind], NULL, 0);
	spi_line = strtol(argv[optind + 1], NULL, 0);

	dev = spi_dev_by_id(spi_bus);
	if (dev == NULL) {
		printf("Failed to select bus #%d\n", spi_bus);
		return -ENOENT;
	}
	dev->is_master = true;

	if (set_mode) {
		if (master_mode) {
			if (spi_set_master_mode(dev)) {
				printf("Failed to set SPI master mode\n");
				return -1;
			}
		}
		else {
			if (spi_set_slave_mode(dev)) {
				printf("Failed to set SPI slave mode\n");
				return -1;
			}
		}
	}

	dev->flags |= SPI_CS_ACTIVE;
	if (full_time_cs)
		dev->flags &= ~SPI_CS_INACTIVE;
	else
		dev->flags |= SPI_CS_INACTIVE;

	ret = spi_select(dev, spi_line);
	if (ret < 0) {
		printf("Failed to select line #%d!\n", spi_line);
		return ret;
	}

	printf("Received data:");
	for (int i = optind + 2; i < argc; i++) {
		uint16_t buf_in, buf_out;
		buf_out = strtol(argv[i], NULL, 0);
		if (i + 1 == argc)
			dev->flags |= SPI_CS_INACTIVE;
		spi_transfer(dev, (uint8_t *)&buf_out, (uint8_t *)&buf_in, 1);
		if (format_16bit) {
			printf(" 0x%04x", buf_in);
		}
		else {
			printf(" 0x%02x", buf_in);
		}
	}
	printf("\n");

	return 0;
}
