/**
 * @file
 * @brief Lego NXT passive sensor example
 * @details Prints one raw sensor value in a second before button 'down' will be
 *          not pressed
 *
 * @date 05.11.10
 * @author Anton Kozlov
 */

#include <stdint.h>
#include <unistd.h> /* usleep */
#include <stdio.h>  /* printf */
#include <framework/example/self.h>

#include <drivers/nxt/buttons.h>
#include <drivers/nxt/sensor.h>

/* example declaration routine sensor_example will be called*/
EMBOX_EXAMPLE(sensor_example);

/* we get sensor value in the sensor's handler */
static sensor_val_t volatile sval = 0;

/*
 * do not use print here! this handler calls really often
 * and slow print operation will slow down whole system,
 * late sensor_handler calls will be kept by print
 */
static void sensor_handler(nxt_sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

/* example main routine*/
static void sensor_example_with_callback(void) {
	nxt_buttons_mask_t buts = 0;

	/* Configure sensor as passive, i.e. has only one value in range 0-1023	 */
	/* sensor_handler will be called each time sensor measure environment */
	nxt_sensor_conf_pass(NXT_SENSOR_1, (sensor_handler_t) sensor_handler);

	/* read buttons that was pressed when we slept
	 * exit if DOWN buttons was pressed
	 */
	while (!(( buts = nxt_buttons_pressed()) & NXT_BUTTON_DOWN)) {
		/* print sensor value */
		/* sensor_get_val() == sval (that was set in sensor_handler) */
		/* there is no difference with */
		/* printf("%d\n", nxt_sensor_get_val(NXT_SENSOR_1));
		 */
		 printf("%d\n", sval);

		/* sleep 1 sec */
		usleep(1000);
	}
}

/* example main routine*/
static void sensor_example_without_callback(void) {
	nxt_buttons_mask_t buts = 0;

	/* Configure sensor as passive, i.e. has only one value in range 0-1023	 */
	/* sensor_handler will be called each time sensor measure environment */
	nxt_sensor_conf_pass(NXT_SENSOR_1, NULL);

	/* read buttons that was pressed when we slept
	 * exit if DOWN buttons was pressed
	 */
	while (!(( buts = nxt_buttons_pressed()) & NXT_BUTTON_DOWN)) {
		/* print sensor value */
		/* sensor_get_val() == sval (that was set in sensor_handler) */
		/* there is no difference with */
		/* printf("%d\n", sval); */
		printf("%d\n", nxt_sensor_get_val(NXT_SENSOR_1));
		/* sleep 1 sec */
		usleep(1000);
	}
}


/* example main routine*/
static int sensor_example(int argc, char **argv) {

	sensor_example_with_callback();

	sensor_example_without_callback();

	return 0;
}
