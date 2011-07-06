/**
 * @file
 * @brief Light sensor test
 *
 * @date 26.02.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/nxt_buttons.h>
#include <drivers/nxt_sonar_sensor.h>

#include <drivers/nxt_motor.h>

EMBOX_TEST(wake_up_test);

#define TOUCH_PORT SENSOR_1
#define MOTOR0 MOTOR_A
#define MOTOR1 MOTOR_B

#define MOTOR_POWER -100
#define STOP_TIME 100

static int is_read = 0;
static int treashold = 800;
static int delta = 100;
static bool moving = false;
static int counter = 0;

static void move_start(void) {
	if (moving) {
		return;
	}

	TRACE("start O_O\n");

	motor_set_power(MOTOR0, MOTOR_POWER);
	motor_set_power(MOTOR1, MOTOR_POWER);
	counter = STOP_TIME;
	moving = true;
}

static void move_stop(void) {
	if (!moving) {
		return;
	}
	if (counter--) {
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
