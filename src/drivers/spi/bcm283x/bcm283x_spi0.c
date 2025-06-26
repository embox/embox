/**
 * @file bcm283x_spi0.c
 * @brief 
 * @author kpishere
 * @version 0.1
 * @date 2021.07.16
 */
#include <errno.h>

#include <drivers/common/memory.h>
#include <drivers/spi.h>
#include <embox/unit.h>
#include <framework/mod/options.h>
#include <hal/reg.h>
#include <util/log.h>
#include <util/math.h>
#include <kernel/printk.h>
#include <asm/delay.h>
#include <assert.h>

#include <drivers/dma/pl330_dma.h>

#include <drivers/gpio.h>
#include <drivers/gpio/bcm283x/bcm283x_gpio.h>
#include <drivers/spi.h>

#include "bcm283x_spi0.h"
#include "bcm283x_spi_dev.h"

#define PBASE                     OPTION_GET(NUMBER,base_addr)
#define SPI_BUS_CLOCK_DIVISOR     OPTION_GET(NUMBER,spi_bus_clock_divisor)
#define SPI_INT0                  OPTION_GET(NUMBER,spi_int)



// command block must be 256 bit aligned in memory
#define MEM_ALGN_256 (0x00000020)

// Toggle an extra GPIO pin at various points to debug/test timing
#if 0
    #define DRIVER_TESTING
    #define PIN ( 1 << 25 )
#endif

/* Errata to BCM2835 behaviour: documentation states that the SPI0 DLEN register is only used for DMA. 
 * However, even when DMA is not being utilized, setting it from a value != 0 or 1 gets rid of an 
 * excess idle clock cycle that is present when transmitting each byte. (by default in Polled SPI 
 * Mode each 8 bits transfer in 9 clocks) With DLEN=2 each byte is clocked to the bus in 8 cycles, 
 * observed to improve max throughput from 56.8mbps to 63.3mbps (+11.4%, quite close to the 
 * theoretical +12.5%)
 * https://www.raspberrypi.org/forums/viewtopic.php?f=44&t=181154
 */
#define DLEN_NO_DMA_VALUE 2;

struct bcm283x_spi_regs {
    uint32_t cs; /* SPI Master Control and Status */
    
/* RW: DMA Mode (DMAEN set)
    If TA is clear, the first 32-bit write to this register will control 
    SPIDLEN and SPICS. Subsequent reads and writes will be taken as 
    four-byte data words to be read/written to the FIFOs Poll/Interrupt 
    Mode (DMAEN clear, TA set) Writes to the register write bytes to TX FIFO. 
    Reads from register read bytes from the RX FIFO
 */
    uint32_t fifo; /* SPI Master TX and RX FIFOs */

/* RW: 15:0 Clock Divider SCLK = Core Clock / CDIV
    If CDIV is set to 0, the divisor is 65536. The divisor must be a power of 2.
    Odd numbers rounded down. The maximum SPI clock rate is of the APB clock.
 */
    uint32_t clk; /* SPI Master Clock Divider */

/* RW: 15:0 Data Length - The number of bytes to transfer.
    This field is only valid for DMA mode (DMAEN set) and controls how many bytes 
    to transmit (and therefore receive).
 */     
    uint32_t dlen; /* SPI Master Data Length */

/* RW: 3:0 This sets the Output Hold delay in APB clocks. A value of 0 causes a 1 clock delay. */
    uint32_t ltoh; /* SPI LOSSI mode TOH */

/* RW: This register controls the generation of the DREQ and Panic signals to an 
    external DMA engine The DREQ signals are generated when the FIFOs reach their 
    defined levels and need servicing. The Panic signals instruct the external DMA engine 
    to raise the priority of its AXI requests.*/
    uint32_t dc; /* SPI DMA DREQ Controls */
};

#define REGS_SPI0 ((struct bcm283x_spi_regs *)(PBASE))
#define BCM283X_SPI0_TX_FIFO_LEN (16*sizeof(uint32_t))
#define BCM283X_SPI0_RX_FIFO_LEN (16*sizeof(uint32_t))
#define BCM283X_SPI0_TX_FIFO_FL  (4*sizeof(uint32_t))
#define BCM283X_SPI0_RX_FIFO_HW  (3*sizeof(uint32_t))

