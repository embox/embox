/**
 * @file
 *
 * @brief
 *
 * @date 07.12.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <unistd.h>
#include <embox/test.h>
#include <kernel/task.h>
#include <kernel/thread/api.h>
EMBOX_TEST_SUITE("test for task API");

static volatile char flag = 0;

extern int kill(int tid, int sig);

extern void signal(int sig, void (*hnd)(int));

static void sig_hnd(int sig) {
	flag = 1;
	task_exit(NULL);
}

static void *task_hnd(void *arg) {

	signal(9, sig_hnd);

	while(1);

	return NULL;
}

TEST_CASE("create task and send signal") {
	int tid = new_task(task_hnd, NULL, 0);
	usleep(100);
	kill(tid, 9);
	usleep(100);
	test_assert(flag != 0);

}

static void *thread_hnd(void *arg) {

	usleep(1000);

	task_exit(NULL);

	return NULL;
}

static void *task_hnd_thread(void *arg) {
	struct thread *thd;

	thread_create(&thd, 0, thread_hnd, NULL);

	return NULL;
}

TEST_CASE("create a task with 2 threads, kill it") {
	 new_task(task_hnd_thread, NULL, 0);
}
