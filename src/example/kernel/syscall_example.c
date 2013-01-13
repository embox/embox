/**
 * @file
 * @brief
 *
 * @date 16.11.2012
 * @author Anton Bulychev
 */

#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>
#include <kernel/thread.h>
#include <kernel/syscall_caller.h>
#include <kernel/printk.h>

EMBOX_EXAMPLE(run);

#define NRS 10000000

SYSCALL1(1,int,syscall_1,int,arg1);
extern int sys_1(int arg1);

SYSCALL0(6,clock_t,syscall_thread_running_time);
SYSCALL1(7,int,syscall_thread_exit,void*,res);

static void *thread_syscall(void *arg) {
	clock_t end;
	clock_t start = syscall_thread_running_time();

	for (int i = 0; i < NRS; i++) {
		syscall_1(i);
	}

	end = syscall_thread_running_time();
	printf("Start: %d. End: %d. Time: %d.\n", (int)start, (int)end, (int)(end-start));
	syscall_thread_exit((void *)(end - start));

	return NULL;
}

static void *thread_nosyscall(void *arg) {
	clock_t end;
	clock_t start = thread_get_running_time(thread_self());

	for (int i = 0; i < NRS; i++) {
		sys_1(i);
	}

	end = thread_get_running_time(thread_self());
	printf("Start: %d. End: %d. Time: %d.\n", (int)start, (int)end, (int)(end-start));
	return (void *)(end - start);
}

static int run(int argc, char **argv) {
	struct thread *thread;
	void *r1, *r2;
	clock_t c1, c2;

	printf("Running syscall thread.\n");
	thread_create(&thread, THREAD_FLAG_USERMODE, thread_syscall, NULL);
	thread_join(thread, &r1);

	printf("Running nosyscall thread.\n");
	thread_create(&thread, 0, thread_nosyscall, NULL);
	thread_join(thread, &r2);

	c1 = (clock_t) r1;
	c2 = (clock_t) r2;

	printf("Total calls: %d. Time diff: %d\n", NRS, (int) (c1-c2));

	return ENOERR;
}
