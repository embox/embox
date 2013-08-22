/**
 * @file
 *
 * @brief Tests changing thread's state by event.
 *
 * @date 18.12.2011
 * @author Anton Bondarev
 */

#include <unistd.h>

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <err.h>
#include <kernel/event.h>
#include <kernel/time/ktime.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("test for change thread state by events");

static volatile int condition = 0;

static void *thread_run(void *arg) {
	struct event *event = (struct event *) arg;

	test_assert_zero(EVENT_WAIT(event, condition, EVENT_TIMEOUT_INFINITE));

	return 0;
}

TEST_CASE("Create thread waiting event and then finish") {
	struct event event;
	struct thread *thread;

	event_init(&event, "test_event");

	thread = thread_create(0, thread_run, &event);

	test_assert_zero(err(thread));

	ksleep(100);

	condition = 1;
	event_notify(&event);

	test_assert_zero(thread_join(thread, NULL));
}

TEST_CASE("Checking timeout of event") {
	struct event event;

	event_init(&event, "test_event");
	test_assert_equal(EVENT_WAIT(&event, 0, 100), -ETIMEDOUT);
}
