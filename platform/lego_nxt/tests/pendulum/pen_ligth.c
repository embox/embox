/**
 * @file
 * @brief Test for pendulum with light sensor
 *
 * @date 05.11.10
 * @author Alexandr Kalmuk
 */


#include <types.h>
#include <embox/test.h>
#include <drivers/nxt_avr.h>
#include <drivers/pins.h>
#include <kernel/diag.h>
#include <unistd.h>

#include <drivers/nxt_sensor.h>
#include <drivers/nxt_avr.h>

#include <drivers/nxt_buttons.h>
#include <drivers/nxt_motor.h>


#define MOTOR_POWER 100
#define MOTOR0 (&motors[0])
#define MOTOR1 (&motors[1])

EMBOX_TEST(pen_ligth);

#define TOUCH_PORT (&sensors[0])

sensor_val_t sval = 10;

void sensor_handler(sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

static int pen_ligth(void) {
	nxt_sensor_conf_pass(TOUCH_PORT, (sensor_hnd_t) sensor_handler);
	int power_val = 0x01;
	//int power_val = 0x01;
	data_to_avr.input_power = 128;

	int cnt = 0;
	motor_start(MOTOR0, MOTOR_POWER, 360, NULL);
	motor_start(MOTOR1, MOTOR_POWER, 360, NULL);
	int motor_pov = 100;
	motor_set_power(MOTOR0, motor_pov);
	motor_set_power(MOTOR1, -motor_pov);

	while (true) {
		if (sval > 500) {
			motor_pov *= (-1);
			motor_set_power(MOTOR0, motor_pov);
			motor_set_power(MOTOR1, -motor_pov);
			while (sval > 500) {
			}
			cnt++;
		}
		if (cnt > 8)
			break;
	}

	while (sval < 540) {
	}
	while (true) {
		if (sval < 500) {
			motor_pov *= (-1);
			motor_set_power(MOTOR0, motor_pov);
			motor_set_power(MOTOR1, -motor_pov);
			while (sval < 500) {
			}
		}
	}

	motor_set_power(MOTOR0, 0);
	motor_set_power(MOTOR1, 0);

	return 0;
}
