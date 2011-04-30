/**
 * @file
 * @brief nxt sonar driver
 *
 * @date 17.01.11
 * @author Alexander Batyukov
 */

#include <types.h>
#include <drivers/nxt_sonar.h>
#include <drivers/nxt_sensor.h>

void nxt_sonar_init (sensor_t *sensor) {
	sensor->def_comm = NXT_SONAR_DISTANCE_COMM;
	nxt_sensor_conf_active(sensor);
}

