/**
 * @file
 * @brief Mutex testing.
 *
 * @date 05.05.10
 * @author Nikolay Malkovsky
 */

#include <kernel/thread/thread.h>
#include <kernel/thread/sched/sched.h>
#include <kernel/thread/sync/mutex.h>
#include <embox/test.h>

EMBOX_TEST(run);

static int run(void) {
	/*
	 * This test simply includes kernel/mutex.h.
	 */
	int result = 0;
	return result;
}
