/**
 * @file
 *
 * @brief
 *
 * @date 07.12.2011
 * @author Anton Bondarev
 */

#include <embox/test.h>

#include <signal.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>

#include <kernel/task.h>
#include <kernel/time/ktime.h>

EMBOX_TEST_SUITE("test for task API");

static volatile char flag = 0;
static volatile char flag2 = 0;

static void sig_hnd(int sig) {
	flag = 1;
	task_exit(NULL);
}

static void *task_hnd(void *arg) {

	signal(9, sig_hnd);

	while(1)
		sleep(0);

	return NULL;
}

TEST_CASE("create task and send signal") {
	int tid = new_task("", task_hnd, NULL);
	ksleep(100);
	kill(tid, 9);
	ksleep(100);
	test_assert(flag != 0);

}

static void sig_hnd2(int sig) {
	if (sig == 9) {
		task_exit(NULL);
	} else {
		flag2 ++;
	}
}

static void *task_hnd2(void *arg) {

	signal(9, sig_hnd2);
	signal(1, sig_hnd2);

	while(1)
		sleep(0);

	return NULL;
}

TEST_CASE("create task and send him signal 3 times") {
	int tid = new_task("", task_hnd2, NULL);
	flag2 = 0;

	ksleep(100);
	kill(tid, 1);

	ksleep(100);
	kill(tid, 1);

	ksleep(100);
	kill(tid, 9);

	test_assert(flag2 == 2);

}

static void *thread_hnd(void *arg) {

	ksleep(1000);

	task_exit(NULL);

	return NULL;
}

static void *task_hnd_thread(void *arg) {
	struct thread *thd;

	pthread_create(&thd, 0, thread_hnd, NULL);

	return NULL;
}

TEST_CASE("create a task with 2 threads, kill it") {
	 new_task("", task_hnd_thread, NULL);
}
