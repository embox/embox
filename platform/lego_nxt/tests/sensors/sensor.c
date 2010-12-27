/**
 * @file
 * @brief Color sensor test
 *
 * @date 05.11.2010
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/nxt_buttons.h>
#include <drivers/pins.h>

#include <drivers/nxt_sensor.h>

EMBOX_TEST(sensor_test);

#define TOUCH_PORT (&sensors[3])

sensor_val_t sval = 10;

void sensor_handler(sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

static int sensor_test(void) {
	nxt_sensor_conf_pass(TOUCH_PORT, (sensor_hnd_t) sensor_handler);

	//pin_config_output((1 << TOUCH_PORT->n0p) | (1 << TOUCH_PORT->n1p));
	//pin_set_output((1 << TOUCH_PORT->n0p | (1 << TOUCH_PORT->n1p)));
	//pin_clear_output((1 << DIGIB0) | (1 << DIGIB1));

	while (true) {
		int butt_state = nxt_buttons_was_pressed();
		if (butt_state & BT_ENTER) {
			TRACE("%d\n", sval);
		}
		if (butt_state & BT_DOWN) {
			break;
		}
		usleep(500);
	}

	return 0;
}