#define BCM283X_PRIV(x) ((struct bcm283x_spi_dev *)(x)->priv)

static int bcm283x_spi0_init(void) {
    uint32_t pins_spi_Alt0 =( 1 << 7 ) | ( 1 << 8 ) | ( 1 << 9 ) | ( 1 << 10 ) | ( 1 << 11 );    
    gpio_setup_mode(GPIO_PORT_A, pins_spi_Alt0, GPIO_MODE_ALT_SET(GFAlt0) );

#ifdef DRIVER_TESTING
    gpio_setup_mode(GPIO_PORT_A, PIN, GPIO_MODE_OUTPUT);
#endif

    // Initialize the Control and Status register to defaults:
    //CS=0 (Chip Select), CPHA=0 (Clock Phase), CPOL=0 (Clock Polarity), 
    //CSPOL=0 (Chip Select Polarity), TA=0 (Transfer not active), and reset TX and RX queues.
    REGS_SPI0->cs = SPI0_CS(0);
    REGS_SPI0->clk = SPI_BUS_CLOCK_DIVISOR;
    REGS_SPI0->dlen = DLEN_NO_DMA_VALUE;

    return 0;
}

static irq_return_t bcm283x_spi_intrd_irq_handler(unsigned int irq_nr, void *data);

static int bcm283x_spi0_attach(struct spi_controller *dev) {
    int res = 0;

    if( !(REGS_SPI0->cs & SPI0_CS_INTD) && !(REGS_SPI0->cs & SPI0_CS_INTR) ) {
        res = irq_attach(SPI_INT0, bcm283x_spi_intrd_irq_handler, 0x00, dev, "SPI0 IRQ");
    }
    return res;
}

static int bcm283x_spi0_select(struct spi_controller *dev, int cs) {
    int res = 0;
    if (cs < 0 || cs > 3) {
        log_error("Only cs=0..3 are avalable!");
        return -EINVAL;
    }
    if( dev->flags & SPI_CS_ACTIVE ) {
    }
    if( dev->flags & SPI_CS_INACTIVE ) {
    }

    // If flag set for either interrupt option, register interrupt function
    if( dev->flags & SPI_CS_IRQD ) {   
        res = bcm283x_spi0_attach(dev);     
        REGS_SPI0->cs |= SPI0_CS_INTD;
    }
    if( dev->flags & SPI_CS_IRQR ) {        
        res = bcm283x_spi0_attach(dev);     
        REGS_SPI0->cs |= SPI0_CS_INTR;
    }
    // If no flags set for interrupt, detatch interrupt
    if( !(dev->flags & SPI_CS_IRQR) && !(dev->flags & SPI_CS_IRQD)
        && irq_nr_valid(SPI_INT0) == 0 ) {
        res = irq_detach(SPI_INT0, dev);
    }

    // Enable DMA
    if( dev->flags & SPI_CS_DMAEN 
        && !(REGS_SPI0->cs & SPI0_CS_DMAEN) ) {
        assert( BCM283X_PRIV(dev)->dma_chan_out != BCM283X_PRIV(dev)->dma_chan_in );

        REGS_SPI0->cs |= SPI0_CS_DMAEN;
        REGS_SPI0->dc = BCM283X_PRIV(dev)->dma_levels;

        /* Set Panic and Priority levels at High 2/3rds and then Low 1/3rd respectively
         * If they are not set, DMA_LEVELS() must be about double higher.
         */
        dma_config_extended(BCM283X_PRIV(dev)->dma_chan_in, NULL, DMA_CS_PANIC_PRIORITY(0x0C) | DMA_CS_PRIORITY(0x06) );
        // Channel out receives data from SPI fifo to memory in    
        dma_config_extended(BCM283X_PRIV(dev)->dma_chan_out, BCM283X_PRIV(dev)->dma_complete,
        		DMA_CS_PANIC_PRIORITY(0x0C) | DMA_CS_PRIORITY(0x06));
    }


    // If no flags set for interrupt, cause interrupt to detatch
    if( !(dev->flags & SPI_CS_DMAEN) && (REGS_SPI0->cs & SPI0_CS_DMAEN) ) {
        dma_config_extended(BCM283X_PRIV(dev)->dma_chan_out, NULL, 0x00);
        REGS_SPI0->cs &= ~( SPI0_CS_DMAEN );
    }

    // set default CPHA=0 (Clock Phase), CPOL=0 (Clock Polarity)
    REGS_SPI0->cs &= ~(SPI0_CS_CPOL | SPI0_CS_CPHA); 
    if(dev->flags & SPI_CS_MODE(SPI_MODE_1)) REGS_SPI0->cs |= SPI0_CS_CPHA;
    if(dev->flags & SPI_CS_MODE(SPI_MODE_2)) REGS_SPI0->cs |= SPI0_CS_CPOL;

    // A clock divisor value is set
    if( (dev->flags >> 16) ) {
        REGS_SPI0->clk = (dev->flags >> 16);
    }

    // Set the CS lines
    REGS_SPI0->cs &= ~SPI0_CS(0xFF);
    REGS_SPI0->cs |= SPI0_CS(cs);

    // Set the CS hold delay
    REGS_SPI0->ltoh = 0;
    return res;
}

