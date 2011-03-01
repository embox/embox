/**
 * @file
 *
 * @brief Implements Lego NXT Bluetooth driver.
 *
 * @date 24.02.2011
 * @author Anton Bondarev
 */
#include <types.h>
#include <embox/unit.h>
#include <hal/reg.h>

#include <drivers/at91sam7s256.h>
#include <drivers/pins.h>
#include <drivers/bluetooth.h>

#define   NXT_BT_RX_PIN  AT91C_PIO_PA21
#define   NXT_BT_TX_PIN  AT91C_PIO_PA22
#define   NXT_BT_SCK_PIN AT91C_PIO_PA23
#define   NXT_BT_RTS_PIN AT91C_PIO_PA24
#define   NXT_BT_CTS_PIN AT91C_PIO_PA25


#define NXT_BT_CS_PIN     AT91C_PIO_PA31
#define NXT_BT_RST_PIN    AT91C_PIO_PA11
#define NXT_BT_CMD_PIN    AT91C_PIO_PA27

#define NXT_BT_ADC_RATE 50000
#define NXT_BT_BAUD_RATE 460800

EMBOX_UNIT_INIT(nxt_bluetooth_init);
///////////////////////////////////////////////////////////////////
int us1_has_symbol(void) {
	return (AT91C_US_RXRDY & REG_LOAD(AT91C_US1_CSR));
}

char us1_getc(void) {
	while (!us1_has_symbol()) {
	}
	return (char) REG_LOAD(AT91C_US1_RHR);
}

void us1_putc(char ch) {
	while (!(AT91C_US_TXRDY & REG_LOAD(AT91C_US1_CSR))) {
	//while (!line_is_accepts_new_char)
	}
	REG_STORE(AT91C_US1_THR, (unsigned long) ch);
}
///////////////////////////////////////////////////////////////////
void bt_clear_arm7_cmd(void)
{
	REG_STORE(AT91C_PIOA_CODR, NXT_BT_CMD_PIN);
}

void bt_set_arm7_cmd(void)
{
	REG_STORE(AT91C_PIOA_SODR, NXT_BT_CMD_PIN);
}

void bt_set_reset_high(void)
{
	REG_STORE(AT91C_PIOA_SODR, NXT_BT_RST_PIN);
}

void bt_set_reset_low(void)
{
  REG_STORE(AT91C_PIOA_CODR, NXT_BT_RST_PIN);
}
///////////////////////////////////////////////////////////////////////

