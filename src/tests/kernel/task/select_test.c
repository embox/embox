/**
 * @file
 *
 * @brief Select test
 *
 * @date 10.10.2012
 * @author Alexander Kalmuk
 */

#include <embox/test.h>
#include <unistd.h>
#include <string.h>
#include <kernel/task.h>
#include <kernel/thread/sched.h>
#include <sys/select.h>
#include <prom/prom_printf.h>
#include <math.h>

EMBOX_TEST_SUITE("select test");

static int pipefd1[2], pipefd2[2];

static void *select_hnd(void *arg) {
	int cnt;
	fd_set readfds;
	int nfds = max(pipefd1[0], pipefd2[0]) + 1;

	FD_ZERO(&readfds);
	FD_SET(pipefd1[0], &readfds);
	FD_SET(pipefd2[0], &readfds);

	cnt = select(nfds, &readfds, NULL, NULL, NULL);

	assert(cnt == 1 && FD_ISSET(pipefd1[0], &readfds) && !FD_ISSET(pipefd2[0], &readfds));

	return NULL;
}

static void *main_hnd(void *arg) {
	unsigned char a = 'a';

	pipe(pipefd1);
	pipe(pipefd2);

	/* Create new task and sleep for 5 seconds to catch on sleeping in select
	 * in created task */
	new_task(select_hnd, NULL);
	sleep(5);

	/* Write something in pipe. In other task we will wake up and check
	 * witch ends of pipe are active */
	write(pipefd1[1], &a, 1);

	return NULL;
}

TEST_CASE("select on pipe ends") {
	new_task(main_hnd, NULL);
}
