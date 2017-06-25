/**
 * @file
 * @brief Test checks possibility soft trap's calling
 *
 * @date 26.06.09
 * @author Alexander Batyukov
 */

#include <stdint.h>
#include <embox/test.h>

#include <hal/test/traps_core.h>

/**
 *
 */
EMBOX_TEST_SUITE("testset for call soft trap (syscalls)");

#define TEST_SOFT_TRAP_NUMBER 0x10
#define SOFT_TRAP_RETVAL      0x1234

/* soft traps counter - used in simple test case */
static volatile uint32_t soft_traps_couter;
/* test soft_trap handler*/
static int test_handler(uint32_t trap_nr, void *data) {
	(*(volatile uint32_t *) data)++;
	soft_traps_couter++;
	return SOFT_TRAP_RETVAL;
}

/**
 * SIMPLE CASE - just call soft trap and check whether routine would call or not
 */
TEST_CASE("call soft trap and check whether routine would call or not") {
	uint32_t prev_counter = soft_traps_couter;
	traps_env_t prev_env;

	traps_save_env(&prev_env);
	traps_set_env(testtraps_env());

	testtraps_set_handler(TRAP_TYPE_SOFTTRAP, TEST_SOFT_TRAP_NUMBER,
			test_handler);

	testtraps_fire_softtrap(TEST_SOFT_TRAP_NUMBER, NULL);

	traps_restore_env(&prev_env);

	test_assert_equal(prev_counter, (soft_traps_couter - 1));
}
/**
 * RETURN CASE - call soft trap and check its return value
 */
TEST_CASE("call soft trap and check its return value") {
	uint32_t retval;
	traps_env_t prev_env;

	traps_save_env(&prev_env);
	traps_set_env(testtraps_env());

	testtraps_set_handler(TRAP_TYPE_SOFTTRAP, TEST_SOFT_TRAP_NUMBER,
			 test_handler);
	retval = testtraps_fire_softtrap(TEST_SOFT_TRAP_NUMBER, NULL);
	traps_restore_env(&prev_env);

	test_assert_equal(retval, SOFT_TRAP_RETVAL);
}
/**
 * USER_DATA CASE - call soft trap and correct change user data
 */
TEST_CASE("call soft trap and check correct change user data send to routine") {
	traps_env_t prev_env;
	volatile uint32_t user_data;
	uint32_t prev_user_data = user_data;

	traps_save_env(&prev_env);
	traps_set_env(testtraps_env());


	testtraps_set_handler(TRAP_TYPE_SOFTTRAP, TEST_SOFT_TRAP_NUMBER,
			test_handler);
	testtraps_fire_softtrap(TEST_SOFT_TRAP_NUMBER, (void *) &user_data);
	traps_restore_env(&prev_env);

	test_assert_equal(prev_user_data, user_data - 1);
}
