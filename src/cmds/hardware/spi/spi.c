/**
 * @file spi.c
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
#include <util/math.h>

#include <drivers/spi.h>
#include <util/pretty_print.h>

static void print_help(char **argv) {
	printf("Transfer bytes via SPI bus\n");
	printf("Usage:\n");
	printf("%s spi [-t<method>] [-d<clkdiv>] [lsmh] bus_number bus_line [byte0 byte1 [...]]\n", argv[0]);
}

static int test_poll(struct spi_device *spi, int spi_line, uint8_t *dataOut, uint8_t *dataIn, int clkdiv) {
		int res;

		//////  SPI_MODE_0
		spi->flags |= SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_0) | SPI_CS_DIVSOR(clkdiv);
		res = spi_select(spi, spi_line);

		// read/write
		res = spi_transfer(spi, dataOut, dataIn, 4);
		// write only
		res = spi_transfer(spi, dataOut, NULL, 4);
		// read only
		res = spi_transfer(spi, NULL, dataIn, 4);
		// read/write
		res = spi_transfer(spi, dataOut, dataIn, 32);
		// write only
		res = spi_transfer(spi, dataOut, NULL, 32);
		// read only
		res = spi_transfer(spi, NULL, dataIn, 32);

		//////  SPI_MODE_1
		spi->flags = SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_1) | SPI_CS_DIVSOR(clkdiv);
		res = spi_select(spi, spi_line);

		// read/write
		res = spi_transfer(spi, dataOut, dataIn, 4);

		//////  SPI_MODE_2
		spi->flags = SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_2) | SPI_CS_DIVSOR(clkdiv);
		res = spi_select(spi, spi_line);

		// read/write
		res = spi_transfer(spi, dataOut, dataIn, 4);

		//////  SPI_MODE_3
		spi->flags = SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_3) | SPI_CS_DIVSOR(clkdiv);
		res = spi_select(spi, spi_line);

		// read/write
		res = spi_transfer(spi, dataOut, dataIn, 4);
		return res;
}

typedef enum {
	UNKNOWN = 0, SEND_COMPLETE, DATA_RECEIVED
} SPI_status;

volatile static SPI_status spi_stat = UNKNOWN;
volatile static int spi_bytes = 0;

static void data_sent(struct spi_device *data) {
	spi_bytes = data->count;
	spi_stat = SEND_COMPLETE;
};

static void data_received(struct spi_device *data) {
	spi_bytes = data->count;
	spi_stat = DATA_RECEIVED;
};

static int test_interrupt(struct spi_device *dev, int spi_line, uint8_t *dataOut, uint8_t *dataIn, int clkdiv, int bytes) {
	int ret;

	dev->flags |= SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_0) | SPI_CS_DIVSOR(clkdiv)
		| SPI_CS_IRQD | SPI_CS_IRQR;
	dev->send_complete = data_sent;
	dev->received_data = data_received;
	ret = spi_select(dev, spi_line);
	// trigger initiating send interrupt
	ret = spi_transfer(dev, dataOut, dataIn, bytes);

	return ret;
}

static void list_spi_devices(void) {
	for (int i = 0; i < SPI_REGISTRY_SZ; i++) {
		struct spi_device *s = spi_dev_by_id(i);

		if (s == NULL) {
			continue;
		}

		printf("Bus %d: ", i);

		if (s->dev == NULL) {
			printf("(unnamed)\n");
			continue;
		}

		printf("%s\n", s->dev->name);
	}
}

static void report(char *msg, uint8_t *data, int len) {

	printf("%s(%d):",msg,len);
	for (int i = 0; i < len; i++) {
		printf("%02x", data[i]);
	}	
	printf("\n");
}

int main(int argc, char **argv) {
	int spi_bus, spi_line;
	int ret = 0, opt, optargs = 0;
	bool set_mode = false, master_mode = false;
	struct spi_device *dev;
	char method = '_';
	uint16_t clkdiv = 0;
	uint8_t dataOut[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0xAD, 0xBE, 0xEF, 0xDE, 0x02, 0xBE, 0xEF, 0xDE, 0xAD, 0x03, 0xEF,
						0xDE, 0xAD, 0xBE, 0x04, 0x05, 0xAD, 0xBE, 0xEF, 0xDE, 0x06, 0xBE, 0xEF, 0xDE, 0xAD, 0x07, 0xEF};
	uint8_t dataIn[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int bCount = 0;
	#define DATA_INOUT_BYTES 32
	#define MIN_ARGS 3

	while (-1 != (opt = getopt(argc, argv, "lhsmt:d:"))) {
		switch(opt) {
		case 's':
			set_mode = true;
			master_mode = false;
			optargs++;
			break;
		case 'm':
			set_mode = true;
			master_mode = true;
			optargs++;
			break;
		case 't':
			method = optarg[0];
			optargs++;
			break;
		case 'd':
			clkdiv = strtol(optarg, NULL, 0);
			optargs++;
			break;
		case 'l':
			list_spi_devices();
			return 0;
		case 'h':
			print_help(argv);
			return 0;
		}
	}

	if (argc < MIN_ARGS + optargs) {
		print_help(argv);
		return 0;
	}

	spi_bus = strtol(argv[1 + optargs], NULL, 0);
	spi_line = strtol(argv[2 + optargs], NULL, 0);

	dev = spi_dev_by_id(spi_bus);
	dev->flags = 0; 	
	if (dev == NULL) {
		printf("Failed to select bus #%d\n", spi_bus);
		return -ENOENT;
	}

	if (set_mode) {
		if (master_mode) {
			if (spi_set_master_mode(dev)) {
				printf("Failed to set SPI master mode\n");
				return -1;
			}
		} else {
			if (spi_set_slave_mode(dev)) {
				printf("Failed to set SPI slave mode\n");
				return -1;
			}
		}
	}

	// copy provided bytes into send buffer
	for (int i = MIN_ARGS; i < min(argc - optargs,DATA_INOUT_BYTES+MIN_ARGS); i++) {
		uint8_t buf_out;

		buf_out = strtol(argv[i + optargs], NULL, 0);
		dataOut[bCount++] = buf_out & 0xFF;
	}
	// If no data passed in command, set ptr to default count of data bytes
	if( bCount == 0 ) bCount = DATA_INOUT_BYTES;


	printf("\nexec: %c, clkdiv: %d, bus: %d, cs: %d\n",method, clkdiv, spi_bus, spi_line);
	report("\nS", dataOut, bCount);

	switch(method) {
	case 'i':
		ret = test_interrupt(dev, spi_line, dataOut, dataIn, clkdiv, -bCount);

		// Busy wait until transfer done
		while(spi_stat == UNKNOWN);
		printf("Status %d, Bytes remain/received: %d\n", (int)spi_stat, spi_bytes);
		break;
	case 'w':
		printf("Waiting for bytes received.\n");
		ret = test_interrupt(dev, spi_line, NULL, dataIn, clkdiv, -bCount);

		// Busy wait until transfer done
		while(spi_stat == UNKNOWN);
		printf("Status %d, Bytes remain/received: %d\n", (int)spi_stat, spi_bytes);
		break;
	case 'd':
		printf("To be implemented.\n");
		return ret;
		break;
	case 'p':
		ret = test_poll(dev, spi_line, dataOut, dataIn, clkdiv);
		break;
	default:
		dev->flags |= SPI_CS_ACTIVE;
		dev->flags |= SPI_CS_INACTIVE;
		ret = spi_select(dev, spi_line);
		if (ret < 0) {
			printf("Failed to select line #%d!\n", spi_line);
			return ret;
		}
		spi_transfer(dev, dataOut, dataIn, bCount);
		break;
	}
	report("\nR", dataIn, bCount);

	return ret;
}
