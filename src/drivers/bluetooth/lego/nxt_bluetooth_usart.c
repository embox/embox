/**
 * @file
 * @brief Implements Lego NXT Bluetooth driver.
 *
 * @date 24.02.11
 * @author Anton Bondarev
 */

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <hal/system.h>
#include <kernel/time/ktime.h>
#include <drivers/at91sam7s256.h>
#include <drivers/pins.h>
#include <drivers/bluetooth/bluetooth.h>
#include <drivers/bluetooth/lego/blue_core4.h>
#include <kernel/time/timer.h>

#include <embox/unit.h>


#include <pnet/core/core.h>
#include <pnet/core/repo.h>
#include <pnet/pack/pnet_pack.h>
#include <pnet/pack/pack_alone.h>

#define RX_PIN  ((uint32_t) (1 << OPTION_GET(NUMBER,rx_pin)))
#define TX_PIN  ((uint32_t) (1 << OPTION_GET(NUMBER,tx_pin)))
#define SCK_PIN ((uint32_t) (1 << OPTION_GET(NUMBER,sck_pin)))
#define RTS_PIN ((uint32_t) (1 << OPTION_GET(NUMBER,rts_pin)))
#define CTS_PIN ((uint32_t) (1 << OPTION_GET(NUMBER,cts_pin)))

#define CMD_PIN ((uint32_t) (1 << OPTION_GET(NUMBER,cmd_pin)))
#define RST_PIN ((uint32_t) (1 << OPTION_GET(NUMBER,rst_pin)))

#define CS_PIN    ((uint32_t) (1 << OPTION_GET(NUMBER,cs_pin)))

static volatile AT91PS_USART us_dev_regs = ((AT91PS_USART) OPTION_GET(NUMBER,serial_port_offset));

#define NXT_BT_ADC_RATE 50000
#define NXT_BT_BAUD_RATE 460800

EMBOX_UNIT_INIT(nxt_bluetooth_init);

PNET_NODE_DEF_NAME(BLUETOOTH_HW_BLUE_CORE4_DATA, this_data, {});
PNET_NODE_DEF_NAME(BLUETOOTH_HW_BLUE_CORE4_CTRL, this_ctrl, {});

static struct pnet_pack *data_pack;

void bluetooth_hw_accept_connect(void) {
	pin_set_output(CMD_PIN);
}

void bluetooth_hw_soft_reset(void) {
	pin_clear_output(CMD_PIN);
}

static irq_return_t nxt_bt_us_handler(unsigned int irq_nr, void *data) {
	if (!data_pack) {
		return IRQ_HANDLED;
	}
	if (REG_LOAD(&(us_dev_regs->US_CSR)) & AT91C_US_ENDTX) {
		struct pnet_pack *_pack = data_pack;
		data_pack = NULL;
		if (0 != netif_rx(_pack)) {
			pnet_pack_destroy(_pack);
		}
	}

	return IRQ_HANDLED;
}

size_t bluetooth_write(uint8_t *buff, size_t len) {
	REG_STORE(&(us_dev_regs->US_TPR), (uint32_t) buff);
	REG_STORE(&(us_dev_regs->US_TCR), len);
	return len;
}

size_t bluetooth_read(size_t len) {
	if (data_pack) {
		pnet_pack_destroy(data_pack);
	}
	data_pack = pnet_pack_create(NULL, len, PNET_PACK_TYPE_SINGLE);

	assert(data_pack);

	data_pack->node = &this_data;

	REG_STORE(&(us_dev_regs->US_RPR), (uint32_t) pnet_pack_get_data(data_pack));
	REG_STORE(&(us_dev_regs->US_RCR), len);

	return 0;
}

