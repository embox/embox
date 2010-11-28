/**
 * @file
 * @brief AT91 watchdog driver
 *
 * @date 29.09.2010
 * @author Anton Kozlov
 */

#include <drivers/watchdog.h>
#include <hal/reg.h>
#include <drivers/at91sam7s256.h>

/* Init watchdog */
void watchdog_init(void) {
}

/* Enable watchdog */
void watchdog_enable(void) {
	REG_ANDIN(AT91C_WDTC_WDMR, ~(AT91C_WDTC_WDDIS));
}

/* Disable watchdog */
void watchdog_disable(void) {
	REG_STORE(AT91C_WDTC_WDMR, AT91C_WDTC_WDDIS);
}

/* Keep-a-live */
void watchdog_restart(void) {
	REG_STORE(AT91C_WDTC_WDCR, AT91C_WDTC_WDRSTT | AT91C_WDTC_KEY);
}
