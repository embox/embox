/**
 * @file
 *
 * @date 21.02.13
 * @author Pavel Cherstvov
 */

#include <embox/unit.h>
#include <sys/types.h>
#include <stdio.h>
#include <hal/reg.h>
//#include <kernel/irq.h> 		/* IRQ is coming soon */
//#include <drivers/irqctrl.h> 	/* IRQ is coming soon */
#include <drivers/dm37xx_mux.h>
#include "omap3_spi_poll.h"


EMBOX_UNIT_INIT(omap3_spi_poll_init);


int spi_poll_switch_master_mode(void) {
	REG_ORIN(MCSPI1_SYSCONFIG, MCSPI_SYSCONFIG_SOFTRESET);
	while (!(REG_LOAD(MCSPI1_SYSSTATUS) & MCSPI_SYSSTATUS_RESETDONE)) {
	}
	REG_CLEAR_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 18); /* IS, somi reception*/
	REG_SET_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 16); /* DPE0, no trans on somi */
	REG_CLEAR_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 17); /* DPE1 1, transmission on simo */
	REG_CLEAR_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 12); /* TRM, transmit and receive mode */
	REG_CLEAR_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 13); /* TRM, transmit and receive mode */
	REG_ORIN(MCSPI1_CHxCONF(SPI_CHANNEL_NR), ( 0x7 << 7)); /* WL, word length 8 bits*/
	REG_SET_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 6); /* EPOL, cs active polarity low */
	REG_ORIN(MCSPI1_CHxCONF(SPI_CHANNEL_NR), ( 0x8 << 2)); /* CLKD, divider 256 (~187 kHz) */
	REG_CLEAR_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 1); /* POL, 0 */
	REG_CLEAR_BIT(MCSPI1_CHxCONF(SPI_CHANNEL_NR), 0); /* PHA, 0 */
	REG_CLEAR_BIT(MCSPI1_MODULCTRL, 2); /* MS, Master mode */
	REG_SET_BIT(MCSPI1_CHxCTRL(SPI_CHANNEL_NR), 0); /* EN, channel SPI_CHANNEL_NR enabled */
	return 0;
}

static int omap3_spi_poll_init(void) {
	REG_ORIN(CM_FCLKEN1_CORE, (1 << 18));
	REG_ORIN(CM_ICLKEN1_CORE, (1 << 18));

	MUX_VAL(CONTROL_PADCONF_MCSPI1_CLK, (IEN | PD | M0 )); /* mcspi1_clk */
	MUX_VAL(CONTROL_PADCONF_MCSPI1_CS0, (IEN | PD | M0 )); /* mcspi1_cs0 */
	MUX_VAL(CONTROL_PADCONF_MCSPI1_SIMO, (IEN | PD | M0 )); /* mcspi1_simo */
	MUX_VAL(CONTROL_PADCONF_MCSPI1_SOMI, (IEN | PD | M0 )); /* mcspi1_somi */

	REG_SET_BIT(CM_ICLKEN_PER,17);
	REG_SET_BIT(CM_FCLKEN_PER,17);

	REG_ORIN(GPIO6_SYSCONFIG, GPIO6_SYSCONFIG_SOFTRESET);
	while (!(REG_LOAD(GPIO6_SYSSTATUS) & GPIO6_SYSSTATUS_RESETDONE)) {
	}

	MUX_VAL(CONTROL_PADCONF_MCBSP1_CLKX, (IEN | PU | M4 )); /* gpio_162 */
	REG_CLEAR_BIT(GPIO6_OE,2);
	REG_SET_BIT(GPIO6_CLEARDATAOUT,2); /* switch on-module chip select muxing to CS1 */
	return 0;
}

#if 0
/* IRQ is coming soon */
static irq_return_t my_isr(unsigned int my_irq_nr, void *my_dev_id) {
	return IRQ_HANDLED;
}
#endif

int spi_poll_send(const char *outdata, __u8 *indata, __u32 nBytes) {
	REG_ORIN(MCSPI1_IRQSTATUS, MCSPI_IRQSTATUS_TXEMPTY(SPI_CHANNEL_NR));
	REG_ORIN(MCSPI1_IRQSTATUS, MCSPI_IRQSTATUS_RXFULL(SPI_CHANNEL_NR));
	while (nBytes) {
		REG_STORE(MCSPI1_TX(SPI_CHANNEL_NR), *outdata);
		while (!(REG_LOAD(MCSPI1_IRQSTATUS) & MCSPI_IRQSTATUS_TXEMPTY(SPI_CHANNEL_NR))) {
				}
		while (!(REG_LOAD(MCSPI1_IRQSTATUS) & MCSPI_IRQSTATUS_RXFULL(SPI_CHANNEL_NR))) {
				}
		*indata = REG_LOAD(MCSPI1_RX(SPI_CHANNEL_NR));
		REG_ORIN(MCSPI1_IRQSTATUS, MCSPI_IRQSTATUS_RXFULL(SPI_CHANNEL_NR));
		REG_ORIN(MCSPI1_IRQSTATUS, MCSPI_IRQSTATUS_TXEMPTY(SPI_CHANNEL_NR));
		nBytes--;
		outdata++;
		indata++;
	}
	return 0;
}
