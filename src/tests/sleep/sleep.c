/**
 * @file
 * @brief TODO This file is derived from Embox test template.
 *
 * @date
 * @author TODO Your name here
 */

#include <embox/test.h>
#include <unistd.h>
#include <util/math.h>
#include <kernel/timer.h>

EMBOX_TEST_SUITE("sleep suite");

#define EPSILON_BORDER 10
#define TIME_TO_SLEEP 100

/**
 *  sleep( any_time )
 *  assert that real time for sleep different with any_time is less than EPSILON_BORDER
 */
TEST_CASE("one sleep") {
	/* TODO: change uint32_t to system time_t type */
	uint32_t cur_time = cnt_system_time();
	uint32_t epsilon;
	usleep(TIME_TO_SLEEP);
	epsilon = abs( (int)(cnt_system_time() - cur_time) - (int)TIME_TO_SLEEP );
	test_assert_true(epsilon<EPSILON_BORDER);
}

/**
 *  run ~3 threads (~3 times) with 1,2,3 sec wait
 *  assert right done sequence of threads
 */
TEST_CASE("simple multithreaded check") {
}

TEST_CASE("sleep sort") {
	/* without print to screen, using EMIT Buffer */
}