static int nxt_bluetooth_init(void) {
	volatile uint8_t tmp;
	/* Configure the usart */
	REG_STORE(AT91C_PMC_PCER, (1 << AT91C_ID_US1));

	REG_STORE(AT91C_PIOA_PDR, NXT_BT_RX_PIN | NXT_BT_TX_PIN | NXT_BT_SCK_PIN | NXT_BT_RTS_PIN
			| NXT_BT_CTS_PIN);
	REG_STORE(AT91C_PIOA_ASR, NXT_BT_RX_PIN | NXT_BT_TX_PIN | NXT_BT_SCK_PIN | NXT_BT_RTS_PIN
			| NXT_BT_CTS_PIN);

	REG_STORE(AT91C_US1_CR, AT91C_US_RSTSTA | AT91C_US_RXDIS | AT91C_US_TXDIS);
	//REG_STORE(AT91C_US1_CR, AT91C_US_RSTSTA);
	REG_STORE(AT91C_US1_CR, AT91C_US_STTTO);
	REG_STORE(AT91C_US1_RTOR, 10000);
	REG_STORE(AT91C_US1_IDR, AT91C_US_TIMEOUT);
	REG_STORE(AT91C_US1_MR, (AT91C_US_USMODE_HWHSH & ~AT91C_US_SYNC)
			| AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE
			| AT91C_US_NBSTOP_1_BIT | AT91C_US_OVER);
	REG_STORE(AT91C_US1_BRGR, ((CONFIG_SYS_CLOCK / 8 / NXT_BT_BAUD_RATE)
			| (((CONFIG_SYS_CLOCK / 8) - ((CONFIG_SYS_CLOCK / 8 / NXT_BT_BAUD_RATE)
					* NXT_BT_BAUD_RATE)) / ((NXT_BT_BAUD_RATE + 4) / 8)) << 16));
	REG_STORE(AT91C_US1_PTCR, (AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS));
	REG_STORE(AT91C_US1_RCR, 0);
	REG_STORE(AT91C_US1_TCR, 0);
	REG_STORE(AT91C_US1_RNPR, 0);
	REG_STORE(AT91C_US1_TNPR, 0);
	tmp = REG_LOAD(AT91C_US1_RHR);
	tmp = REG_LOAD(AT91C_US1_CSR);

	REG_STORE(AT91C_US1_CR, AT91C_US_RXEN | AT91C_US_TXEN);
	REG_STORE(AT91C_US1_PTCR, AT91C_PDC_RXTEN | AT91C_PDC_TXTEN);

	/*configure control pins*/
	REG_STORE(AT91C_PIOA_PPUDR, NXT_BT_CMD_PIN);
	pin_config_output(NXT_BT_CS_PIN | NXT_BT_RST_PIN | NXT_BT_CMD_PIN);
	pin_set_output(NXT_BT_CS_PIN);
	pin_clear_output(NXT_BT_CMD_PIN | NXT_BT_RST_PIN);

#if 0
	/*configure timer*/
	REG_STORE(AT91C_PMC_PCER, (1 << AT91C_ID_TC1));
	REG_STORE(AT91C_TC1_CCR, AT91C_TC_CLKDIS);
	REG_STORE(AT91C_TC1_IDR, ~0);
	/* MCLK/2, wave mode 10 */
	REG_STORE(AT91C_TC1_CMR, AT91C_TC_WAVE | AT91C_TC_WAVESEL_UP_AUTO
			| AT91C_TC_ACPA_SET | AT91C_TC_ACPC_CLEAR | AT91C_TC_ASWTRG_SET);

	REG_STORE(AT91C_TC1_RC, (CONFIG_SYS_CLOCK / 2) / (NXT_BT_ADC_RATE));
	REG_STORE(AT91C_TC1_RA, ((CONFIG_SYS_CLOCK / 2) / (NXT_BT_ADC_RATE)) / 2);
	REG_STORE(AT91C_TC1_CCR, AT91C_TC_CLKEN);
	REG_STORE(AT91C_TC1_CCR, AT91C_TC_SWTRG);
#endif
	/* Configure the ADC */
#if 1
	bt_clear_arm7_cmd();
	bt_set_reset_low();
	usleep(100);
	while (us1_has_symbol()) {
		us1_getc();
	}
	bt_set_reset_high();

	usleep(5000);
	while (us1_has_symbol()) {
		us1_getc();
	}
	bt_clear_arm7_cmd();
#endif
	return 0;
}


//////////////////////////////////////////////////////////////////////

int bt_wrap(bt_message_t *header, uint8_t *buffer) {
	int i;
	uint16_t sum = 0;
	*buffer++ = header->length + 3;
	*buffer++ = header->type;
	sum += header->type;
	for (i = 0; i < header->length; i++) {
		*buffer++ = header->content[i];
		sum += header->content[i];
	}
	sum = ~sum + 1;
	*buffer++ = sum >> 8;
	*buffer++ = sum & 0xff;
	return 1 + header->length + 3;
}

void bt_unwrap(bt_message_t *header, uint8_t *buffer) {
	int i;
	header->length = *buffer++;
	header->length -= 3;

	header->type = *buffer++;
	for (i = 0; i < header->length; i++) {
		header->content[i] = *buffer++;
	}
	header->sum = 0;
}

///////////////////////////////////////////////////////////////////////
#define BT_DMA_COMM

size_t nxt_bluetooth_write(uint8_t *buff, size_t len) {
#ifndef BT_DMA_COMM
	int i = len;
	uint8_t *cur_buff = buff;
	while (i --) {
		us1_putc(*cur_buff++);
	}
#else
	REG_STORE(AT91C_US1_TPR, buff);
	REG_STORE(AT91C_US1_TCR, len);
	usleep(1000);
#endif
	return len;
}

size_t nxt_bluetooth_read(uint8_t *buff, size_t len) {
#ifndef BT_DMA_COMM
	int i = len;
	uint8_t *cur_buff = buff;
	uint8_t length = us1_getc();
	*cur_buff++ = length;
	while (length-- && i--) {
		*cur_buff++ = us1_getc();
		TRACE("0x%x\n", *(cur_buff - 1));
	}
	//swap last 2 bytes
#else
	REG_STORE(AT91C_US1_RPR, buff);
	REG_STORE(AT91C_US1_RCR, len);
	usleep(1000);
#endif

	return 0;
}
