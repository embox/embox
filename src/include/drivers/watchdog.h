/**
 * @file
 * @brief General interface for watchdog.
 *
 * @date 29.09.10
 * @author Anton Kozlov
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/**
 * Enables watchdog.
 */
void watchdog_enable(void);

/**
 * Disables watchdog.
 */
void watchdog_disable(void);

/**
 * Watchdog keep-a-live.
 */
void watchdog_restart(void);

#endif /* WATCHDOG_H_ */
