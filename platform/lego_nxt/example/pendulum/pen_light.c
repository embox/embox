/**
 * @file
 * @brief Example for pendulum with light sensor
 *
 * @date 05.11.10
 * @author Alexandr Kalmuk
 */

#include <embox/example.h>
#include <drivers/nxt/sensor.h>
#include <drivers/nxt/motor.h>

#define MOTOR_POWER 100
#define MOTOR0 NXT_MOTOR_A
#define MOTOR1 NXT_MOTOR_B
#define BLACK_ZONE 500

EMBOX_EXAMPLE(pen_light);

#define LIGHT_PORT NXT_SENSOR_1

static sensor_val_t sval = 10;

void sensor_handler(nxt_sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

static int pen_light(int argc, char **argv) {
	int motor_pov = 100;
	int cnt = 0;

	nxt_sensor_conf_pass(LIGHT_PORT, sensor_handler);
	/*data_to_avr.input_power = 128;*/

	nxt_motor_set_power(MOTOR0, motor_pov);
	nxt_motor_set_power(MOTOR1, -motor_pov);

	while (true) {
		if (sval > BLACK_ZONE) {
			motor_pov *= (-1);
			nxt_motor_set_power(MOTOR0, motor_pov);
			nxt_motor_set_power(MOTOR1, -motor_pov);
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
			nxt_motor_set_power(MOTOR0, motor_pov);
			nxt_motor_set_power(MOTOR1, -motor_pov);
			while (sval < BLACK_ZONE) {
			}
		}
	}

	nxt_motor_set_power(MOTOR0, 0);
	nxt_motor_set_power(MOTOR1, 0);

	return 0;
}
