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
#include <test/misc.h>
#include <unistd.h>
#include <stdio.h>

EMBOX_TEST(run);

#define EVENT_SOFTIRQ 10

static int flag;

static void handler1(struct event_msg* msg) {
	*((int*) msg->data) += 1;
	event_send(EVENT_MESSAGE2, msg->data);
}

static void handler2(struct event_msg* msg) {
	*((int*) msg->data) += 1;
	event_send(EVENT_MESSAGE3, msg->data);
}

static void handler3(struct event_msg* msg) {
	*((int*) msg->data) += 1;
}

static int run(void) {
	softirq_install(EVENT_SOFTIRQ, event_dispatch, NULL);
	softirq_raise(EVENT_SOFTIRQ);

	event_register(EVENT_MESSAGE1, handler1);
	event_register(EVENT_MESSAGE2, handler2);
	event_register(EVENT_MESSAGE3, handler3);

	flag = 0;

	event_send(EVENT_MESSAGE1, &flag);

	test_misc_irq_force(EVENT_SOFTIRQ);

	if (flag != 3) {
		TRACE("Bad results: flag = %d\n", flag);
		return 1;
	}

	return 0;
}
