/**
 * @file
 * @brief Angle sensor interface of nxt
 *
 * @date 30.04.11
 * @author Anton Kozlov
 */

#ifndef NXT_ANGLE_H_
#define NXT_ANGLE_H_

#include <drivers/nxt_sensor.h>

#define NXT_ANGLE_2X_ANGLE_COMM 0x42
#define NXT_ANGLE_1_ANGLE_ADD_COMM 0x43
#define NXT_ANGLE_RPM_HIGH_COMM 0x48
#define NXT_ANGLE_RPM_LOW_COMM 0x49

extern void nxt_angle_init(sensor_t *sensor);

extern uint16_t nxt_angle_get_rpm(sensor_t *sensor);

#endif /* NXT_SONAR_H_ */
