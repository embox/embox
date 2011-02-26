/**
 * @file
 * @brief Power managment test of nxt
 *
 * @date 03.12.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <drivers/nxt_buttons.h>

#include <drivers/nxt_avr.h>


EMBOX_TEST(power_mng_test);

static void nxt_halt(void) {
	data_to_avr.input_power = 0;
	data_to_avr.pwm_frequency = 0x0;
	data_to_avr.input_power = 0;
}

static void nxt_flash(void) {
	data_to_avr.input_power = 0;
	data_to_avr.power = 0xA5;
	data_to_avr.pwm_frequency = 0x5A;
}

static int power_mng_test(void) {
	TRACE("L:HALT R:FLASH\n");
	while (true) {
		int buts = nxt_buttons_was_pressed();
		if (buts & BT_LEFT) {
			nxt_halt();
		}
		if (buts & BT_RIGHT) {
			nxt_flash();
		}
		usleep(200);
	}
	return 0;
}


