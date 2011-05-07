/**
 * @file
 * @brief Color sensor test
 *
 * @date 05.11.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/nxt_buttons.h>
#include <drivers/pins.h>

#include <drivers/nxt_sensor.h>
#include <drivers/nxt_avr.h>

EMBOX_TEST(sensor_test);

#define TOUCH_PORT (&sensors[0])

sensor_val_t sval = 10;

void sensor_handler(sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

static int sensor_test(void) {
	nxt_sensor_conf_pass(TOUCH_PORT, (sensor_hnd_t) sensor_handler);

	uint8_t power_val = 0x01;
	data_to_avr.input_power = power_val;

	while (true) {
		int butt_state = nxt_buttons_was_pressed();
		if (butt_state & BT_ENTER) {
			TRACE("|%d", sval);
		}
		if (butt_state & BT_DOWN) {
			break;
		}
		if (butt_state & BT_LEFT) {
			power_val <<=1;
			if (power_val == 0) {
				power_val = 1;
			}
			data_to_avr.input_power = power_val;
		}
		usleep(500);
	}

	return 0;
}
