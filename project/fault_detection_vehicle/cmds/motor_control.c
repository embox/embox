/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    31.05.2015
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <drivers/sensors/acc.h>
#include <drivers/sensors/gyro.h>
#include <libactuators/motor.h>
#include <libmisc/led.h>
#include <libmisc/button.h>

#include <drivers/gpio.h>
#include <config/board_config.h>

#include <libfilters/filtered_derivative.h>
#include <libfilters/dynamic_window.h>

#define K 10

static struct led led3;
static struct button user_button;

static inline void fault_handle(void) {
	led_on(&led3);
}

static int fault_detect(void) {
	int16_t buf[3] = {0, 0, 0};
	float val;

	while(1) {
		acc_get(buf);

		val = buf[0];
		val = dyn_window_avg(val, K);

		val = filtered_derivative(abs(val));
		if (filtered_derivative_check(val)) {
			return 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	struct motor motor1;

	led_init(&led3, CONF_LED3_GPIO_PORT, CONF_LED3_GPIO_PIN);
	button_init(&user_button, STM_USER_BUTTON_GPIO_PORT, 1 << STM_USER_BUTTON_PIN);
	motor_init(&motor1);

	motor_enable(&motor1);

	/* Wait until button get pressed */
	button_wait_set(&user_button);

	motor_run(&motor1, MOTOR_RUN_FORWARD);
	{
		if (fault_detect()) {
			fault_handle();
		}
	}
	motor_stop(&motor1);

	return 0;
}