static void init_usart(void) {
	/* Configure the usart */
	REG_STORE(AT91C_PMC_PCER, (1 << OPTION_GET(NUMBER,dev_id)));

	REG_STORE(AT91C_PIOA_PDR, RX_PIN | TX_PIN |
			SCK_PIN | RTS_PIN | CTS_PIN);
	REG_STORE(AT91C_PIOA_ASR, RX_PIN | TX_PIN |
			SCK_PIN | RTS_PIN | CTS_PIN);

	REG_STORE(&(us_dev_regs->US_PTCR), (AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS));
	REG_STORE(&(us_dev_regs->US_CR),  AT91C_US_RSTSTA | AT91C_US_RSTRX | AT91C_US_RSTTX);
	REG_STORE(&(us_dev_regs->US_CR), AT91C_US_STTTO);
	REG_STORE(&(us_dev_regs->US_RTOR), 10000);
	REG_STORE(&(us_dev_regs->US_MR), (AT91C_US_USMODE_HWHSH & ~AT91C_US_SYNC)
			| AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE
			| AT91C_US_NBSTOP_1_BIT | AT91C_US_OVER);
	REG_STORE(&(us_dev_regs->US_IDR), ~0);
	REG_STORE(&(us_dev_regs->US_IER), AT91C_US_ENDRX);
	REG_STORE(&(us_dev_regs->US_BRGR), SYS_CLOCK / (8 * NXT_BT_BAUD_RATE));
	REG_STORE(&(us_dev_regs->US_RCR), 0);
	REG_STORE(&(us_dev_regs->US_TCR), 0);
	REG_STORE(&(us_dev_regs->US_RNPR), 0);
	REG_STORE(&(us_dev_regs->US_TNPR), 0);
	REG_LOAD(&(us_dev_regs->US_RHR));
	REG_LOAD(&(us_dev_regs->US_CSR));

	REG_STORE(&(us_dev_regs->US_CR), AT91C_US_RXEN | AT91C_US_TXEN);
	REG_STORE(&(us_dev_regs->US_PTCR), AT91C_PDC_RXTEN | AT91C_PDC_TXTEN);
}

static void init_control_pins(void) {
	/*configure control pins*/
	REG_STORE(AT91C_PIOA_PPUDR, CMD_PIN);
	pin_config_output(CS_PIN | RST_PIN | CMD_PIN);
	pin_set_output(CS_PIN | RST_PIN);
	pin_clear_output(CMD_PIN);

}

static void init_adc(void) {
	/* Configure the ADC */
	REG_STORE(AT91C_PMC_PCER, (1 << AT91C_ID_ADC));
	REG_STORE(AT91C_ADC_MR, 0);
	REG_ORIN(AT91C_ADC_MR, AT91C_ADC_TRGEN_DIS);
	REG_ORIN(AT91C_ADC_MR, 0x00000500); // 4MHz
	REG_ORIN(AT91C_ADC_MR, 0x001f0000); // 64uS
	REG_ORIN(AT91C_ADC_MR, 0x03000000); // 750nS
	REG_STORE(AT91C_ADC_CHER, AT91C_ADC_CH6 | AT91C_ADC_CH4);
	REG_STORE(AT91C_ADC_CR, AT91C_ADC_START);
}

/* timer handler
 * we scan PIN_BT4 for changing and if it changed bt state switch to disconnect
 *  mode.
 */
static void  nxt_bt_timer_handler(struct sys_timer *timer, void *param) {
	static int bt_last_state; //TODO init state? //inited with 0, ok
	int bt_state = REG_LOAD(AT91C_ADC_CDR6) > 0x200 ? 1 : 0;

	if (bt_last_state != bt_state) {
		struct pnet_pack *pack = pnet_pack_create(NULL, 1, PNET_PACK_TYPE_SINGLE);
		assert(pack);
		pack->node = &this_ctrl;

		*((uint8_t *) pnet_pack_get_data(pack)) = bt_state;
		if (0 != pnet_entry(pack)) {
			pnet_pack_destroy(pack);
		}
		bt_last_state = bt_state;
	}

	REG_STORE(AT91C_ADC_CR, AT91C_ADC_START);
}
void bluetooth_hw_hard_reset(void) {
	pin_clear_output(RST_PIN);
	ksleep(2000);
	pin_set_output(RST_PIN);
}

static int nxt_bluetooth_init(void) {
	struct sys_timer *ntx_bt_timer;

	data_pack = NULL;

	irq_attach(OPTION_GET(NUMBER,irq_num),
		nxt_bt_us_handler, 0, NULL, "nxt bt reader");
	// TODO error handling?

	init_usart();
	init_control_pins();
	init_adc();

	//TODO may be it must set when bt has been connected?
	return sys_timer_set(&ntx_bt_timer, SYS_TIMER_PERIODIC, 200, nxt_bt_timer_handler, NULL);
}
