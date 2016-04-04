/**
 * @file servo.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

#ifndef PLATFORM_AGENTS_SERVO_H_
#define PLATFORM_AGENTS_SERVO_H_

/**
 * @brief Should be called before using servo usage
 */
extern int servo_init(void);

/**
 * @brief Sets servo in some position. Currently argument
 * is not an angle, but in future it would be from 0 to 180
 * degrees
 */
extern int servo_set(int pos);

#endif /* PLATFORM_AGENTS_SERVO_H_ */
