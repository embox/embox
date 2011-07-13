/**
 * @file
 * @brief Light sensor example
 * @details example demonstrates using light sensor to print lightness in 0% - 100% range
 * @note view sensor example first
 * @date 05.11.10
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/example.h>
#include <unistd.h>
#include <drivers/nxt/buttons.h>

#include <drivers/nxt/sensor.h>

EMBOX_EXAMPLE(sensor_example);

#define LIGHT_PORT NXT_SENSOR_1

int volatile percent = 0;

/* still, do not print percents here */
/* this handler is primarily for immediate reaction on
/* environment change */

void sensor_handler(nxt_sensor_t *sensor, sensor_val_t val) {
	percent = (val * (100.0 / 1024.0));
}

static int sensor_example(void) {
	nxt_buttons_mask_t buts = 0;
	/* configuring sensor just as passive */
	nxt_sensor_conf_pass(LIGHT_PORT, (sensor_handler_t) sensor_handler);

	while (!(( buts = nxt_buttons_pressed()) & NXT_BOTTON_DOWN)) {
		printf("Lightness is %d%%\n", percent);

		usleep(1000);
	}

	return 0;
}
