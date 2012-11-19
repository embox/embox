/**
 * @file
 * @brief
 *
 * @date 20.11.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <kernel/usermode.h>
#include <kernel/syscall_caller.h>

EMBOX_TEST_SUITE("usermode test");

SYSCALL1(7,int,syscall_thread_exit,int,exitcode);

void usermode_function(void) {
	for (int i = 0; i < 100000; i++) {

	}

	syscall_thread_exit(0);
}

TEST_CASE("usermode") {
	struct thread *t;
	create_usermode_thread(&t, 0, usermode_function, (void *)0x200000UL);
	thread_join(t, NULL);
}