static int bcm283x_spi0_do_transfer(struct spi_controller *dev, uint8_t *inbuf
        , uint8_t *outbuf, int tx_count, int rx_count) {
    int tx_cnt, rx_cnt;

    irq_lock();
    tx_cnt = rx_cnt = 0;
    /* Do not add log_debug() or some another stuff here, */
    while ( ( tx_cnt < tx_count && inbuf != NULL ) 
        ||  (rx_cnt < rx_count && outbuf != NULL ) ) {
        if(tx_cnt < tx_count ) {
            if( inbuf != NULL ) {
                // Wait for FIFO to have 1-byte space    
                while( !(REGS_SPI0->cs & SPI0_CS_TXD) ) {
#ifdef DRIVER_TESTING
                    gpio_set(GPIO_PORT_A, PIN, GPIO_PIN_HIGH );
                    gpio_set(GPIO_PORT_A, PIN, GPIO_PIN_LOW );
#endif
                }; 
                REGS_SPI0->fifo = inbuf[tx_cnt++];
            } else { 
                // write without waiting for buffer ready
                REGS_SPI0->fifo = 0x00;
            }
        }
        if( rx_cnt < rx_count && outbuf != NULL ) {
            while ( !(REGS_SPI0->cs & SPI0_CS_RXD) ); // Wait for rx byte
            outbuf[rx_cnt++] = REGS_SPI0->fifo;
        }
    }
    // read out FIFO incoming buffer
    while( REGS_SPI0->cs & SPI0_CS_RXD ) { 
        uint32_t discard;
        discard += REGS_SPI0->fifo;    
    }
    // Wait for tx to complete
    while( !(REGS_SPI0->cs & SPI0_CS_DONE) ) {
#ifdef DRIVER_TESTING
        gpio_set(GPIO_PORT_A, PIN, GPIO_PIN_HIGH );
        gpio_set(GPIO_PORT_A, PIN, GPIO_PIN_LOW );
#endif
    };   
    irq_unlock();

    return max(tx_cnt, rx_cnt);         
}

