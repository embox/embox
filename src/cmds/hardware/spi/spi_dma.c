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
#include <drivers/dma/dma.h>

#include <string.h>

static void print_help(char **argv) {
	printf("Transfer bytes via SPI bus\n");
	printf("Usage:\n");
	printf("%s spi [-t<method>] [-d<clkdiv>] [lsmh] bus_number bus_line [byte0 byte1 [...]]\n", argv[0]);
}

static int test_poll(struct spi_device *spi, int spi_line, uint8_t *dataOut, uint8_t *dataIn, int clkdiv) {
		int res;

		//////  SPI_MODE_0
		spi->spi_dflags |= SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_0) | SPI_CS_DIVSOR(clkdiv);
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
		spi->spid_flags = SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_1) | SPI_CS_DIVSOR(clkdiv);
		res = spi_select(spi, spi_line);

		// read/write
		res = spi_transfer(spi, dataOut, dataIn, 4);

		//////  SPI_MODE_2
		spi->spid_flags = SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_2) | SPI_CS_DIVSOR(clkdiv);
		res = spi_select(spi, spi_line);

		// read/write
		res = spi_transfer(spi, dataOut, dataIn, 4);

		//////  SPI_MODE_3
		spi->spid_flags = SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_3) | SPI_CS_DIVSOR(clkdiv);
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

static void data_sent(struct spi_controller *data, int cnt) {
	spi_bytes = cnt;
	spi_stat = SEND_COMPLETE;
};

static void data_received(struct spi_controller *data, int cnt) {
	spi_bytes = cnt;
	spi_stat = DATA_RECEIVED;
};

static irq_return_t data_block_complete(unsigned int irq_nr, void *dma_data) {
	irq_return_t ret = IRQ_HANDLED;
	Dma *dev = (Dma *)dma_data;
	spi_bytes = dev->txfr_len;
	spi_stat = SEND_COMPLETE;

	return ret;
}

static int test_interrupt(struct spi_device *dev, int spi_line, uint8_t *dataOut, uint8_t *dataIn, int clkdiv, int bytes) {
	int ret;

	dev->spid_flags |= SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_0) | SPI_CS_DIVSOR(clkdiv)
		| SPI_CS_IRQD | SPI_CS_IRQR;

	spi_irq_prepare(dev->spid_spi_cntl, data_sent, data_received);

	ret = spi_select(dev, spi_line);
	// trigger initiating send interrupt
	ret = spi_transfer(dev, dataOut, dataIn, bytes);

	return ret;
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

