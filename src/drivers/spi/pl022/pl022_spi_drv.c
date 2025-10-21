/**
 * @file
 * @brief
 * @author Anton Bondarev
 * @version
 * @date 29.03.2025
 */

#include <util/log.h>

#include <errno.h>

#include <hal/reg.h>

#include <drivers/spi.h>


#include "pl022_spi.h"


struct pl022_regs {
	uint32_t cr0;
	uint16_t cr1;
	uint16_t dmacr;
};

/*
 * Macros to access SSP Registers with their offsets
 */
 #define SSP_CR0(r)     (r + 0x000)
 #define SSP_CR1(r)     (r + 0x004)
 #define SSP_DR(r)      (r + 0x008)
 #define SSP_SR(r)      (r + 0x00C)
 #define SSP_CPSR(r)    (r + 0x010)
 #define SSP_IMSC(r)    (r + 0x014)
 #define SSP_RIS(r)     (r + 0x018)
 #define SSP_MIS(r)     (r + 0x01C)
 #define SSP_ICR(r)     (r + 0x020)
 #define SSP_DMACR(r)   (r + 0x024)
 #define SSP_CSR(r)     (r + 0x030) /* vendor extension */
 #define SSP_ITCR(r)    (r + 0x080)
 #define SSP_ITIP(r)    (r + 0x084)
 #define SSP_ITOP(r)    (r + 0x088)
 #define SSP_TDR(r)     (r + 0x08C)
 
 #define SSP_PID0(r)    (r + 0xFE0)
 #define SSP_PID1(r)    (r + 0xFE4)
 #define SSP_PID2(r)    (r + 0xFE8)
 #define SSP_PID3(r)    (r + 0xFEC)
 
 #define SSP_CID0(r)    (r + 0xFF0)
 #define SSP_CID1(r)    (r + 0xFF4)
 #define SSP_CID2(r)    (r + 0xFF8)
 #define SSP_CID3(r)    (r + 0xFFC)
 
 /*
  * SSP Control Register 0  - SSP_CR0
  */
 #define SSP_CR0_MASK_DSS     (0x0FUL << 0)
 #define SSP_CR0_MASK_FRF     (0x3UL << 4)
 #define SSP_CR0_MASK_SPO     (0x1UL << 6)
 #define SSP_CR0_MASK_SPH     (0x1UL << 7)
 #define SSP_CR0_MASK_SCR     (0xFFUL << 8)
 #define SSP_CR0_BIT_MODE(x)  ((x) - 1)

 /* SSP Control Register 0  - SSP_CR0 */
#define SSP_CR0_SPO		(0x1 << 6)
#define SSP_CR0_SPH		(0x1 << 7)
#define SSP_SCR_MIN		(0x00)
#define SSP_SCR_MAX		(0xFF)
#define SSP_SCR_SHFT		8
#define DFLT_CLKRATE		2

 /*
  * The ST version of this block moves som bits
  * in SSP_CR0 and extends it to 32 bits
  */
 #define SSP_CR0_MASK_DSS_ST	(0x1FUL << 0)
 #define SSP_CR0_MASK_HALFDUP_ST	(0x1UL << 5)
 #define SSP_CR0_MASK_CSS_ST	(0x1FUL << 16)
 #define SSP_CR0_MASK_FRF_ST	(0x3UL << 21)
 
 /*
  * SSP Control Register 0  - SSP_CR1
  */
 #define SSP_CR1_MASK_LBM   (0x1UL << 0)
 #define SSP_CR1_MASK_SSE   (0x1UL << 1)
 #define SSP_CR1_MASK_MS    (0x1UL << 2)
 #define SSP_CR1_MASK_SOD   (0x1UL << 3)
 
 /*
  * The ST version of this block adds some bits
  * in SSP_CR1
  */
 #define SSP_CR1_MASK_RENDN_ST	(0x1UL << 4)
 #define SSP_CR1_MASK_TENDN_ST	(0x1UL << 5)
 #define SSP_CR1_MASK_MWAIT_ST	(0x1UL << 6)
 #define SSP_CR1_MASK_RXIFLSEL_ST (0x7UL << 7)
 #define SSP_CR1_MASK_TXIFLSEL_ST (0x7UL << 10)
 /* This one is only in the PL023 variant */
 #define SSP_CR1_MASK_FBCLKDEL_ST (0x7UL << 13)
 
 /*
  * SSP Status Register - SSP_SR
  */
 #define SSP_SR_MASK_TFE		(0x1UL << 0) /* Transmit FIFO empty */
 #define SSP_SR_MASK_TNF		(0x1UL << 1) /* Transmit FIFO not full */
 #define SSP_SR_MASK_RNE		(0x1UL << 2) /* Receive FIFO not empty */
 #define SSP_SR_MASK_RFF		(0x1UL << 3) /* Receive FIFO full */
 #define SSP_SR_MASK_BSY		(0x1UL << 4) /* Busy Flag */
 
 /*
  * SSP Clock Prescale Register  - SSP_CPSR
  */
 #define SSP_CPSR_MASK_CPSDVSR	(0xFFUL << 0)

