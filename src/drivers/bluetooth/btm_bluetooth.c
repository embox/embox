/**
 * @file
 * @brief Implements BTM 112 Bluetooth driver.
 *
 * @date 15.07.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/unit.h>
#include <hal/reg.h>

#include <drivers/at91sam7s256.h>
#include <drivers/pins.h>
#include <drivers/bluetooth.h>
#include <kernel/timer.h>
#include <string.h>
#include <unistd.h>

extern void bt_handle(uint8_t *buff);

#define BTM_BT_RX_PIN  ((uint32_t) CONFIG_BTM_BT_RX_PIN)
#define BTM_BT_TX_PIN  ((uint32_t) CONFIG_BTM_BT_TX_PIN)
#define BTM_BT_SCK_PIN ((uint32_t) CONFIG_BTM_BT_SCK_PIN)
#define BTM_BT_RTS_PIN ((uint32_t) CONFIG_BTM_BT_RTS_PIN)
#define BTM_BT_CTS_PIN ((uint32_t) CONFIG_BTM_BT_CTS_PIN)

static volatile AT91PS_USART us_dev_regs = ((AT91PS_USART) CONFIG_BTM_BT_SERIAL_PORT_OFFSET);

#define BTM_BT_ADC_RATE 50000
#define BTM_BT_BAUD_RATE 19200

EMBOX_UNIT_INIT(btm_bluetooth_init);

static uint8_t *btm_bt_read_buff;

volatile enum {SIZE_READ, COMM_READ, UART_MODE} bt_us_state;

void bt_set_uart_state(void) {
	bt_us_state = UART_MODE;
	REG_STORE(&(us_dev_regs->US_IER), AT91C_US_ENDTX);
}

#define BUFF_SIZE 256
static uint8_t bt_buff[BUFF_SIZE];

//CALLBACK_INIT(bluetooth_uart)

static irq_return_t btm_bt_us_handler(int irq_num, void *dev_id) {
	uint32_t us_state = REG_LOAD(&(us_dev_regs->US_CSR));
	if (us_state & AT91C_US_ENDRX) {

	}
	return IRQ_HANDLED;
}

void bt_set_reset_low(void) {
	REG_STORE(AT91C_PIOA_CODR, CONFIG_BTM_BT_RST_PIN);
}

void bt_set_reset_high(void) {
	REG_STORE(AT91C_PIOA_SODR, CONFIG_BTM_BT_RST_PIN);
}

static void init_usart(void) {
	volatile uint8_t tmp;
	/* Configure the usart */
	REG_STORE(AT91C_PMC_PCER, (1 << CONFIG_BTM_BT_US_DEV_ID));

	REG_STORE(AT91C_PIOA_PDR, BTM_BT_RX_PIN | BTM_BT_TX_PIN |
			BTM_BT_SCK_PIN | BTM_BT_RTS_PIN | BTM_BT_CTS_PIN);
	REG_STORE(AT91C_PIOA_ASR, BTM_BT_RX_PIN | BTM_BT_TX_PIN |
			BTM_BT_SCK_PIN | BTM_BT_RTS_PIN | BTM_BT_CTS_PIN);

	REG_STORE(&(us_dev_regs->US_PTCR), (AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS));
	REG_STORE(&(us_dev_regs->US_CR),  AT91C_US_RSTSTA | AT91C_US_RSTRX | AT91C_US_RSTTX);
	REG_STORE(&(us_dev_regs->US_CR), AT91C_US_STTTO);
	REG_STORE(&(us_dev_regs->US_RTOR), 10000);
	REG_STORE(&(us_dev_regs->US_MR), (AT91C_US_USMODE_NORMAL & ~AT91C_US_SYNC)
			| AT91C_US_CLKS_CLOCK | AT91C_US_CHRL_8_BITS | AT91C_US_PAR_NONE
			| AT91C_US_NBSTOP_1_BIT | AT91C_US_OVER);
	REG_STORE(&(us_dev_regs->US_IDR), ~0);
	REG_STORE(&(us_dev_regs->US_IER), AT91C_US_ENDTX | AT91C_US_ENDRX);
	REG_STORE(&(us_dev_regs->US_BRGR), CONFIG_SYS_CLOCK / (8 * BTM_BT_BAUD_RATE));
	REG_STORE(&(us_dev_regs->US_RCR), 0);
	REG_STORE(&(us_dev_regs->US_TCR), 0);
	REG_STORE(&(us_dev_regs->US_RNPR), 0);
	REG_STORE(&(us_dev_regs->US_TNPR), 0);
	tmp = REG_LOAD(&(us_dev_regs->US_RHR));
	tmp = REG_LOAD(&(us_dev_regs->US_CSR));

	REG_STORE(&(us_dev_regs->US_CR), AT91C_US_RXEN | AT91C_US_TXEN);
	REG_STORE(&(us_dev_regs->US_PTCR), AT91C_PDC_RXTEN | AT91C_PDC_TXTEN);
}

static int btm_bluetooth_init(void) {
	const static char ver[] = "ATI?";
	init_usart();

	irq_attach((irq_nr_t) CONFIG_BTM_BT_US_IRQ,
		(irq_handler_t) &btm_bt_us_handler, 0, NULL, "bt reader");
	bt_set_reset_high();
	usleep(100);
	bt_set_reset_low();
	usleep(100);

	bluetooth_write((uint8_t *) ver, strlen(ver));
	bluetooth_read(bt_buff, 2);
	return 0;
}

size_t bluetooth_write(uint8_t *buff, size_t len) {
	while (!(REG_LOAD(&(us_dev_regs->US_CSR)) & AT91C_US_ENDTX)) {
	}
	REG_STORE(&(us_dev_regs->US_TPR), (uint32_t) buff);
	REG_STORE(&(us_dev_regs->US_TCR), len);
	return len;
}

size_t bluetooth_read(uint8_t *buff, size_t len) {
	btm_bt_read_buff = buff;
	REG_STORE(&(us_dev_regs->US_RPR), (uint32_t) buff);
	REG_STORE(&(us_dev_regs->US_RCR), len);

	return 0;
}
