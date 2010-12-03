/**
 * @file
 * @brief simple motor test
 *
 * @date 25.11.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <drivers/nxt_avr.h>
#include <drivers/pins.h>
#include <kernel/diag.h>
#include <unistd.h>

#include <drivers/nxt_buttons.h>
#include <drivers/nxt_motor.h>

EMBOX_TEST(motor_run);

static void motor1_stop(void) {
	if (motor1.state == RUN) {
		motor_set_power(&motor1, 0);
	}
}

static int motor_run(void) {

	motor_start(&motor1, 100, 360, motor1_stop);

	return 0;
}
