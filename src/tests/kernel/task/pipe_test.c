/**
 * @file
 *
 * @brief Pipe test
 *
 * @date 12.09.2012
 * @author Alexander Kalmuk
 */

#include <embox/test.h>
#include <unistd.h>
#include <string.h>
#include <kernel/task.h>

EMBOX_TEST_SUITE("pipe_test");

static int tid_read;
static int tid_write;
static int pipefd[2];
static const char *str = "EMBOX PIPE TEST";
static char buf[64];

static void *read_task_hnd(void *arg) {
	int cnt = 0;

	while (read(pipefd[0], buf + cnt, 1) >= 0) {
		cnt++;
	}

	test_assert(strncmp(str, buf, cnt) == 0);

	return NULL;
}

static void *write_task_hnd(void *arg) {
	pipe(pipefd);

	write(pipefd[1], (void*)str, strlen(str));

	tid_write = new_task("", read_task_hnd, NULL);

	return NULL;
}

TEST_CASE("test read/write pipe operations: Running two tasks and link them by pipe") {
	tid_read = new_task("", write_task_hnd, NULL);
}
