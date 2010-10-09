/**
 * @file
 * @brief TWI init test
 */

#include <embox/test.h>
#include <drivers/at91sam7s256.h>
#include <string.h>

EMBOX_TEST(run);

#define   I2CClk                        400000L
#define CLOCK_FREQUENCY 48054850
#define   CLDIV                         (((CLOCK_FREQUENCY/I2CClk)/2)-3)

#define NXT_AVR_ADDRESS 1
#define NXT_AVR_N_OUTPUTS 4
#define NXT_AVR_N_INPUTS  4

#define TWCK (1 << 4)
#define TWD (1 << 3)

#define U32 unsigned int
#define U8 unsigned char
#define S8 signed char

#define sysc ((volatile struct _AT91S_SYS *)0xFFFFF000)

static U32 twi_pending;
static U8 *twi_ptr;
static U32 twi_mask;

typedef struct{
  // Raw values
	U8 power;
	U8 pwm_frequency;
	S8 output_percent[NXT_AVR_N_OUTPUTS];
	U8 output_mode;
	U8 input_power;
} __attribute__((packed)) to_avr;

static to_avr data_avr;

const char avr_brainwash_string[] =
  "\xCC" "Let's samba nxt arm in arm, (c)LEGO System A/S";

void systick_wait_ns(U32 ns)
{
	volatile U32 x = (ns >> 7) + 1;

	while (x) {
		x--	;
	}
}

void twi_reset(void)
{
	U32 clocks = 9;

	*AT91C_TWI_IDR = ~0;

	*AT91C_PMC_PCER = (1 << AT91C_ID_PIOA) |  /* Need PIO too */
	(1 << AT91C_ID_TWI);    /* TWI clock domain */

	/* Set up pin as an IO pin for clocking till clean */
	*AT91C_PIOA_MDER = TWD | TWCK;
	*AT91C_PIOA_PER = TWD | TWCK;
	*AT91C_PIOA_ODR = TWD;
	*AT91C_PIOA_OER = TWCK;

	while (clocks > 0 && !(*AT91C_PIOA_PDSR & TWD)) {

		*AT91C_PIOA_CODR = TWCK;
		systick_wait_ns(1500);
		*AT91C_PIOA_SODR = TWCK;
		systick_wait_ns(1500);
		clocks--;
	}

	*AT91C_PIOA_PDR = TWD | TWCK;
	*AT91C_PIOA_ASR = TWD | TWCK;

	*AT91C_TWI_CR = AT91C_TWI_SWRST|AT91C_TWI_MSDIS;/* Disable & reset */

	*AT91C_TWI_CWGR = ((CLDIV << 8)|CLDIV);       /* Set for 400kHz */
	*AT91C_TWI_CR = AT91C_TWI_MSEN;       /* Enable as master */
	*AT91C_TWI_IER = AT91C_TWI_NACK;
	twi_mask = 0;
}

int twi_init(void)
{
	//int i_state;

	//i_state = interrupts_get_and_disable();

	*AT91C_TWI_IDR = ~0;      /* Disable all interrupt sources */
	//aic_mask_off(AT91C_ID_TWI);
	sysc->AIC_IDCR = (1 << AT91C_ID_TWI);
	//aic_set_vector(AT91C_ID_TWI, AIC_INT_LEVEL_ABOVE_NORMAL, (int)twi_isr_entry);
	sysc->AIC_SMR[AT91C_ID_TWI] = AIC_INT_LEVEL_ABOVE_NORMAL;
	//sysc->AIC_SVR[AT91C_ID_TWI] = twi_isr_entry;
	//aic_mask_on(AT91C_ID_TWI);
	sysc->AIC_IECR = (1 << AT91C_ID_TWI);

	twi_reset();

	/* Init peripheral */

	//twi_state = TWI_IDLE;

	//if (i_state)
	//interrupts_enable();

	return 1;
}

twi_start_write(U32 dev_addr, const U8 *data, U32 nBytes)
{
//	if (twi_state < TWI_RX_BUSY) {
//	twi_state = TWI_TX_BUSY;
	twi_ptr = (U8 *)data;
	twi_pending = nBytes;

	*AT91C_TWI_MMR = AT91C_TWI_IADRSZ_NO|((dev_addr & 0x7f) << 16);
	*AT91C_TWI_THR = *twi_ptr++;
	twi_pending--;
//	STATS(twi_stats.bytes_tx++)
	twi_mask = AT91C_TWI_TXRDY|AT91C_TWI_NACK;
	*AT91C_TWI_IER = AT91C_TWI_TXRDY;
//	}

}

/**
 * The test itself.
 *
 * @return the test result
 * @retval 0 on success
 * @retval nonzero on failure
 */
static int run(void) {
	int result = 0;
	twi_init();
	twi_start_write(NXT_AVR_ADDRESS, (const U8 *) avr_brainwash_string, strlen(avr_brainwash_string));

	data_avr.power = 0;
	data_avr.pwm_frequency = 0;
	data_avr.output_mode = 0;
	data_avr.input_power = 0;

	twi_start_write(NXT_AVR_ADDRESS, (const U8 *) &data_avr, sizeof(data_avr));

	/*
	 * The test itself.
	 */

	return result;
}
