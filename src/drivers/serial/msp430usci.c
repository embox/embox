/**
 * @file
 * @brief MSP430's universal serial communication interface, uart mode
 *
 * @author  Anton Kozlov
 * @date    02.08.2013
 */

#include <hal/reg.h>
#include <drivers/diag.h>

#define USCI_BASE 	0x60
#define CTL0 		(USCI_BASE + 0)
#define CTL1 		(USCI_BASE + 1)
#define BR0  		(USCI_BASE + 2)
#define BR1  		(USCI_BASE + 3)
#define MCTL 		(USCI_BASE + 4)
#define STAT 		(USCI_BASE + 5)
#define RXB  		(USCI_BASE + 6)
#define TXB  		(USCI_BASE + 7)
#define ABCTL  		0x5d
#define IE2    		0x1
#define IFG2   		0x3

#define CTL1_RST	(1 << 0)
#define CTL1_SMCLK	(2 << 6)

#define MCTL_BRF_OFF 	4
#define MCTL_BRS_OFF 	1
#define MCTL_UCOS16O 	0

#define RXIFG		(1 << 0)
#define TXIFG 		(1 << 1)

#define P1_IN	    	0x20
#define P1_OUT	    	0x21
#define P1_PORTSEL1 	0x26
#define P1_PORTSEL2 	0x41

#define UARTRXD 	(1 << 1)
#define UARTTXD 	(1 << 2)

/* below is for 9660 from official doc */
#define PAR_BR 		1666
#define PAR_BRS		6
#define PAR_BRF 	0
#define PAR_UCO		0

static int msp430usci_diag_init(const struct diag *diag) {

	/*reset uart, select clock*/
	REG_STORE(CTL1, 0x01);
	REG_ORIN(CTL1, CTL1_SMCLK);

	/* select 8-1-no_parity, manual baudrate.
 	 * This is defaults, just to be sure */
	REG_STORE(CTL0, 0);
	REG_STORE(ABCTL, 0);

	/* select baudrate */
	REG_STORE(BR0, PAR_BR & 0xff);
	REG_STORE(BR1, PAR_BR >> 8);
	REG_STORE(MCTL, (PAR_BRF << MCTL_BRF_OFF) |
		        (PAR_BRS << MCTL_BRS_OFF) |
			(PAR_UCO << MCTL_UCOS16O));

	/*configure i/o pins */
	REG_ORIN(P1_PORTSEL1, UARTRXD);
	REG_ORIN(P1_PORTSEL2, UARTRXD);
	REG_ORIN(P1_IN, UARTRXD);

	REG_ORIN(P1_PORTSEL1, UARTTXD);
	REG_ORIN(P1_PORTSEL2, UARTTXD);
	REG_ORIN(P1_OUT, UARTTXD);

	/* release the reset */
	REG_ANDIN(CTL1, ~1);

	return 0;
}

static void msp430usci_diag_putc(const struct diag *diag, char ch) {

	REG_STORE(TXB, ch);

	while (!(REG_LOAD(IFG2) & TXIFG));
}

static char msp430usci_diag_getc(const struct diag *diag) {
	return REG_LOAD(RXB);
}

static int msp430usci_diag_kbhit(const struct diag *diag) {
	return (REG_LOAD(IFG2) & RXIFG);
}

DIAG_OPS_DEF(
		.init = msp430usci_diag_init,
		.putc = msp430usci_diag_putc,
		.getc = msp430usci_diag_getc,
		.kbhit = msp430usci_diag_kbhit,
);
