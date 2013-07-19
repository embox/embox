/**
 * @file
 *
 */

#include <embox/test.h>

#include <kernel/sched/sched_strategy.h>
#include <kernel/thread.h>

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
