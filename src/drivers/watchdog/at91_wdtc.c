/**
 * @file
 * @brief AT91 watchdog driver.
 *
 * @date 29.09.10
 * @author Anton Kozlov
 */

#include <embox/unit.h>
#include <drivers/watchdog.h>
#include <hal/reg.h>
#include <drivers/at91sam7s256.h>

EMBOX_UNIT_INIT(unit_init);

static int unit_init(void) {
	watchdog_disable();
	return 0;
}

void watchdog_enable(void) {
	REG_ANDIN(AT91C_WDTC_WDMR, ~(AT91C_WDTC_WDDIS));
}

void watchdog_disable(void) {
	REG_STORE(AT91C_WDTC_WDMR, AT91C_WDTC_WDDIS);
}

void watchdog_restart(void) {
	REG_STORE(AT91C_WDTC_WDCR, AT91C_WDTC_WDRSTT | AT91C_WDTC_KEY);
}
