/**
 * @file
 *
 * @brief
 *
 * @date 07.12.2011
 * @author Anton Bondarev
 */

#include <unistd.h>
#include <embox/test.h>
#include <kernel/task.h>

EMBOX_TEST_SUITE("test for task API");

static volatile char flag = 0;

extern void kill(int tid, int sig);

extern void signal(int sig, void (*hnd)(int));

static void sig_hnd(int sig) {
	flag = 1;
}

static void *task_hnd(void *arg) {
	int cnt = 5;

	signal(9, sig_hnd);

	while (cnt--) {
		usleep(1000);
	}
	return NULL;
}

TEST_CASE("create task and send signal") {
	int tid = new_task(task_hnd, NULL);
	usleep(2500);
	kill(tid, 9);
	usleep(10000);
	test_assert(flag != 0);

}
