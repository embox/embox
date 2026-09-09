/**
 * @file
 * @brief Pinning a thread from a test case.
 *
 * Its own header and not framework/test/api.h: that one arrives in every
 * suite in the tree through <embox/test.h>, and a declaration put there is a
 * declaration put everywhere.  <kernel/thread.h> in particular drags
 * lib/libds/slist.h in, which collides with sys/queue.h -- both define
 * SLIST_INIT, and src/tests/bsd/queue_test.c includes both.
 */

#ifndef FRAMEWORK_TEST_THREAD_PIN_H_
#define FRAMEWORK_TEST_THREAD_PIN_H_

#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>
#include <kernel/thread.h>

/**
 * put a thread on the core the suite is running on.
 *
 * A test that asserts an exact sequence -- "abcde", the emissions of three
 * threads of three priorities -- is asserting that the threads TAKE TURNS.
 * With more than one core and no affinity they do not take turns, they run at
 * the same time, and the sequence is whatever the machine produced. That is
 * not a defect in the scheduler and not something the case was written to
 * find; it is the case asking a question that has no answer in that
 * configuration.
 *
 * So an ordering case pins what it orders. The mirror of it is just as
 * important and has its own scar: a case that measures whether the scheduler
 * SPREADS work must not pin anything, or it measures nothing.
 *
 * A no-op on a kernel built without SMP.
 */
static inline void test_thread_pin(struct thread *t) {
#ifdef SMP
	cpu_bind(cpu_get_id(), t);
#else
	(void)t;
#endif
}

#endif /* FRAMEWORK_TEST_THREAD_PIN_H_ */
