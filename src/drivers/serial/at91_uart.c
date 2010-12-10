/**
 * @file
 * @brief Implements diag and serial driver for at91 controller
 *
 * @date 7.06.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <kernel/diag.h>
#include <kernel/irq.h>
#include <hal/reg.h>

//TODO generalize to use with both US0 and US1

#include <drivers/at91sam7s256.h>

int uart_init(void) {
	diag_init();
	return 0;
}

void uart_putc(char ch) {
	diag_putc(ch);
}

char uart_getc(void) {
	return diag_getc();
}

int uart_set_irq_handler(irq_handler_t pfunc) {
	return 0;
}

int uart_remove_irq_handler(void) {
	return 0;
}

/*implement diag interface*/

#define TTGR_DISABLE 0

/* Baudrate=SYS_CLOCK/(8(2-Over)CD) = MCK/16CD = 18432000/(16*30) = 38400
 * CD = SYS_CLOCK / (16 * UART_BAUD_RATE)
 */
#define UART_CLOCK_DIVIDER (CONFIG_SYS_CLOCK / (16 * CONFIG_UART_BAUD_RATE))

void diag_init(void) {
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
}

char diag_getc(void) {
	while (!diag_has_symbol()) {
	}
	return (char) REG_LOAD(AT91C_US0_RHR);
}

int diag_has_symbol(void) {
	return (AT91C_US_RXRDY & REG_LOAD(AT91C_US0_CSR));
}

void diag_putc(char ch) {
	while (!(AT91C_US_TXRDY & REG_LOAD(AT91C_US0_CSR))) {
	//while (!line_is_accepts_new_char)
	}
	REG_STORE(AT91C_US0_THR, (unsigned long) ch);
}
