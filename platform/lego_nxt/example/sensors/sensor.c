/**
 * @file
 * @brief any passive sensor example
 * @details Prints one raw sensor value in second
 *
 * @date 05.11.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/example.h>
#include <unistd.h>
#include <drivers/nxt/buttons.h>
#include <drivers/pins.h>

#include <drivers/nxt/sensor.h>
#include <drivers/nxt/avr.h>

EMBOX_EXAMPLE(sensor_example);

sensor_val_t volatile sval = 0;

/* do not use print here! this handler calls really often
/* and slow print operation will slow down all system,
/* late sensor_handler calls will be kept by print */

void sensor_handler(nxt_sensor_t *sensor, sensor_val_t val) {
	sval = val;
}

static int sensor_example(void) {
	nxt_buttons_mask_t buts = 0;

	/* Configure sensor as passive, i.e. has only one value in range 0-1023	 */
	/* sensor_handler will be called each time sensor measure environment */
	nxt_sensor_conf_pass(NXT_SENSOR_1, (sensor_handler_t) sensor_handler);

	/* read buttons that was pressed when we slept */
	/* exit if DOWN buttons was pressed */
	while (!(( buts = nxt_buttons_pressed()) & NXT_BUTTON_DOWN)) {
		/* print sensor value */
		/* sensor_get_val() == sval (that was set in sensor_handler) */
		/* there is no difference with */
		/* printf("%d\n", sval); */
		printf("%d\n", nxt_sensor_get_val(NXT_SENSOR_1));
		/* sleep 1 sec */
		usleep(1000);
	}
	return 0;
}
