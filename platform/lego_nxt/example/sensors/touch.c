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
#include <embox/example.h>
#include <unistd.h>
#include <drivers/nxt/buttons.h>
#include <drivers/nxt/touch_sensor.h>

#include <drivers/nxt/motor.h>

#define MOTOR_POWER -100
#define BREAK_TIME 150

#define TOUCH_PORT NXT_SENSOR_1

EMBOX_EXAMPLE(touch_example);

int flag = 1;

static void touch_handler(nxt_sensor_t *sensor) {
	if (!flag) {
		return;
	}
	nxt_motor_set_power(NXT_MOTOR_A, -MOTOR_POWER);
	nxt_motor_set_power(NXT_MOTOR_B, -MOTOR_POWER);
	flag = 0;
}

static int touch_example(void) {
	touch_sensor_init(TOUCH_PORT, (touch_hnd_t) touch_handler);
	nxt_motor_set_power(NXT_MOTOR_A, MOTOR_POWER);
	nxt_motor_set_power(NXT_MOTOR_B, MOTOR_POWER);

	while (flag) {
		usleep(BREAK_TIME);
		if (nxt_buttons_pressed()) {
			nxt_motor_set_power(NXT_MOTOR_A, 0);
			nxt_motor_set_power(NXT_MOTOR_B, 0);
			return 1;
		}
	}
	usleep(BREAK_TIME);
	nxt_motor_set_power(NXT_MOTOR_A, 0);
	nxt_motor_set_power(NXT_MOTOR_B, 0);

	return 0;
}
