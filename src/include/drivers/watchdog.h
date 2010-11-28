/**
 * @file
 * @brief General interface for watchdog
 *
 * @date 29.09.2010
 * @author Anton Kozlov
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

/* Init watchdog */
void watchdog_init(void);

/* Enable watchdog */
void watchdog_enable(void);

/* Disable watchdog */
void watchdog_disable(void);

/* Keep-a-live */
void watchdog_restart(void);

#endif /* WATCHDOG_H_ */

