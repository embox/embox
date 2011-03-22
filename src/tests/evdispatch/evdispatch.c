/**
 * @brief
 * 		Simple test for event dispatch
 *
 * @date 20.03.2011
 * @author Kirill Tyushev
 */

#include <embox/test.h>
#include <kernel/evdispatch.h>
#include <kernel/messages_defs.h>
#include <stdio.h>

EMBOX_TEST(run);

void handler1(struct msg* msg) {
	TRACE("handler1 start\n");
	send_message(MESSAGE2, NULL);
	TRACE("handler1 finish\n");
}

void handler2(struct msg* msg) {
	TRACE("handler2 start\n");
	send_message(MESSAGE3, NULL);
	TRACE("handler2 finish\n");
}

void handler3(struct msg* msg) {
	TRACE("handler3 start\n");
	TRACE("handler3 finish\n");
}

static int run(void) {
	register_handler(MESSAGE1, handler1);
	register_handler(MESSAGE2, handler2);
	register_handler(MESSAGE3, handler3);

	send_message(MESSAGE1, NULL);

	return 0;
}