// Interrupt for data send required or complete
static irq_return_t bcm283x_spi_intrd_irq_handler(unsigned int irq_nr, void *data) {
	irq_return_t ret = IRQ_HANDLED;
	struct spi_controller *dev = (struct spi_controller *) data;

	// Transmit and receive with CS asserted
	if ((REGS_SPI0->cs & (SPI0_CS_DONE | SPI0_CS_TA))
			&& BCM283X_PRIV(dev)->count > 0) {
		BCM283X_PRIV(dev)->count -= bcm283x_spi0_do_transfer(dev,
		BCM283X_PRIV(dev)->in, BCM283X_PRIV(dev)->out,
				min(BCM283X_SPI0_TX_FIFO_FL, BCM283X_PRIV(dev)->count),
				min(BCM283X_SPI0_TX_FIFO_FL, BCM283X_PRIV(dev)->count));
		if (BCM283X_PRIV(dev)->count <= 0) {
			REGS_SPI0->cs &= ~SPI0_CS_TA; // De-assert
		}
		if (BCM283X_PRIV(dev)->send_complete && BCM283X_PRIV(dev)->count <= 0) {
			BCM283X_PRIV(dev)->send_complete(dev, BCM283X_PRIV(dev)->count);
		}
	}

	// Receive with CS un-asserted
	if ((REGS_SPI0->cs & SPI0_CS_RXR)) {
		BCM283X_PRIV(dev)->count = bcm283x_spi0_do_transfer(dev,
				BCM283X_PRIV(dev)->in, BCM283X_PRIV(dev)->out,
					min(BCM283X_SPI0_RX_FIFO_HW, BCM283X_PRIV(dev)->count),
					min(BCM283X_SPI0_RX_FIFO_HW, BCM283X_PRIV(dev)->count));
		if (BCM283X_PRIV(dev)->received_data && BCM283X_PRIV(dev)->count > 0) {
			BCM283X_PRIV(dev)->send_complete(dev, BCM283X_PRIV(dev)->count);
		}
	}
	return ret;
}

static struct dma_ctrl_blk *bcm283x_init_dma_block_spi_in(struct spi_controller *dev,
		struct dma_mem_handle  *mem_handle, uint32_t offset, void *src, uint32_t bytes,
		struct dma_ctrl_blk *next_conbk, bool int_enable) {

	assert( (((uint32_t)(mem_handle->physical_addr) + offset) & ~MEM_ALGN_256 ) == ((uint32_t)(mem_handle->physical_addr) + offset));

	struct dma_ctrl_blk *cbp = (struct dma_ctrl_blk *)(mem_handle->physical_addr + offset);
	cbp->ti = DMA_TI_PERMAP(DMA_PERMAP_SPI_TX) | DMA_TI_SRC_INC | DMA_TI_DEST_DREQ | DMA_TI_WAIT_RESP;
	cbp->dest_ad = (uint32_t)DMA_PERF_TO_BUS((uint32_t)&(REGS_SPI0->fifo));
	cbp->stride = 0x0;

	cbp->source_ad = (uint32_t)DMA_PHYS_TO_BUS((uint32_t)src);
	cbp->txfr_len = bytes;
	cbp->nextconbk = ( next_conbk == NULL ? 0x00 : (uint32_t)DMA_PHYS_TO_BUS((uint32_t)next_conbk) );

	if(int_enable){
		cbp->ti |= DMA_TI_INTEN;
	}
	else {
		cbp->ti &= ~DMA_TI_INTEN;
	}

	return cbp;
}

static struct dma_ctrl_blk *bcm283x_init_dma_block_spi_out(struct spi_controller *dev,
		struct dma_mem_handle  *mem_handle, uint32_t offset, void *dest, uint32_t bytes,
		struct dma_ctrl_blk *next_conbk, bool int_enable) {

	assert( (((uint32_t)(mem_handle->physical_addr) + offset) & ~MEM_ALGN_256 ) == ((uint32_t)(mem_handle->physical_addr) + offset));

	struct dma_ctrl_blk *cbp = (struct dma_ctrl_blk *)(mem_handle->physical_addr + offset);
	cbp->ti = DMA_TI_PERMAP(DMA_PERMAP_SPI_RX) | DMA_TI_DEST_INC | DMA_TI_SRC_DREQ | DMA_TI_WAIT_RESP;
	cbp->source_ad = (uint32_t)DMA_PERF_TO_BUS((uint32_t)&(REGS_SPI0->fifo));
	cbp->stride = 0x0;

	cbp->dest_ad = (uint32_t)DMA_PHYS_TO_BUS((uint32_t)dest);
	cbp->txfr_len = bytes;
	cbp->nextconbk = ( next_conbk == NULL ? 0x00 : (uint32_t)DMA_PHYS_TO_BUS((uint32_t)next_conbk) );

	if(int_enable) {
		cbp->ti |= DMA_TI_INTEN;
	}
	else {
		cbp->ti &= ~DMA_TI_INTEN;
	}

	return cbp;
}

