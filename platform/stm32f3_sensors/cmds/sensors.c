/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @date    19.05.2015
 */

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

#include <stm32f3_discovery_gyroscope.h>
#include <stm32f3_discovery_accelerometer.h>

#define ACC_ARRAY_SIZE 2048

static int16_t x_values[ACC_ARRAY_SIZE];
static int16_t y_values[ACC_ARRAY_SIZE];

static void stm32f3_delay(void) {
	size_t i = 0;

	for (i = 0; i < 10000; i++)
		;
}

static void init_leds(void) {
	BSP_LED_Init(LED4);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED5);
	BSP_LED_Init(LED7);
	BSP_LED_Init(LED9);
	BSP_LED_Init(LED10);
	BSP_LED_Init(LED8);
	BSP_LED_Init(LED6);
}

static void acc_test(void) {
	int16_t buf[3] = {0};

	for (size_t i = 0; i < ACC_ARRAY_SIZE; i++) {
		BSP_ACCELERO_GetXYZ(buf);

		x_values[i] = buf[0];
		y_values[i] = buf[1];

		stm32f3_delay();
	}
}

static void gyro_test(void) {
	float buf[3] = {0};
	float x, y;

	for (size_t i = 0; i < 100; i++) {
		BSP_GYRO_GetXYZ(buf);

		x = abs(buf[0]);
		y = abs(buf[1]);

		printf("x=%f y=%f z=%f\n", x, y);
		stm32f3_delay();
	}
}

int main(int argc, char *argv[]) {
	int res;

	HAL_Init();

	init_leds();

	res = BSP_ACCELERO_Init();
	if (res != HAL_OK) {
		printf("BSP_ACCLEERO_Init failed, returned %d\n", res);
		return -1;
	}

	res = BSP_GYRO_Init();
	if (res != HAL_OK) {
		printf("BSP_GYRO_Init failed, returned %d\n", res);
		return -1;
	}

	BSP_LED_On(LED3);

	acc_test();
	gyro_test();

	return 0;
}