#define SSP_CPSR_MIN		(0x02)
#define SSP_CPSR_MAX		(0xFE)
#define DFLT_PRESCALE		(0x40)
 
 /*
  * SSP Interrupt Mask Set/Clear Register - SSP_IMSC
  */
 #define SSP_IMSC_MASK_RORIM (0x1UL << 0) /* Receive Overrun Interrupt mask */
 #define SSP_IMSC_MASK_RTIM  (0x1UL << 1) /* Receive timeout Interrupt mask */
 #define SSP_IMSC_MASK_RXIM  (0x1UL << 2) /* Receive FIFO Interrupt mask */
 #define SSP_IMSC_MASK_TXIM  (0x1UL << 3) /* Transmit FIFO Interrupt mask */
 
 /*
  * SSP Raw Interrupt Status Register - SSP_RIS
  */
 /* Receive Overrun Raw Interrupt status */
 #define SSP_RIS_MASK_RORRIS		(0x1UL << 0)
 /* Receive Timeout Raw Interrupt status */
 #define SSP_RIS_MASK_RTRIS		(0x1UL << 1)
 /* Receive FIFO Raw Interrupt status */
 #define SSP_RIS_MASK_RXRIS		(0x1UL << 2)
 /* Transmit FIFO Raw Interrupt status */
 #define SSP_RIS_MASK_TXRIS		(0x1UL << 3)
 
 /*
  * SSP Masked Interrupt Status Register - SSP_MIS
  */
 /* Receive Overrun Masked Interrupt status */
 #define SSP_MIS_MASK_RORMIS		(0x1UL << 0)
 /* Receive Timeout Masked Interrupt status */
 #define SSP_MIS_MASK_RTMIS		(0x1UL << 1)
 /* Receive FIFO Masked Interrupt status */
 #define SSP_MIS_MASK_RXMIS		(0x1UL << 2)
 /* Transmit FIFO Masked Interrupt status */
 #define SSP_MIS_MASK_TXMIS		(0x1UL << 3)
 
 /*
  * SSP Interrupt Clear Register - SSP_ICR
  */
 /* Receive Overrun Raw Clear Interrupt bit */
 #define SSP_ICR_MASK_RORIC		(0x1UL << 0)
 /* Receive Timeout Clear Interrupt bit */
 #define SSP_ICR_MASK_RTIC		(0x1UL << 1)
 
 /*
  * SSP DMA Control Register - SSP_DMACR
  */
 /* Receive DMA Enable bit */
 #define SSP_DMACR_MASK_RXDMAE		(0x1UL << 0)
 /* Transmit DMA Enable bit */
 #define SSP_DMACR_MASK_TXDMAE		(0x1UL << 1)
 
 /*
  * SSP Chip Select Control Register - SSP_CSR
  * (vendor extension)
  */
 #define SSP_CSR_CSVALUE_MASK		(0x1FUL << 0)
 
 /*
  * SSP Integration Test control Register - SSP_ITCR
  */
 #define SSP_ITCR_MASK_ITEN		(0x1UL << 0)
 #define SSP_ITCR_MASK_TESTFIFO		(0x1UL << 1)
 
 /*
  * SSP Integration Test Input Register - SSP_ITIP
  */
 #define ITIP_MASK_SSPRXD		 (0x1UL << 0)
 #define ITIP_MASK_SSPFSSIN		 (0x1UL << 1)
 #define ITIP_MASK_SSPCLKIN		 (0x1UL << 2)
 #define ITIP_MASK_RXDMAC		 (0x1UL << 3)
 #define ITIP_MASK_TXDMAC		 (0x1UL << 4)
 #define ITIP_MASK_SSPTXDIN		 (0x1UL << 5)
 
 /*
  * SSP Integration Test output Register - SSP_ITOP
  */
 #define ITOP_MASK_SSPTXD		 (0x1UL << 0)
 #define ITOP_MASK_SSPFSSOUT		 (0x1UL << 1)
 #define ITOP_MASK_SSPCLKOUT		 (0x1UL << 2)
 #define ITOP_MASK_SSPOEn		 (0x1UL << 3)
 #define ITOP_MASK_SSPCTLOEn		 (0x1UL << 4)
 #define ITOP_MASK_RORINTR		 (0x1UL << 5)
 #define ITOP_MASK_RTINTR		 (0x1UL << 6)
 #define ITOP_MASK_RXINTR		 (0x1UL << 7)
 #define ITOP_MASK_TXINTR		 (0x1UL << 8)
 #define ITOP_MASK_INTR			 (0x1UL << 9)
 #define ITOP_MASK_RXDMABREQ		 (0x1UL << 10)
 #define ITOP_MASK_RXDMASREQ		 (0x1UL << 11)
 #define ITOP_MASK_TXDMABREQ		 (0x1UL << 12)
 #define ITOP_MASK_TXDMASREQ		 (0x1UL << 13)
 
 /*
  * SSP Test Data Register - SSP_TDR
  */
 #define TDR_MASK_TESTDATA		(0xFFFFFFFF)

 /*
 * ARM PL022 exists in different 'flavors'.
 * This drivers currently support the standard variant (0x00041022), that has a
 * 16bit wide and 8 locations deep TX/RX FIFO.
 */
