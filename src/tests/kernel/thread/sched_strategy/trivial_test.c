/**
 * @file
 *
 */

#include <embox/test.h>

#include <kernel/thread/sched_strategy.h>
#include <kernel/thread/api.h>

struct runq runq;
struct thread idle, current;

EMBOX_TEST_SUITE("Trivial scheduler strategy test");

TEST_SETUP(setup);

TEST_CASE("empty test") {

}

static int setup(void) {
	runq_init(&runq, &current, &idle);
	return 0;
}
