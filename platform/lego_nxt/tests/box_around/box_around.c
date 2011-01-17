/**
 * @file
 * @brief nxt box around test
 * @details during the test robot travels clockwise around a rectangular box
 *
 * @date 17.01.11
 * @author Alexander Batyukov
 */
#include <types.h>
#include <embox/test.h>
#include <unistd.h>
#include <drivers/nxt_buttons.h>
#include <drivers/nxt_sonar_sensor.h>

#include <drivers/nxt_motor.h>

EMBOX_TEST(box_around_test);

#define SONAR_PORT (&sensors[1])
#define MOTOR0 (&motors[0])
#define MOTOR1 (&motors[1])

#define MOTOR_POWER -100
#define SONAR_TRESHOLD 5

/* TODO: mb move this stuff to library */
static void rotate_to_left(void) {
	motor_start(MOTOR0, 0, 1, NULL);
	motor_start(MOTOR1, MOTOR_POWER, 2, NULL);
}

static void move_start(void) {
	motor_start(MOTOR0, MOTOR_POWER, 1, NULL);
	motor_start(MOTOR1, MOTOR_POWER, 1, NULL);
}

static void move_stop(void) {
	motor_set_power(MOTOR0, 0);
	motor_set_power(MOTOR1, 0);
}

static int box_around_test(void) {
	while (!nxt_buttons_was_pressed()){
		while (sonar_sensor_get_val(SONAR_PORT) < SONAR_TRESHOLD) {
			move_start();
		}
		move_stop();
		rotate_to_left();
		move_start();
	}
	return 0;
}
