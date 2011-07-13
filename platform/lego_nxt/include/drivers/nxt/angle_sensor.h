/**
 * @file angle_sensor.h
 * @brief NXT angle sensor interface
 *
 * @date 30.04.11
 * @author Anton Kozlov
 */

#ifndef NXT_ANGLE_H_
#define NXT_ANGLE_H_

#include <drivers/nxt/sensor.h>

/* commands to be given as 'commands'
 * to nxt_sensor_active_get_val */
#define NXT_ANGLE_COMMAND_2DIV_ANGLE    0x42 /**< angle div by 2 */
#define NXT_ANGLE_COMMAND_1_ANGLE_ADD 0x43 /**< angle mod by 2 */
#define NXT_ANGLE_COMMAND_RPM_HIGH    0x48 /**< high byte of Rotates Per Minutes */
#define NXT_ANGLE_COMMAND_RPM_LOW     0x49 /**< low byte of Rotates Per Minute */

/**
 * Dynamically defines sensor as angle sensor
 * @param sensor Sensor to be defined
 */
extern void nxt_angle_sensor_init(nxt_sensor_t *sensor);

/**
 * Gets Rotates Per Minute from angle sensor
 * @param sensor Sensor to be accessed
 * @return Rotates Per Minute value
 */
extern uint16_t nxt_angle_get_rpm(nxt_sensor_t *sensor);

#endif /* NXT_SONAR_H_ */
