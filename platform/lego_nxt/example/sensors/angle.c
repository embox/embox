/**
 * @file
 * @brief angle sensor example
 * @details example demonstrates using angle sensor
 * @note view sonar example first
 * @date 12.07.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <embox/example.h>
#include <unistd.h>
#include <drivers/nxt/buttons.h>

#include <drivers/nxt/angle_sensor.h>

EMBOX_EXAMPLE(angle_example);

static int angle(void) {
	int buts;

	nxt_angle_init(NXT_SENSOR_1);

	while (!(( buts = nxt_buttons_pressed()) & NXT_BUTTON_DOWN)) {
		/* get angle value divided by 2 -- this is default value of
		   sensor_get_val. Full equivalent of
		   nxt_sensor_active_get_val(NXT_SENSOR_1, NXT_ANGLE_COMMAND_2DIV_ANGLE) */
		int div2_angle = nxt_sensor_get_val(NXT_SENSOR_1);
		/* get remainder of angle value */
		int add1_angle = nxt_sensor_active_get_val(NXT_SENSOR_1,
				NXT_ANGLE_COMMAND_1_ANGLE_ADD);

		/* print full angle and Rotates Per Minute */
		printf("Angle sensor: %d degrees, %d RPM\n", div2_angle * 2 + add1_angle,
				nxt_angle_get_rpm(NXT_SENSOR_1));
		/* nxt_angle_get_rpm just asks for NXT_ANGLE_COMMAND_RPM_HIGH (high),
		 NXT_ANGLE_COMMAND_RPM_LOW (low byte) and does some calcs
		 ((high << 8) | low). In general, it's like what
		 we just have done */
		usleep(1000);
	}
	return 0;
}
