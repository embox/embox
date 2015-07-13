/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    31.05.2015
 */

#include <stdio.h>
#include <unistd.h>

#include <libsensors/acc.h>
#include <libsensors/gyro.h>
#include <libactuators/motor.h>
#include <libmisc/led.h>
#include <libmisc/button.h>

#include <libfilters/filtered_derivative.h>

static inline void fault_handle(void) {
	led_on(LED3);
}

static int fault_detect(void) {
	int16_t buf[3] = {0, 0, 0};
	float filtered_val;

	while(1) {
		acc_get(buf);

		filtered_val = filtered_derivative(buf[0]);
		if (filtered_derivative_check(filtered_val)) {
			return 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	struct motor motor1;

	led_init(LED3);
	button_init(BUTTON_USER);
	motor_init(&motor1, GPIOD, MOTOR_ENABLE1, MOTOR_INPUT1, MOTOR_INPUT2);

	motor_enable(&motor1);

	/* Wait until button get pressed */
	button_wait_set(BUTTON_USER);

	motor_run(&motor1, MOTOR_RUN_RIGHT);
	{
		if (fault_detect()) {
			fault_handle();
		}
	}
	motor_stop(&motor1);

	return 0;
}
