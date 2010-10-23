/**
 * @file
 * @brief TWI init test
 *
 * @date 16.10.10
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <drivers/at91sam7s256.h>
#include <string.h>
#include <hal/reg.h>
#include <drivers/twi.h>
#include <unistd.h>

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

#define DEL 10
static from_avr data_from_avr;
static int run(void) {
	int result = 0;
	int old_state = 0;
	int state_count = DEL;

	twi_init();
	twi_write(NXT_AVR_ADDRESS, (const uint8_t *) avr_brainwash_string, strlen(avr_brainwash_string));

	data_avr.power = 0;
	data_avr.pwm_frequency = 0;
	data_avr.output_mode = 0;
	data_avr.input_power = 0;

	while (1) {
		int new_state = 0;
		twi_send(NXT_AVR_ADDRESS, (const uint8_t *) &data_avr, sizeof(data_avr));
		usleep(1);
		twi_receive(NXT_AVR_ADDRESS, (uint8_t *) &data_from_avr, sizeof(data_from_avr));
		new_state = data_from_avr.buttonsVal > 1500;
		if (new_state == old_state) {
			if (!state_count--) {
				if (new_state) { //button pressed
					//nxt_lcd_set_all_pixels_on(1);
					//switch_bank();
				} else { //button released
				}
			}
		} else { //floating value
			old_state = new_state;
			state_count = DEL;
			//nxt_lcd_set_all_pixels_on(0);
		}
	}

	return result;
}
