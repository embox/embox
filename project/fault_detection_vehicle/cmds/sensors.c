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


#include <drivers/sensors/acc.h>
#include <drivers/sensors/gyro.h>
#include <libmisc/led.h>

#include <drivers/gpio.h>
#include <config/board_config.h>

#include <hal/clock.h>
#include <kernel/time/time.h>

#include <filter/kalman_filter.h>


static struct led led3;
static struct led led4;
static struct led led5;
static struct led led6;
static struct led led7;
static struct led led8;
static struct led led9;
static struct led led10;

static float angles[3];
static float speed = 0;
static int inited = 0;

static struct kalman_filter kalman[3];

static void init_leds(void) {
	led_init(&led3, CONF_LED3_GPIO_PORT, CONF_LED3_GPIO_PIN);
	led_init(&led4, CONF_LED4_GPIO_PORT, CONF_LED4_GPIO_PIN);
	led_init(&led5, CONF_LED5_GPIO_PORT, CONF_LED5_GPIO_PIN);
	led_init(&led6, CONF_LED6_GPIO_PORT, CONF_LED6_GPIO_PIN);
	led_init(&led7, CONF_LED7_GPIO_PORT, CONF_LED7_GPIO_PIN);
	led_init(&led8, CONF_LED8_GPIO_PORT, CONF_LED8_GPIO_PIN);
	led_init(&led9, CONF_LED9_GPIO_PORT, CONF_LED9_GPIO_PIN);
	led_init(&led10, CONF_LED10_GPIO_PORT, CONF_LED10_GPIO_PIN);
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

		led_off(&led3);
		led_off(&led4);
		led_off(&led5);
		led_off(&led6);
		led_off(&led7);

		gyro_data_obtain(gyro);
		acc_data_obtain(acc);

		current = jiffies2ms(clock_sys_ticks());
		dt = (current - prev) / 1000.0; /* seconds */
		prev = current;

		update_angles(acc, gyro, dt);

		compensation = 1000 * sin(kalman[1].value) * g;
		update_speed(acc[0] - compensation, dt);

		if (speed > 50) {
			led_off(&led7);
		} else if (speed > 10) {
			led_off(&led5);
		} else if (speed < -50) {
			led_on(&led6);
		} else if (speed < -10) {
			led_on(&led4);
		} else {
			led_on(&led3);
		}

		usleep(10);
	}
}




#if 0
static void gyro_test(void) {
	float buf[3] = {0};
	float x, y;

	for (size_t i = 0; i < 100; i++) {
		gyro_get(buf);

		x = abs(buf[0]);
		y = abs(buf[1]);

		printf("x=%f y=%f z=%f\n", x, y);
		usleep(10);
	}
}
#endif

int main(int argc, char *argv[]) {
	init_leds();

	/* Sensors are initialized */
	led_on(&led10);

	speed_test();

	return 0;
}
