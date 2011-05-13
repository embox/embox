/**
 * @file
 * @brief simple motor test
 *
 * @date 25.11.10
 * @author Anton Kozlov
 */

#include <embox/test.h>
#include <drivers/nxt_motor.h>

EMBOX_TEST(motor_run);

static int flag = 1;

static void motor1_stop(void) {
	if (motors[0].state == RUN) {
		motor_set_power(&motors[0], 0);
		flag = 0;
	}
}

static int motor_run(void) {

	motor_start(&motors[0], 100, 360, motor1_stop);

	while (flag)
		;

	return 0;
}
