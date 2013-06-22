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
#include <kernel/sched.h>
#include <sys/select.h>
#include <math.h>

EMBOX_TEST_SUITE("select test");

static int pipefd1[2], pipefd2[2];
static int active_desc_cnt;
static fd_set readfds;
static bool select_hnd_exist = true;

static void *select_hnd(void *arg) {
	int nfds = max(pipefd1[0], pipefd2[0]) + 1;

	FD_ZERO(&readfds);
	FD_SET(pipefd1[0], &readfds);
	FD_SET(pipefd2[0], &readfds);

	/* Select will change active_desc_cnt and readfds. */
	active_desc_cnt = select(nfds, &readfds, NULL, NULL, NULL);

	select_hnd_exist = false;

	return NULL;
}

TEST_CASE("select on pipe ends") {
	/* Create two pipes */
	assert(pipe(pipefd1) >= 0);
	assert(pipe(pipefd2) >= 0);

	/* Create new task and sleep for 5 seconds to catch on sleeping in select
	 * in created task */
	new_task("", select_hnd, &readfds);

	/* Used to switch to select_hnd */
	sleep(0);

	/* Write something in pipe. In other task we will wake up and check
	 * witch ends of pipe are active */
	write(pipefd1[1], "a", 1);

	while (select_hnd_exist) {}

	/* Assertions after select */
	test_assert(active_desc_cnt == 1);
	test_assert(FD_ISSET(pipefd1[0], &readfds));
	test_assert(!FD_ISSET(pipefd2[0], &readfds));

	/* close pipes */
	close(pipefd1[0]);
	close(pipefd1[1]);
	close(pipefd2[0]);
	close(pipefd2[1]);
}
