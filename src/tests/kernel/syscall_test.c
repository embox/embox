/**
 * @file
 * @brief
 *
 * @date 06.11.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <kernel/syscall_caller.h>

EMBOX_TEST_SUITE("syscall test");

SYSCALL0(0,int,syscall_0);
SYSCALL1(1,int,syscall_1,int,arg1);
SYSCALL2(2,int,syscall_2,int,arg1,int,arg2);
SYSCALL3(3,int,syscall_3,int,arg1,int,arg2,int,arg3);
SYSCALL4(4,int,syscall_4,int,arg1,int,arg2,int,arg3,int,arg4);
SYSCALL5(5,int,syscall_5,int,arg1,int,arg2,int,arg3,int,arg4,int,arg5);
SYSCALL6(6,int,syscall_6,int,arg1,int,arg2,int,arg3,int,arg4,int,arg5,int,arg6);

TEST_CASE("calling syscalls with different number of arguments") {
	test_assert(syscall_0() == 1);
	test_assert(syscall_1(1) == 1);
	test_assert(syscall_2(0,1) == 1);
	test_assert(syscall_3(0,0,1) == 1);
	test_assert(syscall_4(0,0,0,1) == 1);
	test_assert(syscall_5(0,0,0,0,1) == 1);
	test_assert(syscall_6(0,0,0,0,0,1) == 1);
}
