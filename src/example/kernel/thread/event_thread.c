/**
 * @file
 *
 * @brief
 *
 * @date 15.01.2013
 * @author Lapshin Alexander
 */
#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>
#include <kernel/thread.h>
#include <kernel/event.h>

#include <util/macro.h>

/**
 * This macro is used to register this example at the system.
 */
EMBOX_EXAMPLE(run);

/** The thread handler function*/
static void *thread_run(void *arg) {
	struct event *event = (struct event *)arg;
	/**waiting until event receives  */
	printf("waiting for event's notifying...\n");
	EVENT_WAIT(event, 0, SCHED_TIMEOUT_INFINITE); /* TODO: event condition */
	printf("event has been received.\n");
	return 0;
}

static int run(int argc, char **argv) {
	struct event sync_event;
	struct thread *thread;

	event_init(&sync_event, "sync_event");
	thread_create(&thread, 0, thread_run, &sync_event);
	/**need some delay here
	 *  because thread should have time to sleep
	 *  within "event_wait" till invoking
	 *  "event_notify" function.
	 */
	sleep(1);
	event_notify(&sync_event);
	thread_join(thread, NULL);

	return ENOERR;
}

