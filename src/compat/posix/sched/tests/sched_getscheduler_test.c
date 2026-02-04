/**
 * @file
 * @brief
 *
 * @author  Anton Bondarev
 * @date    29.01.2026
 */

#include <kernel/task.h>
#include <embox/test.h>
#include <sched.h>

EMBOX_TEST_SUITE("sched_getscheduler test suite");

static void run_wait_task(void *(*fn)(void *), void *arg) {
	pid_t p;

	p = new_task("test_task", fn, NULL);

	while (-EINTR == task_waitpid(p));
}

static void *task_handler_pid0(void *arg) {

	sched_getscheduler(0);
	test_emit('a');
	return NULL;
}

TEST_CASE("sched_getparam with pid==0") {
	run_wait_task(task_handler_pid0, NULL);
	test_assert_emitted("a");
}


