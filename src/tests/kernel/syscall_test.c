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

TEST_CASE("calling syscall without arguments") {
	test_assert_equal(syscall_0(), 0);
}

TEST_CASE("calling syscall with one argument") {
	test_assert_equal(syscall_1(1), 1);
}

TEST_CASE("calling syscall with two arguments") {
	test_assert_equal(syscall_2(1,2), 2);
}

TEST_CASE("calling syscall with three arguments") {
	test_assert_equal(syscall_3(1,2,3), 3);
}

TEST_CASE("calling syscall with four arguments") {
	test_assert_equal(syscall_4(1,2,3,4), 4);
}

TEST_CASE("calling syscall with five arguments") {
	test_assert_equal(syscall_5(1,2,3,4,5), 5);
}
