/**
 * @file angle.c
 * @brief NXT angle sensor driver
 *
 * @date 30.04.11
 * @author Anton Kozlov
 */

#include <types.h>
#include <drivers/nxt/angle_sensor.h>
#include <drivers/nxt/sensor.h>

void nxt_angle_sensor_init (nxt_sensor_t *sensor) {
	sensor->def_comm = NXT_ANGLE_COMMAND_2X_ANGLE;
	nxt_sensor_conf_active(sensor);
}

uint16_t nxt_angle_get_rpm(nxt_sensor_t *sensor) {
	uint8_t high = nxt_sensor_active_get_val(sensor, NXT_ANGLE_COMMAND_RPM_HIGH);
	uint8_t low = nxt_sensor_active_get_val(sensor, NXT_ANGLE_COMMAND_RPM_LOW);
	return (high << 8) + low;
}
