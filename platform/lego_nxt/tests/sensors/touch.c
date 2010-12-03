/**
 * @file
 * @brief nxt touch sensor test
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>

#include <drivers/nxt_touch_sensor.h>

#define TOUCH_PORT 0

EMBOX_TEST(nxt_test_sensor_touch);

int flag = 1;

static void touch_handler(sensor_t sensor) {
	flag = 0;
}

static int nxt_test_sensor_touch(void) {
	touch_sensor_init(TOUCH_PORT, (touch_hnd_t) touch_handler);

	while (flag);

	return 0;
}

