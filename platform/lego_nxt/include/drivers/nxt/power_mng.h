/**
 * @file
 * @brief nxt avr power manager driver
 *
 * @date 29.06.11
 * @author Anton Kozlov
 */

#ifndef POWER_MNG_H_
#define POWER_MNG_H_

/**
 * Sets NXT in halt state, i.e. power-saving mode
 */
extern void nxt_halt(void);

/**
 * Sets NXT in flashing state, i.e. new firmware can be loaded
 */
extern void nxt_flash(void);

#endif /* POWER_MNG_H_ */