static int pl022_is_supported(struct pl022_spi *dev) {
	/* PL022 version is 0x00041022 */
	if ( (REG16_LOAD(SSP_PID0(dev->base_addr)) == 0x22) &&
	    (REG16_LOAD(SSP_PID1(dev->base_addr)) == 0x10) &&
	    ((REG16_LOAD(SSP_PID2(dev->base_addr)) & 0xf) == 0x04) &&
	    (REG16_LOAD(SSP_PID3(dev->base_addr)) == 0x00)) {
			return 1;
	}

	return 0;
}

static void pl022_spi_flush(struct pl022_spi *dev) {
	do {
		while (REG16_LOAD(SSP_SR(dev->base_addr)) & SSP_SR_MASK_RNE) {
			REG16_LOAD(SSP_DR(dev->base_addr));
		}
	} while (REG16_LOAD(SSP_SR(dev->base_addr)) & SSP_SR_MASK_BSY);
}

static inline int pl022_spi_claim_bus(struct pl022_spi *dev) {
	uint16_t reg;

	/* Enable the SPI hardware */
	reg = REG16_LOAD(SSP_CR1(dev->base_addr));
	reg |= SSP_CR1_MASK_SSE;
	REG16_STORE(SSP_CR1(dev->base_addr), reg);

	pl022_spi_flush(dev);

	return 0;
}

