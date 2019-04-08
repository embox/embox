/**
 * @file
 * @brief Implements serial driver for at91 controller
 *
 * @date 07.06.10
 * @author Anton Kozlov
 */

#include <embox/unit.h>
#include <hal/reg.h>
#include <drivers/at91sam7s256.h>
#include <hal/system.h>
#include <drivers/diag.h>

/* Baudrate=SYS_CLOCK/(8(2-Over)CD) = MCK/16CD = 18432000/(16*30) = 38400
 * CD = SYS_CLOCK / (16 * UART_BAUD_RATE)
 */
#define BAUD_RATE \
	OPTION_GET(NUMBER, baud_rate)

#define UART_CLOCK_DIVIDER \
	(SYS_CLOCK / (BAUD_RATE * 16))

#define TTGR_DISABLE 0

static int at91uart_diag_init(const struct diag *diag) {
	/* Disabling controling PA5 and PA6 by PIO */
	REG_STORE(AT91C_PIOA_PDR, AT91C_PA5_RXD0 | AT91C_PA6_TXD0);
	/* Selecting control by USART controller */
	REG_STORE(AT91C_PIOA_ASR, AT91C_PA5_RXD0 | AT91C_PA6_TXD0);
	/*resetting & disabling RX, TX */
	REG_STORE(AT91C_US0_CR, AT91C_US_RXDIS | AT91C_US_TXDIS | \
					AT91C_US_RSTRX | AT91C_US_RSTTX);
	/* was 0x8c0, now there is Even Parity */
	//REG_STORE(AT91C_US0_MR, AT91C_US_CHRL_8_BITS | AT91C_US_CLKS_CLOCK);
	REG_STORE(AT91C_US0_BRGR, UART_CLOCK_DIVIDER);
	REG_STORE(AT91C_US0_MR,  AT91C_US_PAR_NONE | \
			AT91C_US_CHRL_8_BITS | AT91C_US_CLKS_CLOCK);
	/* enabling clocking USART */
	REG_STORE(AT91C_PMC_PCER, 1 << AT91C_ID_US0);
	/* enabling RX, TX */
	REG_STORE(AT91C_US0_CR, AT91C_US_RXEN | AT91C_US_TXEN);
	return 0;
}

static int at91uart_diag_hasrx(const struct diag *diag) {
	return (AT91C_US_RXRDY & REG_LOAD(AT91C_US0_CSR));
}

static char at91uart_diag_getc(const struct diag *diag) {
	return (char) REG_LOAD(AT91C_US0_RHR);
}

static void at91uart_diag_putc(const struct diag *diag, char ch) {
	while (!(AT91C_US_TXRDY & REG_LOAD(AT91C_US0_CSR))) {
	}
	REG_STORE(AT91C_US0_THR, (unsigned long) ch);
}

DIAG_OPS_DEF(
		.init = at91uart_diag_init,
		.putc = at91uart_diag_putc,
		.getc = at91uart_diag_getc,
		.kbhit = at91uart_diag_hasrx,
);
