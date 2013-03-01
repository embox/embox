/**
 * @file
 * @brief nxt box around test
 * @details during the test robot travels clockwise around a rectangular box
 *
 * @date 17.01.11
 * @author Alexander Batyukov
 */

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <embox/example.h>
#include <drivers/nxt/buttons.h>
#include <drivers/nxt/sonar_sensor.h>
#include <drivers/nxt/motor.h>



EMBOX_EXAMPLE(box_around_cmd);

#define SONAR_PORT NXT_SENSOR_1
#define MOTOR0 NXT_MOTOR_A
#define MOTOR1 NXT_MOTOR_B

#define ROBOT_WIDTH 15

#define MOTOR_POWER -100

static int sonar_treshold = 50;

/* TODO: mb move this stuff to library */


static void rotate_start(int motor_power0, int motor_power1) {
	nxt_motor_set_power(MOTOR0, motor_power0);
	nxt_motor_set_power(MOTOR1, motor_power1);
}

static void move_start(void) {
	nxt_motor_set_power(MOTOR0, MOTOR_POWER);
	nxt_motor_set_power(MOTOR1, MOTOR_POWER);
}

static void move_stop(void) {
	nxt_motor_set_power(MOTOR0, 0);
	nxt_motor_set_power(MOTOR1, 0);
}

static int box_around_cmd(int argc, char **argv) {
	int mp0, mp1;

	nxt_sonar_init(SONAR_PORT);

	sonar_treshold = nxt_sensor_get_val(SONAR_PORT);

	printf ("distance is %d\n", sonar_treshold);

	mp0 = MOTOR_POWER * sonar_treshold / (sonar_treshold + ROBOT_WIDTH);
	mp1 = MOTOR_POWER;

	while (!nxt_buttons_pressed()) {
		while (abs(nxt_sensor_get_val(SONAR_PORT) - sonar_treshold) < 3) {
			move_start();
		}
		move_stop();

		while (abs(nxt_sensor_get_val(SONAR_PORT) - sonar_treshold) >= 3) {
			rotate_start(mp0, mp1);
		}

		move_stop();
	}

	return 0;
}