static void fill_test(uint32_t *memLoc, uint32_t bytes) {
    uint32_t pattern = 0xDEADBEEF;
    for(uint32_t i = 0 ; i < bytes / sizeof(uint32_t) ; i+= 2 ) {
        memLoc[i] = pattern;
        memLoc[i+1] = i;
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
	dev->spid_flags = 0;
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

	switch(method) {
	case 'i':
		report("\nS", dataOut, bCount);
		ret = test_interrupt(dev, spi_line, dataOut, dataIn, clkdiv, -bCount);

		// Busy wait until transfer done
		while(spi_stat == UNKNOWN);
		report("\nR", dataIn, bCount);
		printf("Status %d, Bytes remain/received: %d\n", (int)spi_stat, spi_bytes);
		break;
	case 'w':
		report("\nS", dataOut, bCount);
		printf("Waiting for bytes received.\n");
		ret = test_interrupt(dev, spi_line, NULL, dataIn, clkdiv, -bCount);

		// Busy wait until transfer done
		while(spi_stat == UNKNOWN);
		report("\nR", dataIn, bCount);
		printf("Status %d, Bytes remain/received: %d\n", (int)spi_stat, spi_bytes);
		break;
	case 'd':
		{
			#define DMA_XFER_LEN 0x1000
			#define DMA_XFER_BLOCKS 4
			#define DMA_MEM (DMA_XFER_LEN * DMA_XFER_BLOCKS * 2)
			#define DMA_CHAN_OUT 4
			#define DMA_CHAN_IN 5
			struct dma_ctrl_blk *cbp_out, *cbp_out_first = NULL;
			struct dma_ctrl_blk *cbp_in, *cbp_in_first = NULL;
			uint32_t error_flags = 0x00;

			struct dma_mem_handle *mem_handle = dma_malloc(DMA_MEM);

			fill_test(mem_handle->physical_addr, DMA_MEM / 2);

			// Configure SPI
			dev->spid_flags |= SPI_CS_ACTIVE | SPI_CS_MODE(SPI_MODE_0) | SPI_CS_DIVSOR(clkdiv)
				| SPI_CS_DMAEN;
			spi_dma_prepare(dev->spid_spi_cntl,
					data_block_complete, DMA_CHAN_OUT, DMA_CHAN_IN,
					DMA_LEVELS(12, 4, 12, 4));

			// Note: These values are important to tune and check in multi-block transfers
			// A symtom of improper values is the receipt memory locations are missing 
			// values but retain their correct position in memory from the transmit memory
			// values.
			//dev->dma_levels = DMA_LEVELS(12, 4, 12, 4);

			ret = spi_select(dev, spi_line);

			/* NOTE: The BCM2835 Peripheral document speaks of putting a uint32_t value of the following
			 * form in the data stream on the outgoing data block but with the latest firmware
			 * this does not seem necessary and this extra byte is unwantedly transmitted.  So, it isn't
			 * present in these examples but I note it as a deviation just in case you're on a firmware
			 * that requires it.
			 * 
			 *  // Place SPI config on first word of data stream
			 *	DMA_TXFR_LEN_YLENGTH(bytes - sizeof(uint32_t)) | (REGS_SPI0->cs & 0x0000007F) | SPI0_CS_TA;
			 *
			 */		

#if 0		// Send a single block
			// Send block 
			cbp_out = (struct dma_ctrl_blk *)init_dma_block_spi_in(dev, mem_handle, 0
				, (void *)(mem_handle->physical_addr + sizeof(struct dma_ctrl_blk) )
				, DMA_XFER_LEN - sizeof(struct dma_ctrl_blk) );
			cbp_out->nextconbk = 0;								
			// Receive block 
			cbp_in = (struct dma_ctrl_blk *)init_dma_block_spi_out(dev, mem_handle, 0 + DMA_XFER_LEN * DMA_XFER_BLOCKS
				, (void *)(mem_handle->physical_addr + DMA_XFER_LEN * DMA_XFER_BLOCKS + sizeof(struct dma_ctrl_blk) )
				, DMA_XFER_LEN - sizeof(struct dma_ctrl_blk) );
			cbp_in->nextconbk = 0;
			cbp_in->ti |= DMA_TI_INTEN;

			if(cbp_out_first == NULL) cbp_out_first = cbp_out;
			if(cbp_in_first == NULL) cbp_in_first = cbp_in;

#else		// Send multple blocks

			// Layout control blocks
			for(uint32_t i = 0 ; i < DMA_XFER_LEN * DMA_XFER_BLOCKS; i += ( DMA_XFER_LEN ) ) {
				// Send block 
				cbp_out = init_dma_block_spi_in(dev->spid_spi_cntl, mem_handle, i 
					, (void *)(mem_handle->physical_addr + i + sizeof(struct dma_ctrl_blk) )
					, DMA_XFER_LEN - sizeof(struct dma_ctrl_blk)
					, ( i + DMA_XFER_LEN < DMA_XFER_LEN * DMA_XFER_BLOCKS
						? mem_handle->physical_addr + i + DMA_XFER_LEN
						: 0 )
					, false
					);
				// Receive block 
				cbp_in = init_dma_block_spi_out(dev->spid_spi_cntl, mem_handle, i + DMA_MEM / 2 
					, (void *)(mem_handle->physical_addr + i + DMA_MEM / 2 + sizeof(struct dma_ctrl_blk) )
					, DMA_XFER_LEN - sizeof(struct dma_ctrl_blk)
					, ( i + DMA_XFER_LEN < DMA_XFER_LEN * DMA_XFER_BLOCKS
						? mem_handle->physical_addr + i + DMA_XFER_LEN + DMA_MEM / 2 
						: 0 )
					// Set last Receiving control block with enabled interrupt
					, !( i + DMA_XFER_LEN < DMA_XFER_LEN * DMA_XFER_BLOCKS ) 
					);
				if(cbp_out_first == NULL) cbp_out_first = cbp_out;
				if(cbp_in_first == NULL) cbp_in_first = cbp_in;
			}
#endif
			// Initiate transfer with callback interrupts
			ret = spi_transfer(dev, (uint8_t *)cbp_out_first
				, (uint8_t *)cbp_in_first
				, DMA_XFER_BLOCKS * ( DMA_XFER_LEN - sizeof(struct dma_ctrl_blk) ) );
	
			// busy wait until interrupt that DMA is done
			while(spi_stat == UNKNOWN ) {
				// busy polling wait
				// TODO: This polling check should not be necessary, the interrupt should
				// be firing and setting spi_stat to indicate transfer is complete
#if 0
				if(!dma_in_progress_status(dev->dma_chan_out, &error_flags)) {
					break;
				}
#endif
				if(error_flags) {
					printf("DMA_DEBUG_FLAGS %x\n", error_flags);
				}
			};

			dma_free(mem_handle);
			spi_stat = UNKNOWN;
		}
		break;
	case 'p':
		report("\nS", dataOut, 32);
		ret = test_poll(dev, spi_line, dataOut, dataIn, clkdiv);
		report("\nR", dataIn, 32);
		break;
	default:
		dev->spid_flags |= SPI_CS_ACTIVE;
		dev->spid_flags |= SPI_CS_INACTIVE;
		ret = spi_select(dev, spi_line);
		if (ret < 0) {
			printf("Failed to select line #%d!\n", spi_line);
			return ret;
		}
		report("\nS", dataOut, bCount);
		spi_transfer(dev, dataOut, dataIn, bCount);
		report("\nR", dataIn, bCount);
		break;
	}

	return ret;
}
