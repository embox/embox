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

TEST_CASE("simple one sleep") {
#if 0
	uint32_t cur_time = cnt_system_time();
	uint32_t epsilon;
	usleep(3000);
	epsilon = abs( (int)(cnt_system_time() - cur_time) - (int)3000 );
	test_assert_true(epsilon<10);
#endif
}

