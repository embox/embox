/**
 * @file
 * @brief Tests threads.
 *
 * @date 16.04.2010
 * @author Dmitry Avdyukhin
 */

#include <embox/test.h>
#include <kernel/thread.h>

EMBOX_TEST(run);

static int run() {
	threads_init();
	scheduler_dispatch();
	return 0;
}
