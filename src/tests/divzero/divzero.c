/**
 * @file
 *
 * @date 26.05.10
 * @author Alexander Batyukov, Anton Kozlov
 */

#include <types.h>
#include <embox/test.h>
#include <hal/env/traps_core.h>
#include <hal/test/traps_core.h>

EMBOX_TEST(run);

static volatile uint32_t a = 17;
static int was_in_trap = 0;

/* MMU data access exception handler */
static int dfault_handler(uint32_t trap_nr, void *data) {
	TRACE("\n ]:-> division by zero\n");
	was_in_trap = 1;
	/* skip instruction */
	return 0;
}

static int run(void) {
	volatile uint32_t zero = 0;

	traps_env_t old_env;

	traps_save_env(&old_env);
	traps_set_env(testtraps_env());

	testtraps_set_handler(TRAP_TYPE_HARDTRAP, DIVZERO_FAULT, dfault_handler);

	a = (23 / zero);

	traps_restore_env(&old_env);

	/* TRACE("\t\ta = %d\n", a); */

	return !was_in_trap;
}
