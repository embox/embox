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
static uint32_t twi_mask;

static uint8_t out_buff[sizeof(to_avr_t) + 1];

static void systick_wait_ns(uint32_t ns) {
	volatile uint32_t x = (ns >> 7) + 1;

	while (x) {
		x--;
	}
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
}

void  twi_init(void) {
	twi_reset();
}

void twi_write(uint32_t dev_addr, const uint8_t *data, uint32_t nBytes) {
	twi_ptr = (uint8_t *) data;
	twi_pending = nBytes;

	*AT91C_TWI_MMR = AT91C_TWI_IADRSZ_NO | ((dev_addr & 0x7f) << 16);
	*AT91C_TWI_THR = *twi_ptr++;
	twi_pending--;
	while (twi_pending > 0) {
		while (!(REG_LOAD(AT91C_TWI_SR) & AT91C_TWI_TXRDY));
		REG_STORE(AT91C_TWI_THR, *twi_ptr++);
		twi_pending--;
	}

	while (!(REG_LOAD(AT91C_TWI_SR) & AT91C_TWI_TXCOMP));
}

void twi_send(uint32_t dev_addr, const uint8_t *data, uint32_t count) {
	const uint8_t *sptr = data;
	uint8_t *dptr = out_buff;
	uint8_t checkbyte = 0;
	uint32_t left_count = count;
	while (left_count-- > 0) {
		checkbyte += *sptr;
		*dptr++ = *sptr++;
	}
	*dptr = ~checkbyte;
	twi_write(dev_addr, out_buff, count + 1);
}

int twi_receive(uint32_t dev_addr, uint8_t *data, uint32_t count) {
	uint8_t *ptr = data;
	uint8_t checkbyte = 0;
	*AT91C_TWI_MMR = AT91C_TWI_IADRSZ_NO |
			AT91C_TWI_MREAD | ((dev_addr & 0x7f) << 16);
	*AT91C_TWI_CR = AT91C_TWI_START;
	while (count-- > 1) {
		while (!(REG_LOAD(AT91C_TWI_SR) & AT91C_TWI_RXRDY));
		*ptr = REG_LOAD(AT91C_TWI_RHR);
		checkbyte += *ptr;
		ptr++;
	}

	*AT91C_TWI_CR = AT91C_TWI_STOP;
	while (!(REG_LOAD(AT91C_TWI_SR) & AT91C_TWI_RXRDY));
	*ptr = REG_LOAD(AT91C_TWI_RHR);
	checkbyte += *ptr;

	while (!(REG_LOAD(AT91C_TWI_SR) & AT91C_TWI_TXCOMP));

	return ((checkbyte == 0xff) ? 1 : 0);
}

