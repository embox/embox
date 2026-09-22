/**
 * @file
 * @brief Watchdog and SoC reset through the BCM283x/BCM2711 PM block
 *
 * There is no "reset now" register on this SoC: a reset is the watchdog
 * firing immediately. One driver covers both, and platform_shutdown() is a
 * watchdog armed with the shortest count that still lets the write land.
 *
 * Every write to the PM block needs the password 0x5a in the top byte, or it
 * is discarded silently. The counter runs at 65536 Hz and is 20 bits wide,
 * so the longest timeout it holds is about 15.9 seconds.
 */

#include <compiler.h>
#include <stdint.h>

#include <drivers/common/memory.h>
#include <drivers/watchdog.h>
#include <framework/mod/options.h>
#include <hal/mem_barriers.h>
#include <hal/platform.h>
#include <hal/reg.h>
#include <util/log.h>

#define PM_BASE ((uintptr_t)OPTION_GET(NUMBER, base_addr))
#define PM_RSTC (PM_BASE + 0x1c)
#define PM_WDOG (PM_BASE + 0x24)

#define PM_PASSWORD              0x5a000000u
#define PM_RSTC_WRCFG_CLR        0xffffffcfu
#define PM_RSTC_WRCFG_FULL_RESET 0x00000020u
#define PM_RSTC_RESET            0x00000102u /* the register's reset value */
#define PM_WDOG_MASK             0x000fffffu

#define PM_WDOG_HZ  65536u
#define PM_WDOG_MAX (PM_WDOG_MASK * 1000u / PM_WDOG_HZ)

#define TIMEOUT_MS OPTION_GET(NUMBER, timeout_ms)

PERIPH_MEMORY_DEFINE(bcm2835_pm, PM_BASE, 0x100);

static void wdog_start(uint32_t ticks) {
	uint32_t rstc;

	if (ticks < 1) {
		ticks = 1;
	}
	dmb(sy);
	REG32_STORE(PM_WDOG, PM_PASSWORD | (ticks & PM_WDOG_MASK));
	rstc = REG32_LOAD(PM_RSTC) & PM_RSTC_WRCFG_CLR;
	REG32_STORE(PM_RSTC, PM_PASSWORD | rstc | PM_RSTC_WRCFG_FULL_RESET);
	dsb(sy);
}

void watchdog_enable(void) {
	uint32_t ms = TIMEOUT_MS;

	if (ms > PM_WDOG_MAX) {
		log_warning("%u ms exceeds the %u ms the counter holds; arming at "
		            "the maximum",
		    ms, (unsigned)PM_WDOG_MAX);
		ms = PM_WDOG_MAX;
	}
	wdog_start((uint32_t)(((uint64_t)ms * PM_WDOG_HZ) / 1000u));
}

void watchdog_disable(void) {
	dmb(sy);
	REG32_STORE(PM_RSTC, PM_PASSWORD | PM_RSTC_RESET);
	dsb(sy);
}

void watchdog_restart(void) {
	watchdog_enable();
}

/* Nothing here cuts the power, so a halt is a spin and everything else is a
 * reset. Ten ticks is ~150 us: long enough for the write to land. */
void _NORETURN platform_shutdown(shutdown_mode_t mode) {
	if (mode != SHUTDOWN_MODE_HALT) {
		wdog_start(10);
	}
	while (1) {}
}
