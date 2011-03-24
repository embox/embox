/**
 * @brief
 * 		Simple test for event dispatch
 *
 * @date 20.03.2011
 * @author Kirill Tyushev
 */

#include <embox/test.h>
#include <kernel/evdispatch.h>
#include <kernel/softirq.h>

EMBOX_TEST(run);

#define EVENT_SOFTIRQ 12

static void handler1(struct event_msg* msg) {
	event_send(EVENT_MESSAGE2, NULL);
}

static void handler2(struct event_msg* msg) {
	event_send(EVENT_MESSAGE3, NULL);
}

static void handler3(struct event_msg* msg) {
}

static int run(void) {
	softirq_install(EVENT_SOFTIRQ, event_dispatch, NULL);
	softirq_raise(EVENT_SOFTIRQ);

	event_register(EVENT_MESSAGE1, handler1);
	event_register(EVENT_MESSAGE2, handler2);
	event_register(EVENT_MESSAGE3, handler3);

	event_send(EVENT_MESSAGE1, NULL);

	return 0;
}