static inline int pl022_spi_release_bus(struct pl022_spi *dev) {
	uint16_t reg;

	pl022_spi_flush(dev);

	/* Disable the SPI hardware */
	reg = REG16_LOAD(SSP_CR1(dev->base_addr));
	reg &= ~SSP_CR1_MASK_SSE;
	REG16_STORE(SSP_CR1(dev->base_addr), reg);

	return 0;
}

static int pl022_spi_setup(struct pl022_spi *dev, bool is_master) {
	uint16_t reg;

	reg = REG16_LOAD(SSP_CR1(dev->base_addr));

	if (!is_master) {
		reg |= SSP_CR1_MASK_MS;
	} else {
		reg &= ~((uint16_t)SSP_CR1_MASK_MS);
	}

	/* 8 bits per word, high polarity and default clock rate */
	REG16_STORE(SSP_CR0(dev->base_addr), SSP_CR0_BIT_MODE(8));
	REG16_STORE(SSP_CPSR(dev->base_addr), DFLT_PRESCALE);
	REG16_STORE(SSP_CR1(dev->base_addr), reg);

	return 0;
}

int pl022_spi_init(struct pl022_spi *dev) {
	/* Check the PL022 version */
	if (!pl022_is_supported(dev)) {
		return -ENOTSUP;
	}

	return pl022_spi_setup(dev, true);
}

static int pl022_spi_select(struct spi_controller *spi_dev, int cs) {
	log_debug("NIY");

	return 0;
}

static int pl022_spi_set_mode(struct spi_controller *spi_dev, bool is_master) {
	struct pl022_spi *dev = spi_dev->spic_priv;
	uint16_t reg;

	reg = REG16_LOAD(SSP_CR0(dev->base_addr));
	reg &= ~(SSP_CR0_SPH | SSP_CR0_SPO);
	if (spi_dev->flags & SPI_CS_CPHA) {
		reg |= SSP_CR0_SPH;
	}
	if (spi_dev->flags & SPI_CS_CPOL) {
		reg |= SSP_CR0_SPO;
	}
	REG16_STORE(SSP_CR0(dev->base_addr), reg);

	return pl022_spi_setup(dev, is_master);
}

static int pl022_spi_transfer(struct spi_controller *spi_dev, uint8_t *inbuf,
		uint8_t *outbuf, int count) {
	struct pl022_spi *dev = spi_dev->spic_priv;
	uint8_t value;
	int tx_cnt;
	int rx_cnt;

	rx_cnt = 0;
	tx_cnt = 0;

	if (spi_dev->flags & SPI_CS_ACTIVE && spi_dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we lower the same pin all the tiem */
	}

	pl022_spi_claim_bus(dev);

	/* transmit/recieve */
	while (tx_cnt < count) {
		if (REG16_LOAD(SSP_SR(dev->base_addr)) & SSP_SR_MASK_TNF) {
			if (inbuf) {
				value = *inbuf++;
			} else {
				value = 0;
			}

			REG16_STORE(SSP_DR(dev->base_addr), value);
			tx_cnt++;
		}

		if (REG16_LOAD(SSP_SR(dev->base_addr)) & SSP_SR_MASK_RNE) {
			value = REG16_LOAD(SSP_DR(dev->base_addr));
			if (outbuf) {
				*outbuf++ = value;
			}
			rx_cnt++;
		}
	}

	while (rx_cnt < tx_cnt) {
		if (REG16_LOAD(SSP_SR(dev->base_addr)) & SSP_SR_MASK_RNE) {
			value = REG16_LOAD(SSP_DR(dev->base_addr));
			if (outbuf) {
				*outbuf++ = value;
			}
			rx_cnt++;
		}
	}

	pl022_spi_release_bus(dev);

	if (spi_dev->flags & SPI_CS_INACTIVE && spi_dev->is_master) {
		/* Note: we suppose that there's a single slave device
		 * on the SPI bus, so we raise the same pin all the tiem */
	}

	return 0;
}

struct spi_controller_ops pl022_spic_ops = {
	.select   = pl022_spi_select,
	.set_mode = pl022_spi_set_mode,
	.transfer = pl022_spi_transfer
};
