/**
 * @file
 * @brief Test for pendulum with angle sensor
 *        It must be better then early tests
 *
 * @date 05.11.10
 * @author Alexandr Kalmuk
 */


#include <embox/test.h>
#include <drivers/nxt_avr.h>
#include <drivers/pins.h>
#include <unistd.h>
#include <drivers/nxt_sensor.h>
#include <drivers/nxt_avr.h>
#include <drivers/nxt_motor.h>


#define MOTOR_POWER 100
#define MOTOR0 (&motors[0])
#define MOTOR1 (&motors[1])

#define MAX_POWER
#define KAPPA
#define TAU
#define H_
#define EPSILON 1

double prev_angle;
double prev_time;

EMBOX_TEST(pen_angle);

static double dif_angle(double cur_angle, double cur_time) {
	return (cur_angle - prev_angle) / (cur_time - prev_time);
}

#define TOUCH_PORT (&sensors[0])

sensor_val_t sval = 10;

void sensor_handler(sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

static int pen_angle(void) {

	return 0;;
}
