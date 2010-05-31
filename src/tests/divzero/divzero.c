/**
 * @file
 *
 * @date 26.05.2010
 * @author sunnya
 */

#include <types.h>

#include <embox/test.h>
#include <hal/env/traps_core.h>
#include <hal/test/testtraps_core.h>

/* declare test in system */
EMBOX_TEST(run);

#define DIV_BY_ZERO     0x06

/* MMU data access exception handler */
static int dfault_handler(uint32_t trap_nr, void *data) {
	TRACE("\n ]:-> division by zero\n");
	/* skip instruction */
	return 0;
}

static int run() {
	volatile uint32_t a = 17;
	volatile uint32_t zero = 0;

	traps_env_t old_env;

	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, DIV_BY_ZERO, dfault_handler);

	a = (23 / zero);

	traps_restore_env(&old_env);

	TRACE("\t\ta = %d\n", a);
	return a;
}
