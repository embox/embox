/**
 * @file at91_diag_uart.c
 * @brief at91 diag uart
 *
 * @date 7.06.2010
 * @author Anton Kozlov
 */

#include <types.h>

#include <kernel/diag.h>
#include <hal/reg.h>

//TODO generalize to use with both US0 and US1

#include <drivers/at91sam7s256.h>

#define TTGR_DISABLE 0

void diag_init(void) {
	REG_STORE(AT91C_PIOA_PDR, AT91C_PA5_RXD0 | AT91C_PA6_TXD0); /* Disabling controling PA5 and PA6 by PIO */
	REG_STORE(AT91C_PIOA_ASR, AT91C_PA5_RXD0 | AT91C_PA6_TXD0); /* Selecting control by USART controller */
	REG_STORE(AT91C_US0_CR, 0xac); /*resetting & disabling RX, TX */
	REG_STORE(AT91C_US0_BRGR, 313); // for 9600 //XXX will be changed since we initialize MCK
	//REG_STORE(AT91C_US0_MR, AT91C_US_CHRL_8_BITS ); //was 0x8c0, now there is Even Parity
	REG_STORE(AT91C_US0_MR, AT91C_US_PAR_NONE | AT91C_US_CHRL_8_BITS );
	REG_STORE(AT91C_PMC_PCER, 1 << AT91C_ID_US0); //enabling clocking USART
	//REG_STORE(AT91C_US0_TTGR, TTGR_DISABLE); //? =)
	//REG_STORE(PDC_PTCR, 11);
	REG_STORE(AT91C_US0_CR, 0x50); /*enabling RX, TX */
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
	while (!(AT91C_US_TXRDY & REG_LOAD(AT91C_US0_CSR))) {  //while (!line_is_accepts_new_char)
	}
	REG_STORE(AT91C_US0_THR, (unsigned long) ch);
}
