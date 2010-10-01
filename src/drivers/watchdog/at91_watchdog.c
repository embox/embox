/* @file at91_watchdog.c
 * @brief AT91 watchdog driver
 *
 * @date 29.09.2010
 * @author Anton Kozlov
 */

#include <drivers/watchdog.h>
#include <hal/reg.h>
#include <drivers/at91sam7s256.h>

/* Init watchdog */
void watchdog_init() {
}

/* Enable watchdog */
void watchdog_enable() {
	REG_ANDIN(AT91C_WDTC_WDMR, ~(AT91C_WDTC_WDDIS));
}

/* Disable watchdog */
void watchdog_disable() {
	REG_ORIN(AT91C_WDTC_WDMR, ~(AT91C_WDTC_WDDIS));
}

/* Keep-a-live */
void watchdog_restart() {
	REG_STORE(AT91C_WDTC_WDCR, AT91C_WDTC_WDRSTT | AT91C_WDTC_KEY);
}
