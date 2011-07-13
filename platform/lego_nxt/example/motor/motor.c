/**
 * @file
 * @brief simple motor test
 *
 * @date 25.11.10
 * @author Anton Kozlov
 */

#include <framework/example/self.h>
#include <drivers/nxt/motor.h>

EMBOX_EXAMPLE(motor_run);

static int flag = 1;

static void motor1_stop(void) {
	nxt_motor_set_power(NXT_MOTOR_A, 0);
	flag = 0;
}

static int motor_run(int argc, char **argv) {

	nxt_motor_tacho_set_counter(NXT_MOTOR_A, 360, motor1_stop);
	nxt_motor_set_power(NXT_MOTOR_A, 100);

	while (flag)
		;

	return 0;
}
