/**
 * @file
 * @brief Simple motor test
 *
 * @details The example demonstrates working with motor connected to the port A
 *          NXT brick. It contains both working with function #sleep (motor run
 *          for predefined period and controlling through tachometer circles.
 *
 * @date 25.11.10
 * @author Anton Kozlov
 */

#include <framework/example/self.h>   /* example declaration */
#include <drivers/nxt/motor.h>        /* NXT API interface */

/*
 * Declares an example. You should do this to execute the examples from the
 * system
 */
EMBOX_EXAMPLE(motor_run);

/*
 * Declares flag showing end of motor work.
 * we check it to know when motor is stopped in callback handler if we use
 * controlling motor by tachometer.
 */
static volatile int finish;

/*
 * Callback handler
 * This handler set in the nxt_motor_tacho_set_counter function and call
 * when motor did defined circle number.
 */
static void motor_stop(void) {
	/* switch off motor */
	nxt_motor_set_power(NXT_MOTOR_A, NXT_MOTOR_POWER_OFF);

	/* set flag to say that we already finished */
	finish = 0;
}

/*
 * Demonstrates controlling a motor through tachometer sensor.
 */
static void motor_tachometer(void) {
	/* setup number of motor circle and callback handler which will call when
	 * the motor would the counter circle became zero.
	 */
	nxt_motor_tacho_set_counter(NXT_MOTOR_A, 360, motor_stop);

	/* switch on the motor (MAX_POWER) */
	nxt_motor_set_power(NXT_MOTOR_A, NXT_MOTOR_POWER_MAX);

	/* setup flag */
	finish = 1;

	/*wait till callback handler (motor_stop) be fulfilled */
	while (1 == finish) {
	}

}

/*
 * Demonstrate motor's work for specified time.
 */
static void motor_period(void) {
	nxt_motor_set_power(NXT_MOTOR_A, NXT_MOTOR_POWER_MAX);
	sleep(5);
	nxt_motor_set_power(NXT_MOTOR_A, NXT_MOTOR_POWER_OFF);
}

/* The example entry point */
static int motor_run(int argc, char **argv) {

	/* execute demo with work for determined time period */
	motor_period();

	/* separate demos */
	sleep(3);

	/* execute demo with work for determined tachometer circles */
	motor_tachometer();

	return 0;
}
