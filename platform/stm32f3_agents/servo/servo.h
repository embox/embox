/**
 * @file servo.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-04-04
 */

#ifndef PLATFORM_AGENTS_SERVO_H_
#define PLATFORM_AGENTS_SERVO_H_

/* Most servos has three wires. Actually they are:
 *     Black   for ground
 *     Red     for power supply
 *     White   for servo control
 *
 * For black wire it's quite simple: just connect it to any GND pin.
 *
 * Red wire can be connected for both 3V or 5V pins, and 5V is a better choice.
 * In case of 3V servo will be slower and will hold itself the way worse.
 *
 * White wire can be connected to any TIMx_CHx pin. For default configuration
 * it's PD4 (TIM2 channel 2).
 *
 * The constants from timer/pwm configuration comes from following:
 *   Most servos has 50 Hz frequency, so we have to do control output like this.
 *
 *          ^
 * Control  |__      __      __
 *  output  |  |    |  |    |  |
 *          |  |    |  |    |  |
 *           ------------------------->
 *
 *          | 20 ms | 20 ms | 20 ms |
 *
 *     (1 sec / 50 = 20 ms)
 *
 *  The wider impulse is, the higher degree of servo will be.
 *  Yes, it's as simple as that: use configure PWM to ouput fixed with of the
 *  signal, so the servo drives to the required position on it's own.
 *
 */

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
