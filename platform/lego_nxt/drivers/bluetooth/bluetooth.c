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
#include <unistd.h>

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

volatile enum {SIZE_READ, COMM_READ, UART_MODE} bt_us_state;

volatile void bt_set_uart_state(void) {
	bt_us_state = UART_MODE;
}

#define BUFF_SIZE 256
//#define DEBUG
uint8_t bt_buff[BUFF_SIZE];

static int bt_buff_pos = 0;

static void bt_us_read_handle(void) {
	int msg_len = bt_buff[bt_buff_pos];

	switch (bt_us_state) {
		case SIZE_READ:
#ifdef DEBUG
			TRACE("R%x", msg_len);
#endif
			if (msg_len != 0) {
				bt_us_state = COMM_READ;
				if (bt_buff_pos + 1 + msg_len >= BUFF_SIZE) {
					bt_buff[0] = msg_len;
					bt_buff_pos = 0;
				}
				nxt_bluetooth_read(bt_buff + bt_buff_pos + 1, msg_len);
			}
			break;
		case COMM_READ:
			bt_us_state = SIZE_READ;

#ifdef DEBUG
			TRACE("$");
			for (int i = 0; i <= msg_len; i++) {
				TRACE("%x:", *(bt_buff + bt_buff_pos + i));
			}
			TRACE("$");
#endif
			bt_handle(bt_buff + bt_buff_pos);

			bt_buff_pos += 1 + msg_len;
			if (bt_buff_pos >= BUFF_SIZE) {
				bt_buff_pos = 0;
			}
			nxt_bluetooth_read(bt_buff + bt_buff_pos, 1);
			break;
		case UART_MODE:
//#ifdef DEBUG
			TRACE("%c", bt_buff[0]);
//#endif
			nxt_bluetooth_read(bt_buff, 1);
			break;
		default:
			break;
	}
}

void bt_uart_putc(char c) {
	if (bt_us_state != UART_MODE) {
		return;
	}
	/* here might be beaty buffered write policy*/
	/* instead */
	while (!(REG_LOAD(AT91C_US1_CSR) & AT91C_US_ENDTX)) {

	}
	nxt_bluetooth_write(&c, 1);
}

volatile int bt_uart_inited = 0;

volatile void bt_us_receive_init(void) {
	REG_STORE(AT91C_US1_IDR, AT91C_US_ENDTX);
	bt_uart_inited = 1;
	int delay = 3000;
	while (delay--);
	bt_set_arm7_cmd();
	delay = 3000;
	while (delay--);

	/*doing last steps for init*/
	bt_buff_pos = 0;

	nxt_bluetooth_read(bt_buff, 1);

}

irq_return_t nxt_bt_us_handler(int irq_num, void *dev_id) {
	uint32_t us_state = REG_LOAD(AT91C_US1_CSR);
	if (us_state & AT91C_US_ENDRX) {
		bt_us_read_handle();
	}
	if (!bt_uart_inited && (us_state & AT91C_US_ENDTX) && (bt_us_state == UART_MODE)) {
		bt_us_receive_init();
	}
	return IRQ_HANDLED;
}

static int nxt_bluetooth_init(void) {
	volatile uint8_t tmp;
	/* Configure the usart */
	REG_STORE(AT91C_PMC_PCER, (1 << AT91C_ID_US1));

	REG_STORE(AT91C_PIOA_PDR, NXT_BT_RX_PIN | NXT_BT_TX_PIN | NXT_BT_SCK_PIN | NXT_BT_RTS_PIN
			| NXT_BT_CTS_PIN);
	REG_STORE(AT91C_PIOA_ASR, NXT_BT_RX_PIN | NXT_BT_TX_PIN | NXT_BT_SCK_PIN | NXT_BT_RTS_PIN
			| NXT_BT_CTS_PIN);

	REG_STORE(AT91C_US1_PTCR, (AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS));
//	REG_STORE(AT91C_US1_CR, AT91C_US_RSTSTA | AT91C_US_RXDIS | AT91C_US_TXDIS);
	REG_STORE(AT91C_US1_CR, AT91C_US_RSTRX | AT91C_US_RSTTX);
	REG_STORE(AT91C_US1_CR, AT91C_US_STTTO);
	REG_STORE(AT91C_US1_RTOR, 10000);
//	REG_STORE(AT91C_US1_IDR, AT91C_US_TIMEOUT);
	REG_STORE(AT91C_US1_MR, (AT91C_US_USMODE_HWHSH & ~AT91C_US_SYNC)
			| AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE
			| AT91C_US_NBSTOP_1_BIT | AT91C_US_OVER);
	REG_STORE(AT91C_US1_IDR, ~0);
    REG_STORE(AT91C_US1_IER, AT91C_US_ENDRX);
//  REG_STORE(AT91C_US1_IER, AT91C_US_ENDRX | AT91C_US_ENDTX);
	/*
	REG_STORE(AT91C_US1_MR, (AT91C_US_USMODE_HWHSH & ~AT91C_US_SYNC) |
			AT91C_US_NBSTOP_1_BIT |
			(AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE));
	*/
	/*
	REG_STORE(AT91C_US1_BRGR, ((CONFIG_SYS_CLOCK / 8 / NXT_BT_BAUD_RATE)
			| (((CONFIG_SYS_CLOCK / 8) - ((CONFIG_SYS_CLOCK / 8 / NXT_BT_BAUD_RATE)
					* NXT_BT_BAUD_RATE)) / ((NXT_BT_BAUD_RATE + 4) / 8)) << 16));
	*/
	REG_STORE(AT91C_US1_BRGR, CONFIG_SYS_CLOCK / (8 * NXT_BT_BAUD_RATE));
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
	pin_set_output(NXT_BT_CS_PIN | NXT_BT_RST_PIN);
	pin_clear_output(NXT_BT_CMD_PIN);

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

	usleep(2000);

	bt_set_reset_high();

	usleep(5000);

	bt_us_state = SIZE_READ;
	nxt_bluetooth_read(bt_buff, 1);

	irq_attach((irq_nr_t) AT91C_ID_US1,
		(irq_handler_t) &nxt_bt_us_handler, 0, NULL, "nxt bt reader");

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
	sum += header->length + 3; //is it need?
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
size_t nxt_bluetooth_write(uint8_t *buff, size_t len) {
	REG_STORE(AT91C_US1_TPR, buff);
	REG_STORE(AT91C_US1_TCR, len);
	return len;
}

size_t nxt_bluetooth_read(uint8_t *buff, size_t len) {
	REG_STORE(AT91C_US1_RPR, buff);
	REG_STORE(AT91C_US1_RCR, len);

	return 0;
}
