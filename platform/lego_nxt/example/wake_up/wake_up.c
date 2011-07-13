/**
 * @file
 * @brief Light sensor test
 *
 * @date 26.02.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/example.h>
#include <unistd.h>
#include <drivers/nxt/buttons.h>
#include <drivers/nxt/sonar_sensor.h>

#include <drivers/nxt/motor.h>

EMBOX_EXAMPLE(wake_up_example);

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

	printf("start O_O\n");

	nxt_motor_set_power(NXT_MOTOR_A, MOTOR_POWER);
	nxt_motor_set_power(NXT_MOTOR_B, MOTOR_POWER);
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
	printf("stop -_-\n");

	nxt_motor_set_power(NXT_MOTOR_A, 0);
	nxt_motor_set_power(NXT_MOTOR_B, 0);

	moving = false;
}

void sensor_handler(nxt_sensor_t *sensor, sensor_val_t val) {
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

static int wake_up_example(void) {

	nxt_sensor_conf_pass(NXT_SENSOR_1, (sensor_handler_t) sensor_handler);

	while (true);

	return 0;
}
