/* @file watchdog.h
 * @brief General interface for watchdog
 *
 * @date 29.09.2010
 * @author Anton Kozlov
 */


/* Init watchdog */
void watchdog_init(void);

/* Enable watchdog */
void watchdog_enable(void);

/* Disable watchdog */
void watchdog_disable(void);

/* Keep-a-live */
void watchdog_restart(void);
