#include <stdio.h>
#include <unistd.h>
#include <config/custom_config_qspi.h>
#include "sys_clock_mgr.h"
#include "sys_power_mgr.h"

extern bool goto_deepsleep(void);

#define LPC_SETTLE_TIME   (1000 * 1000)

__RETAINED_CODE void deepsleep_test(void) {
	bool entered_sleep = 0;

	printf("Init power manager, preparing to deepsleep\n");

	pm_set_sys_wakeup_mode(pm_sys_wakeup_mode_fast);
	pm_prepare_sleep(pm_mode_extended_sleep);

#if 0
	/* Here we go to deep sleep. printf() is not working from this place. */
	entered_sleep = goto_deepsleep();
#else
	while(1) {
	__asm__ __volatile__ ("wfi");
	}
#endif
	pm_resume_from_sleep();

	printf("Deep sleep: %d\n", entered_sleep ? "OK" : "ERROR");
}

int main(int argc, char **argv) {
	/* Low power clock */
	printf("Init low power clock\n");
	hw_clk_enable_lpclk(LP_CLK_IS_RC32K);
	hw_clk_set_lpclk(LP_CLK_IS_RC32K);

	usleep(LPC_SETTLE_TIME);

	while(1) {
	deepsleep_test();
	}
	return 0;
}
