/**
 * @file
 * @brief blind intelegent robot
 * @details implements scheme: moving forward before getting
 * to wall, etc, then moving a few backward then stoping.
 *
 * @date 02.12.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/nxt_buttons.h>
#include <drivers/nxt_touch_sensor.h>

#include <drivers/nxt_motor.h>

#define MOTOR_POWER -100
#define BREAK_TIME 150

#define TOUCH_PORT (&sensors[3])
#define MOTOR0 (&motors[0])
#define MOTOR1 (&motors[1])

EMBOX_TEST(nxt_test_sensor_touch);

int flag = 1;

static void touch_handler(sensor_t *sensor) {
	if (!flag) {
		return;
	}
	motor_set_power(MOTOR0, -MOTOR_POWER);
	motor_set_power(MOTOR1, -MOTOR_POWER);
	flag = 0;
}

static int nxt_test_sensor_touch(void) {
	touch_sensor_init(TOUCH_PORT, (touch_hnd_t) touch_handler);
	motor_start(MOTOR0, MOTOR_POWER, 360, NULL);
	motor_start(MOTOR1, MOTOR_POWER, 360, NULL);

	while (flag && (!nxt_buttons_was_pressed())) {
		usleep(BREAK_TIME);
	}
	usleep(BREAK_TIME);
	motor_set_power(MOTOR0, 0);
	motor_set_power(MOTOR1, 0);

	return 0;
}

