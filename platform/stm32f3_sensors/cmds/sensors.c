/**
 * @file
 * @brief
 *
 * @author  Alex Kalmuk
 * @author  Vita Loginova
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

#include <drivers/block_dev/flash/flash_dev.h>
#include <hal/clock.h>
#include <kernel/time/time.h>

#include <kalman_filter.h>

#define g 9.80665f
#define PI 3.14159265358979f

static float gyro_offset[3], acc_offset[3];
static float angles[3];
static float speed = 0;
static int inited = 0;

static struct kalman_filter kalman[3];

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


/* mm/s^2 */
static void acc_data_normalize(int16_t *in, float *out) {
	for (int i = 0; i < 3; i++)
		out[i] = (in[i] / 16) * g;
}

/* mm/s^2, without offset */
static void acc_data_obtain(float *out) {
	int16_t acc[3] = {0};
	BSP_ACCELERO_GetXYZ(acc);
	acc_data_normalize(acc, out);

	for (int i = 0; i < 3; i++)
		out[i] -= acc_offset[i];
}

/* mrad/s */
static void gyro_data_normalize(float *in, float *out) {
	for (int i = 0; i < 3; i++)
		out[i] = in[i] * 8.75f / 360 * 2 * PI;
}

/* mrad/s, without offset */
static void gyro_data_obtain(float *out) {
	float gyro[3] = {0};
	BSP_GYRO_GetXYZ(gyro);
	gyro_data_normalize(gyro, out);

	for (int i = 0; i < 3; i++)
		out[i] -= gyro_offset[i];
}

#define INC_WINDOW_SIZE 10
float incs[INC_WINDOW_SIZE] = {0};
int inc_index = 0;
float average_inc = 0;

/**
 * speed in mm/s
 * @param acceleration mm/s^2
 * @param compensation mm/s^2
 * @param dt           seconds
 */
static void update_speed(float acceleration, float dt) {
	float inc = acceleration*dt;
	average_inc += (inc - incs[inc_index])/INC_WINDOW_SIZE;
	incs[inc_index] = inc;
	inc_index = (inc_index + 1) % INC_WINDOW_SIZE;

	speed += average_inc;
}

/**
 * Update angles, output value in radians
 * @param acc  mm/s^2
 * @param gyro mrad/s
 * @param dt   seconds
 */
void update_angles(float *acc, float *gyro, float dt) {
	float eps = 0.07;
	float roll  = atan2(acc[1], acc[2]);
	float pitch = atan(-acc[0] / sqrt(acc[1] * acc[1] + acc[2] * acc[2]));

	if (!inited) {
		angles[0] = roll;
		angles[1] = pitch;

		kalman_filter_init(&kalman[0], roll);
		kalman_filter_init(&kalman[1], pitch);

		inited = 1;
		return;
	}

	/* complementary filter */
	angles[0] = (1 - eps) * (angles[0] + gyro[0]/1000 * dt) + eps * roll;
	angles[1] = (1 - eps) * (angles[1] + gyro[1]/1000 * dt) + eps * pitch;

	kalman_filter_iterate(&kalman[0], roll, gyro[0]/1000, dt);
	kalman_filter_iterate(&kalman[1], pitch, gyro[1]/1000, dt);
}

static void speed_test(void) {
	float acc[3] = {0};
	float gyro[3] = {0};
	int prev = 0;

	while(1) {
		time64_t current;
		float dt, compensation;

		BSP_LED_Off(LED3);
		BSP_LED_Off(LED4);
		BSP_LED_Off(LED5);
		BSP_LED_Off(LED6);
		BSP_LED_Off(LED7);

		gyro_data_obtain(gyro);
		acc_data_obtain(acc);

		current = jiffies2ms(clock_sys_ticks());
		dt = (current - prev) / 1000.0; /* seconds */
		prev = current;

		update_angles(acc, gyro, dt);

		compensation = 1000 * sin(kalman[1].value) * g;
		update_speed(acc[0] - compensation, dt);

		if (speed > 50) {
			BSP_LED_On(LED7);
		} else if (speed > 10) {
			BSP_LED_On(LED5);
		} else if (speed < -50) {
			BSP_LED_On(LED6);
		} else if (speed < -10) {
			BSP_LED_On(LED4);
		} else {
			BSP_LED_On(LED3);
		}

		stm32f3_delay();
	}
}

static void gyro_calculate_offset(){
	float eps = 0.2;

	for (int  i= 0; i < 1000; i++) {
		float gyro[3] = {0};
		BSP_GYRO_GetXYZ(gyro);

		for (int j = 0; j < 3; j++)
			gyro_offset[j] = (1 - eps)*gyro_offset[j] + eps*gyro[j];
	}
	gyro_data_normalize(gyro_offset, gyro_offset);
}

static void acc_calculate_offset(){
	int16_t temp[3] = {0};
	float eps = 0.2;

	for (int i=0; i<1000; i++){
		int16_t acc[3] = {0};
		BSP_ACCELERO_GetXYZ(acc);

		for (int j = 0; j < 3; j++)
			temp[j] = (1 - eps) * temp[j] + eps*acc[j];
	}

	acc_data_normalize(temp, acc_offset);
	acc_offset[2] -= g*1000;
}

#define ACC_ARRAY_SIZE 2048
#define ACC_VALUE_SIZE 2 /* Two bytes per value */
extern struct flash_dev stm32f3_flash;
static void acc_test(void) {
	static const uint32_t x_values_offset = 0;
	static const uint32_t y_values_offset = ACC_VALUE_SIZE * ACC_ARRAY_SIZE;
	int16_t buf[3] = {0};

	for (size_t i = 0; i < 8; i++)
		flash_erase(&stm32f3_flash, i);

	for (size_t i = 0; i < ACC_ARRAY_SIZE; i++) {
		BSP_ACCELERO_GetXYZ(buf);
		flash_write(&stm32f3_flash,
				x_values_offset + i * ACC_VALUE_SIZE,
				&buf[0],
				ACC_VALUE_SIZE);

		flash_write(&stm32f3_flash,
				y_values_offset + i * ACC_VALUE_SIZE,
				&buf[1],
				ACC_VALUE_SIZE);
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

	acc_test();

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

	gyro_calculate_offset();
	acc_calculate_offset();

	/* Sensors are initialized */
	BSP_LED_On(LED10);

	speed_test();

	return 0;
}
