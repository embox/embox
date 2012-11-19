/**
 * @file
 * @brief
 *
 * @date 20.11.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <kernel/usermode.h>

EMBOX_TEST_SUITE("usermode test");

void usermode_function(void) {

}

TEST_CASE("usermode") {
	struct thread *t;
	create_usermode_thread(&t, 0, usermode_function, (void *)0x20000000UL);
	thread_join(t, NULL);
}
