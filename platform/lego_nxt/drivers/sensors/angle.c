/**
 * @file
 * @brief nxt angle sensor driver
 *
 * @date 30.04.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <drivers/nxt_angle.h>
#include <drivers/nxt_sensor.h>

void nxt_angle_init (sensor_t *sensor) {
	sensor->def_comm = NXT_ANGLE_2X_ANGLE_COMM;
	nxt_sensor_conf_active(sensor);
}

uint16_t nxt_angle_get_rpm(sensor_t *sensor) {
	uint8_t high = nxt_sensor_active_get_val(sensor, NXT_ANGLE_RPM_HIGH_COMM);
	uint8_t low = nxt_sensor_active_get_val(sensor, NXT_ANGLE_RPM_LOW_COMM);
	return (high << 8) + low;
}
