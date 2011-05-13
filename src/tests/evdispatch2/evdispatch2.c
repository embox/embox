/**
 * @file
 * @brief Simple test for event dispatch
 *
 * @date 28.04.11
 * @author Malkovsky Nikolay
 */

#include <embox/test.h>
#include <kernel/evdispatch2.h>

EMBOX_TEST(run);

static int flag;

static void handler3(void *data) {
	*((int*) data) += 1;
}

static void handler2(void *data) {
	*((int*) data) += 1;
	event_send(handler3, data);
}

static void handler1(void *data) {
	*((int*) data) += 1;
	event_send(handler2, data);
}

static int run(void) {
	flag = 0;

	event_send(handler1, &flag);

	event_dispatch();

	if (flag != 3) {
		return 1;
	}

	return 0;
}
