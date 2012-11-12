/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <kernel/syscall.h>

#define SYSCALL_NRS_TOTAL 6

static int sys_0(void) {
	return 0;
}

static int sys_1(int arg1) {
	return arg1;
}

static int sys_2(int arg1, int arg2) {
	return arg2;
}

static int sys_3(int arg1, int arg2, int arg3) {
	return arg3;
}

static int sys_4(int arg1, int arg2, int arg3, int arg4) {
	return arg4;
}

static int sys_5(int arg1, int arg2, int arg3, int arg4, int arg5) {
	return arg5;
}

void *const SYSCALL_TABLE[SYSCALL_NRS_TOTAL] = {
	sys_0,
	sys_1,
	sys_2,
	sys_3,
	sys_4,
	sys_5,
};
