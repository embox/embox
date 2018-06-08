/**
 * @file
 *
 * @brief Select test
 *
 * @date 10.10.2012
 * @author Alexander Kalmuk
 */

#include <unistd.h>
#include <string.h>
#include <sys/select.h>

#include <util/math.h>

#include <kernel/task.h>
#include <kernel/sched.h>

#include <embox/test.h>


EMBOX_TEST_SUITE("select test");

struct task_args {
	int pipefd1[2];
	int pipefd2[2];
	int active_desc_cnt;
	fd_set readfds;
	bool select_hnd_exist;
};

static void *select_hnd(void *arg) {
	struct task_args *volatile args = arg;
	int nfds;

	args = arg;
	nfds = max(args->pipefd1[0], args->pipefd2[0]) + 1;

	FD_ZERO(&args->readfds);
	FD_SET(args->pipefd1[0], &args->readfds);
	FD_SET(args->pipefd2[0], &args->readfds);

	/* Select will change active_desc_cnt and readfds. */
	args->active_desc_cnt = select(nfds, &args->readfds, NULL, NULL, NULL);

	args->select_hnd_exist = false;

	return NULL;
}

TEST_CASE("select on pipe ends") {
	struct task_args volatile args;

	args.select_hnd_exist = true;
	/* Create two pipes */
	test_assert(pipe((int *)args.pipefd1) >= 0);
	test_assert(pipe((int *)args.pipefd2) >= 0);

	/* Create new task and sleep for 5 seconds to catch on sleeping in select
	 * in created task */
	new_task("", select_hnd, (void*)&args);

	/* Used to switch to select_hnd */
	sleep(0);

	/* Write something in pipe. In other task we will wake up and check
	 * witch ends of pipe are active */
	write(args.pipefd1[1], "a", 1);

	while (args.select_hnd_exist) {}

	/* Assertions after select */
	test_assert(args.active_desc_cnt == 1);
	test_assert(FD_ISSET(args.pipefd1[0], &args.readfds));
	test_assert(!FD_ISSET(args.pipefd2[0], &args.readfds));

	/* close pipes */
	close(args.pipefd1[0]);
	close(args.pipefd1[1]);
	close(args.pipefd2[0]);
	close(args.pipefd2[1]);
}
