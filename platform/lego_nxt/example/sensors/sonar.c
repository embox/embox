/**
 * @file
 * @brief Sonar test
 *
 * @date 22.12.10
 * @author Anton Kozlov
 */

#include <drivers/soft_i2c.h>
#include <drivers/nxt/buttons.h>
#include <drivers/nxt/sonar_sensor.h>
#include <embox/example.h>
#include <drivers/at91sam7s256.h>
#include <unistd.h>

EMBOX_EXAMPLE(sonar_example);


static int sonar_test(void) {
	int buts;
	/* init sensor as sonar sensor */
	nxt_sonar_init(NXT_SENSOR_1);
	/* please refer to sensor example */
	while (!(( buts = nxt_buttons_pressed()) & NXT_BOTTON_DOWN)) {
		/* note, after configuring there is no difference between active and passive sensors */
		printf("%d\n", nxt_sensor_get_val(NXT_SENSOR_1));

		usleep(1000);
	}
	return 0;
}
