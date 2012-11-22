/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

//#include <kernel/syscall.h>

#define SYSCALL_NRS_TOTAL 10

int sys_0(void) {
	return 0;
}

int sys_1(int arg1) {
	return arg1;
}

int sys_2(int arg1, int arg2) {
	return arg2;
}

int sys_3(int arg1, int arg2, int arg3) {
	return arg3;
}

int sys_4(int arg1, int arg2, int arg3, int arg4) {
	return arg4;
}

int sys_5(int arg1, int arg2, int arg3, int arg4, int arg5) {
	return arg5;
}

#include <hal/ipl.h>
#include <kernel/thread/api.h>
#include <kernel/task.h>

clock_t sys_thread_running_time(void) {
	ipl_enable();
	return thread_get_running_time(thread_self());
}

int sys_thread_exit(void *r) {
	ipl_enable();
	thread_exit(r);
	return 0;
}

#include <stdio.h>

int sys_exit(void *r) {
	ipl_enable();
	printf("Exit task %d with exitcode: %d\n", task_self()->tid, (int) r);
	task_exit(r);
	return 0;
}

extern int sys_fork(void);

void *const SYSCALL_TABLE[SYSCALL_NRS_TOTAL] = {
	sys_0,
	sys_1,
	sys_2,
	sys_3,
	sys_4,
	sys_5,
	sys_thread_running_time,
	sys_thread_exit,
	sys_fork,
	sys_exit,
};
