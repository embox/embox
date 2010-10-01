/**
 * @file arch.c
 * @brief ARM basic arch work
 *
 * @date 21.06.2010
 * @author Anton Kozlov
 */

#include <hal/reg.h>
#include <drivers/watchdog.h>

#include <drivers/at91sam7s256.h>

static void initialize_main_clock() {
	REG_STORE(AT91C_CKGR_MOR, AT91C_CKGR_MOSCEN | 6 << 8 );
	while (! (REG_LOAD(AT91C_PMC_SR) | AT91C_PMC_MOSCS));

	REG_STORE(AT91C_CKGR_PLLR, (CONFIG_SYS_CLK_MUL << AT91C_CKGR_MUL_OFFSET) | \
							   (CONFIG_SYS_CLK_DIV << AT91C_CKGR_DIV_OFFSET) | \
							   (0x1c << AT91C_CKGR_PLLCOUNT_OFFSET));
	while (! (REG_LOAD(AT91C_PMC_SR) | AT91C_PMC_LOCK));

	REG_STORE(AT91C_PMC_MCKR, AT91C_PMC_PRES_CLK_2);
	while (! (REG_LOAD(AT91C_PMC_SR) | AT91C_PMC_MCKRDY));

	REG_STORE(AT91C_PMC_MCKR, AT91C_PMC_CSS_PLL_CLK | AT91C_PMC_PRES_CLK_2);
	while (! (REG_LOAD(AT91C_PMC_SR) | AT91C_PMC_MCKRDY));

}

void arch_init(void) {
	watchdog_disable();
	initialize_main_clock();
}

void arch_idle(void) {
}

void arch_shutdown(void) {
	while(1);
}
