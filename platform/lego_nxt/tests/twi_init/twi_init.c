/**
 * @file twi_init.c
 * @brief TWI init test
 *
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <drivers/at91sam7s256.h>
#include <string.h>
#include <hal/reg.h>
#include <drivers/twi.h>


EMBOX_TEST(run);

const char avr_brainwash_string[] =
  "\xCC" "Let's samba nxt arm in arm, (c)LEGO System A/S";

static to_avr data_avr;
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
	twi_write(NXT_AVR_ADDRESS, (const uint8_t *) avr_brainwash_string, strlen(avr_brainwash_string));

	data_avr.power = 0;
	data_avr.pwm_frequency = 0;
	data_avr.output_mode = 0;
	data_avr.input_power = 0;


	while (1) {
		twi_send(NXT_AVR_ADDRESS, (const uint8_t *) &data_avr, sizeof(data_avr));
	}


	return result;
}
