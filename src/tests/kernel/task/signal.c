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

EMBOX_TEST_SUITE("test for task API");

static volatile char flag = 0;

extern void kill(int tid, int sig);

extern void signal(int sig, void (*hnd)(int));

static void sig_hnd(int sig) {
	flag = 1;
	task_exit(0);
}

static void *task_hnd(void *arg) {

	signal(9, sig_hnd);

	while(1);

	return NULL;
}

TEST_CASE("create task and send signal") {
	int tid = new_task(task_hnd, NULL);
	usleep(100);
	kill(tid, 9);
	usleep(100);
	test_assert(flag != 0);

}
