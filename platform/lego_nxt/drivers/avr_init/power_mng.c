/**
 * @file
 * @brief nxt avr power manager driver
 *
 * @date 29.07.11
 * @author Anton Kozlov
 */

#include <drivers/nxt/avr.h>
#include <drivers/nxt/power_mng.h>

extern to_avr_t data_to_avr;

void nxt_halt(void) {
	data_to_avr.input_power = 0;
	data_to_avr.power = 0x5A;
	data_to_avr.pwm_frequency = 0x00;
	data_to_avr.input_power = 0;
}

void nxt_flash(void) {
	data_to_avr.input_power = 0;
	data_to_avr.power = 0xA5;
	data_to_avr.pwm_frequency = 0x5A;
}
