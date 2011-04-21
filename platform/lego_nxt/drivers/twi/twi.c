/**
 * @file
 * @brief TWI communication for Lego AVR
 *
 * @date 26.09.2010
 * @author Anton Kozlov
 * @author Fedor Burdun
 */

#include <drivers/at91sam7s256.h>
#include <drivers/twi.h>
#include <hal/reg.h>
#include <drivers/nxt_avr.h>

#define   I2CClk           400000L
#define   CLDIV            (((CONFIG_SYS_CLOCK/I2CClk)/2)-3)

//#define AT91C_PA4_TWCK AT91C_PA4_AT91C_PA4_TWCK //(1 << 4)
//#define AT91C_PA3_TWD	 AT91C_PA3_AT91C_PA3_TWD //(1 << 3)

//#define sysc ((volatile struct _AT91S_SYS *) 0xFFFFF000)

static uint32_t twi_pending;
static uint8_t *twi_ptr;
static uint8_t checkbyte = 0;
static uint32_t twi_mask;

enum twi_state_t twi_state;

static uint8_t out_buff[sizeof(to_avr_t) + 1];

static void systick_wait_ns(uint32_t ns) {
	volatile uint32_t x = (ns >> 7) + 1;

	while (x) {
		x--;
	}
}

static irq_return_t twi_handler(int irq_num, void *dev_id) {
	switch (twi_state) {
		case TWI_READ_BUT_LAST:
			*twi_ptr = REG_LOAD(AT91C_TWI_RHR);
			avr_read_done();
			twi_state = TWI_COMP;
			REG_STORE(AT91C_TWI_IDR, AT91C_TWI_RXRDY);
			REG_STORE(AT91C_TWI_IER, AT91C_TWI_TXCOMP);
			break;
		case TWI_READ:
			*twi_ptr = REG_LOAD(AT91C_TWI_RHR);
			twi_ptr++;
			if (--twi_pending == 1) {
				*AT91C_TWI_CR = AT91C_TWI_STOP;
				twi_state = TWI_READ_BUT_LAST;
			}
			break;
		case TWI_WRITE:
			REG_STORE(AT91C_TWI_THR, *twi_ptr++);
			if (--twi_pending == 0) {
				twi_state = TWI_COMP;
				REG_STORE(AT91C_TWI_IDR, AT91C_TWI_TXRDY);
				REG_STORE(AT91C_TWI_IER, AT91C_TWI_TXCOMP);
			};
			break;
		case TWI_COMP:
			twi_state = TWI_IDLE;
			REG_STORE(AT91C_TWI_IDR, AT91C_TWI_TXCOMP);
			break;
		case TWI_IDLE:
		default:
			break;
	}
	return IRQ_HANDLED;
}

static void twi_reset(void) {
	uint32_t clocks = 9;

	REG_STORE(AT91C_TWI_IDR, ~0);

	*AT91C_PMC_PCER = (1 << AT91C_ID_PIOA) |  /* Need PIO too */
			(1 << AT91C_ID_TWI);    /* TWI clock domain */

	/* Set up pin as an IO pin for clocking till clean */
	REG_STORE(AT91C_PIOA_MDER, AT91C_PA3_TWD | AT91C_PA4_TWCK);
	REG_STORE(AT91C_PIOA_PER, AT91C_PA3_TWD | AT91C_PA4_TWCK);
	REG_STORE(AT91C_PIOA_ODR, AT91C_PA3_TWD);
	REG_STORE(AT91C_PIOA_OER, AT91C_PA4_TWCK);

	while (clocks > 0 && !(*AT91C_PIOA_PDSR & AT91C_PA3_TWD)) {
		REG_STORE(AT91C_PIOA_CODR, AT91C_PA4_TWCK);
		systick_wait_ns(1500);
		REG_STORE(AT91C_PIOA_SODR, AT91C_PA4_TWCK);
		systick_wait_ns(1500);
		clocks--;
	}

	REG_STORE(AT91C_PIOA_PDR, AT91C_PA3_TWD | AT91C_PA4_TWCK);
	REG_STORE(AT91C_PIOA_ASR, AT91C_PA3_TWD | AT91C_PA4_TWCK);

	/* Disable & reset */
	REG_STORE(AT91C_TWI_CR, AT91C_TWI_SWRST | AT91C_TWI_MSDIS);

	/* Set for 400kHz */
	REG_STORE(AT91C_TWI_CWGR, ((CLDIV << 8) | CLDIV));
	/* Enable as master */
	REG_STORE(AT91C_TWI_CR, AT91C_TWI_MSEN);
	//*AT91C_TWI_IER = AT91C_TWI_NACK;
	twi_mask = 0;
	twi_state = TWI_IDLE;
	irq_attach((irq_nr_t) AT91C_ID_TWI,
		(irq_handler_t) &twi_handler, 5, NULL, "twi reader");
}

void twi_init(void) {
	twi_reset();
}

int twi_write(uint32_t dev_addr, const uint8_t *data, uint32_t nBytes) {
	if (twi_state != TWI_IDLE) {
		return 1;
	}
	twi_ptr = (uint8_t *) data;
	twi_pending = nBytes;

	*AT91C_TWI_MMR = AT91C_TWI_IADRSZ_NO | ((dev_addr & 0x7f) << 16);
	twi_state = TWI_WRITE;
	REG_STORE(AT91C_TWI_IER, AT91C_TWI_TXRDY);
	return 0;
}

int twi_send(uint32_t dev_addr, const uint8_t *data, uint32_t count) {
	const uint8_t *sptr = data;
	uint8_t *dptr = out_buff;
	uint8_t checkbyte = 0;
	uint32_t left_count = count;
	while (left_count-- > 0) {
		checkbyte += *sptr;
		*dptr++ = *sptr++;
	}
	*dptr = ~checkbyte;
	return twi_write(dev_addr, out_buff, count + 1);
}

int twi_receive(uint32_t dev_addr, uint8_t *data, uint32_t count) {
	if (twi_state != TWI_IDLE) {
		return 1;
	}
	twi_ptr = data;
	twi_pending = count;
	*AT91C_TWI_MMR = AT91C_TWI_IADRSZ_NO |
			AT91C_TWI_MREAD | ((dev_addr & 0x7f) << 16);
	twi_state = TWI_READ;
	*AT91C_TWI_CR = AT91C_TWI_START;
	REG_STORE(AT91C_TWI_IER, AT91C_TWI_RXRDY);
	return 0;
}


