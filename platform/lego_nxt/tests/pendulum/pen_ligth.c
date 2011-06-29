/**
 * @file
 * @brief Test for pendulum with light sensor
 *
 * @date 05.11.10
 * @author Alexandr Kalmuk
 */

#include <embox/test.h>
#include <drivers/nxt_sensor.h>
#include <drivers/nxt_motor.h>

#define MOTOR_POWER 100
#define MOTOR0 MOTOR_A
#define MOTOR1 MOTOR_B
#define BLACK_ZONE 500

EMBOX_TEST(pen_ligth);

#define TOUCH_PORT (&sensors[0])

sensor_val_t sval = 10;

void sensor_handler(sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

static int pen_ligth(void) {
	int motor_pov = 100;
	int cnt = 0;

	nxt_sensor_conf_pass(TOUCH_PORT, (sensor_hnd_t) sensor_handler);
	data_to_avr.input_power = 128;

	motor_start(MOTOR0, MOTOR_POWER, 360, NULL);
	motor_start(MOTOR1, MOTOR_POWER, 360, NULL);
	motor_set_power(MOTOR0, motor_pov);
	motor_set_power(MOTOR1, -motor_pov);

	while (true) {
		if (sval > BLACK_ZONE) {
			motor_pov *= (-1);
			motor_set_power(MOTOR0, motor_pov);
			motor_set_power(MOTOR1, -motor_pov);
			while (sval > BLACK_ZONE) {
			}
			cnt++;
		}
		if (cnt > 8)
			break;
	}

	while (sval < BLACK_ZONE + 40) {
	}
	while (true) {
		if (sval < BLACK_ZONE) {
			motor_pov *= (-1);
			motor_set_power(MOTOR0, motor_pov);
			motor_set_power(MOTOR1, -motor_pov);
			while (sval < BLACK_ZONE) {
			}
		}
	}

	motor_set_power(MOTOR0, 0);
	motor_set_power(MOTOR1, 0);

	return 0;
}
