/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    31.05.2015
 */

#include <stdio.h>
#include <unistd.h>

#include <stm32f3xx.h>
#include <stm32f3_discovery.h>

#include <libsensors/acc.h>
#include <libsensors/gyro.h>
#include <libactuators/motor.h>

#include <libfilters/filtered_derivative.h>


// --- fault detection code START

static inline void fault_handle(void) {
	BSP_LED_On(LED3);
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

// --- fault detection code END

int main(int argc, char *argv[]) {
	struct motor motor1;

	BSP_LED_Init(LED3);
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

	motor_init(&motor1, GPIOD, MOTOR_ENABLE1, MOTOR_INPUT1, MOTOR_INPUT2);

	motor_enable(&motor1);

	while(BSP_PB_GetState(BUTTON_USER) != SET) ;

	motor_run(&motor1, MOTOR_RUN_RIGHT);
	{
		if (fault_detect()) {
			fault_handle();
		}
	}
	motor_stop(&motor1);

	return 0;
}
