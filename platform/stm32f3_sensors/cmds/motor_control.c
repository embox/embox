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
#include <stm32f3xx_hal_gpio.h>
#include <stm32f3xx_hal_rcc.h>

#include <libsensors/acc.h>
#include <libsensors/gyro.h>
#include <libactuators/motor.h>


static void init_pins(GPIO_TypeDef  *GPIOx, uint16_t pins) {
	GPIO_InitTypeDef  GPIO_InitStruct;

	GPIO_InitStruct.Pin = pins;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

// --- fault detection code START

static float ema = 0;

#define FD_WIN_SIZE   200
#define FD_THRESHOLD  2600
static float f_dv[FD_WIN_SIZE] = {0};
static int index_l = 0;
static int index_h = FD_WIN_SIZE / 2;
static int index = 0;
static float diff = 0;

#if 0
#define INC_WINDOW_SIZE 50
#define EMA_ALPHA ((float) 2.0 / (float) (INC_WINDOW_SIZE + 1))

static float incs[INC_WINDOW_SIZE] = {0};
static int inc_index = 0;
static float average_inc = 0;

static inline float moving_average(float inc) {
	average_inc += (inc - incs[inc_index]) / INC_WINDOW_SIZE;
	incs[inc_index] = inc;
	inc_index = (inc_index + 1) % INC_WINDOW_SIZE;
	return average_inc;
}

static inline float exp_moving_average(float inc) {
	ema += (inc - ema) * EMA_ALPHA;
	return ema;
}
#endif

static inline float filtered_derivative(float inc) {
	diff += ((inc - 2 * f_dv[index_h] + f_dv[index_l]) / FD_WIN_SIZE);
	f_dv[index] = inc;
	index = (index + 1) % FD_WIN_SIZE;
	index_h = (index_h + 1) % FD_WIN_SIZE;
	index_l = (index_l + 1) % FD_WIN_SIZE;
	return diff;
}

static inline void fault_handle(void) {
	BSP_LED_On(LED3);
}

static int fault_detect(void) {
	int16_t buf[3] = {0, 0, 0};
	float filtered_val;

	acc_get(buf);
	ema = buf[0];

	while(1) {
		acc_get(buf);

		filtered_val = filtered_derivative(buf[0]);
		if (filtered_val < -FD_THRESHOLD || filtered_val > FD_THRESHOLD) {
			return 1;
		}
	}
	return 0;
}

// --- fault detection code END

int main(int argc, char *argv[]) {
	struct motor motor1;

	BSP_LED_Init(LED4);
	BSP_LED_Init(LED3);
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

	init_pins(GPIOD, MOTOR_ENABLE1 | MOTOR_INPUT1 | MOTOR_INPUT2 |
			MOTOR_ENABLE2 | MOTOR_INPUT3 | MOTOR_INPUT4);

	motor_init(&motor1, GPIOD, MOTOR_ENABLE1, MOTOR_INPUT1,
		MOTOR_INPUT2, 0x3);

	motor_enable(&motor1);

	while(BSP_PB_GetState(BUTTON_USER) != SET)
		;

	motor_run(&motor1, MOTOR_RUN_RIGHT);
	{

		if (fault_detect()) {
			fault_handle();
		}
	}
	motor_stop(&motor1);

	return 0;
}