/*
    // Hi 16 bits are length, lower 8 bits are the lower 8 bits of SPI control register
 * 
 * For SPI Device: txfr_len is in top sixteen bits and control register settings are in [7:0]
 * (the bottom eight bits) for TA = 1, CS, CPOL, CPHA 
*/

static int bcm283x_spi0_transfer(struct spi_controller *dev, uint8_t *inbuf, uint8_t *outbuf, int count) {

    // Interrupt mode
    if( ( (REGS_SPI0->cs & SPI0_CS_INTD) || (REGS_SPI0->cs & SPI0_CS_INTR) ) ) {
        if(count < 0) {
            // count < 0 is signal to trigger interrupt and return
            BCM283X_PRIV(dev)->count = -1 * count;    /* set the target amount to output in series of interrupt transfers */
            BCM283X_PRIV(dev)->in = inbuf;
            BCM283X_PRIV(dev)->out = outbuf;
            REGS_SPI0->dlen = DLEN_NO_DMA_VALUE;
            REGS_SPI0->cs |= SPI0_CS_CLEAR( SPI0_tx_fifo | SPI0_rx_fifo ) | SPI0_CS_TA; // clear FIFO and Assert
            return 0;
        } else {
            // called in interrupt handler to send/receive data
            BCM283X_PRIV(dev)->count -= bcm283x_spi0_do_transfer(dev, inbuf, outbuf, min(BCM283X_SPI0_TX_FIFO_FL,count), BCM283X_SPI0_RX_FIFO_HW);
            REGS_SPI0->cs &= ~SPI0_CS_TA; // De-assert
            return 0;
        }
    }

    // DMA Mode
    if(REGS_SPI0->cs & SPI0_CS_DMAEN) { 
        REGS_SPI0->dlen = count;    

        // Receive - start dma transfer
        dma_transfer_conbk(BCM283X_PRIV(dev)->dma_chan_out, (volatile struct dma_ctrl_blk *)outbuf);
        // Transmit - start dma transfer
        dma_transfer_conbk(BCM283X_PRIV(dev)->dma_chan_in, (volatile struct dma_ctrl_blk *)inbuf);
        // Activate SPI
        REGS_SPI0->cs |= SPI0_CS_ADCS | SPI0_CS_CLEAR( SPI0_tx_fifo | SPI0_rx_fifo ) | SPI0_CS_TA; // clear FIFO and Assert
    } else { 
        // Poll mode - bytes send, bytes receive
        REGS_SPI0->cs |= SPI0_CS_CLEAR( SPI0_tx_fifo | SPI0_rx_fifo ) | SPI0_CS_TA; // clear FIFO and Assert

        /* because we need to write all tx data before transfer competed. */
        REGS_SPI0->dlen = DLEN_NO_DMA_VALUE;

        bcm283x_spi0_do_transfer(dev, inbuf, outbuf, count, count);
        REGS_SPI0->cs &= ~SPI0_CS_TA; // De-assert
    }
    return 0;
}

struct spi_controller_ops bcm283x_spi0_ops = {
    .select   = bcm283x_spi0_select,
    .transfer = bcm283x_spi0_transfer,
};

PERIPH_MEMORY_DEFINE(bcm283x_spi0, PBASE, sizeof(struct bcm283x_spi_regs));

static struct bcm283x_spi_dev bcm283x_spi_dev = {
		.regs = REGS_SPI0,
		.init_dma_block_spi_in = bcm283x_init_dma_block_spi_in,
		.init_dma_block_spi_out = bcm283x_init_dma_block_spi_out
};

SPI_CONTROLLER_DEF("spi0", &bcm283x_spi0_ops, &bcm283x_spi_dev, 0);

SPI_DEV_DEF("spi0_0", NULL, NULL, 0, 0);

EMBOX_UNIT_INIT(bcm283x_spi0_init);
