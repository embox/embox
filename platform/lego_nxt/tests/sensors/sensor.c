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

#include <drivers/nxt_sensor.h>

EMBOX_TEST(sensor_test);

#define TOUCH_PORT 1

#define DIGIA0 23
#define DIGIA1 18
#define DIGIB0 28
#define DIGIB1 19
#define DIGIC0 29
#define DIGIC1 20
#define DIGID0 30
#define DIGID1 2

int flag = 1;

void sensor_handler(sensor_t sensor, sensor_val_t val) {
	if (nxt_buttons_are_pressed() & BT_ENTER) {
		TRACE("%d\n", val);
	}
	if (nxt_buttons_are_pressed() & BT_DOWN) {
		flag = 0;
	}
}

static int sensor_test(void) {
	nxt_sensor_conf_pass(TOUCH_PORT, (sensor_hnd_t) sensor_handler);

	pin_config_output((1 << DIGIB0) | (1 << DIGIB1));
	pin_set_output((1 << DIGIB0) | (1 << DIGIB1));

	while (flag);

	return 0;
}
