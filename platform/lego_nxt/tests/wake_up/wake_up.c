/**
 * @file
 * @brief Light sensor test
 *
 * @date 26.02.2010
 * @author Anton Kozlov
 */
#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/nxt_buttons.h>
#include <drivers/nxt_sonar_sensor.h>

#include <drivers/nxt_motor.h>

EMBOX_TEST(wake_up_test);

#define TOUCH_PORT (&sensors[0])
#define MOTOR0 (&motors[0])
#define MOTOR1 (&motors[1])

#define MOTOR_POWER -100

int is_read = 0;
int treashold = 800;
int delta = 200;
bool moving = false;

static void move_start(void) {
	if (moving) {
		return;
	}

	TRACE("start O_O\n");

	motor_set_power(MOTOR0, MOTOR_POWER);
	motor_set_power(MOTOR1, MOTOR_POWER);

	moving = true;
}

static void move_stop(void) {
	if (!moving) {
		return;
	}
	TRACE("stop -_-\n");

	motor_set_power(MOTOR0, 0);
	motor_set_power(MOTOR1, 0);

	moving = false;
}

void sensor_handler(sensor_t *sensor, sensor_val_t val) {
	if (!is_read) {
		treashold = val;
		is_read = true;
		return;
	}

	if (val < (treashold - delta)) {
		move_start();
	} else {
		move_stop();
	}
}

static int wake_up_test(void) {
	uint8_t power_val = 0x01;

	data_to_avr.input_power = power_val;
	nxt_sensor_conf_pass(TOUCH_PORT, (sensor_hnd_t) sensor_handler);

	motor_start(MOTOR0, 0, 360, NULL);
	motor_start(MOTOR1, 0, 360, NULL);

	while (true);

	return 0;
}
