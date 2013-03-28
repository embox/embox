/**
 * @file
 * @brief Example for pendulum without any sensors
 *
 * @date 10.05.11
 * @author Alexandr Kalmuk
 */

#include <embox/example.h>
#include <unistd.h>
#include <drivers/nxt/motor.h>

#define MOTOR_POWER 100
#define MOTOR0 NXT_MOTOR_A
#define MOTOR1 NXT_MOTOR_B

EMBOX_EXAMPLE(without_sensors);

static int without_sensors(int argc, char **argv) {
	int motor_pov = 100;

	usleep(210);
	motor_pov *= (-1);

	for (int i = 1; i < 8; i++) {
		nxt_motor_set_power(MOTOR0, motor_pov);
		nxt_motor_set_power(MOTOR1, -motor_pov);
		usleep(420);
		motor_pov *= (-1);
	}
	for (int i = 1; i < 7; i++) {
		nxt_motor_set_power(MOTOR0, motor_pov);
		nxt_motor_set_power(MOTOR1, -motor_pov);
		usleep(600);
		motor_pov *= (-1);
	}
	nxt_motor_set_power(MOTOR0, motor_pov);
	nxt_motor_set_power(MOTOR1, -motor_pov);
	usleep(100);
	motor_pov *= (-1);

	nxt_motor_set_power(MOTOR0, 0);
	nxt_motor_set_power(MOTOR1, 0);

	usleep(400);
	motor_pov *= (-1);
	nxt_motor_set_power(MOTOR0, motor_pov);
	nxt_motor_set_power(MOTOR1, -motor_pov);
	usleep(600);
	for (int i = 1; i < 7; i++) {
		nxt_motor_set_power(MOTOR0, motor_pov);
		nxt_motor_set_power(MOTOR1, -motor_pov);
		usleep(150);
		motor_pov *= (-1);
	}

	return 0;
}
