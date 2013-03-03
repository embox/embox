/**
 * @file
 * TODO header and printf inside a test case
 * This test must check correct closing a task and free int resources (in, out description for example)
 *
 * @date 28.09.12
 * @author Anton Kozlov
 */

#include <stdio.h>
#include <embox/test.h>
#include <kernel/task.h>

EMBOX_TEST_SUITE("tasks");

static void *tsk_hnd1(void *data) {
	for (int cnt = 1 << 15; cnt > 0; cnt --) {
		printf("a");
	}
	return NULL;
}

static void *tsk_hnd2(void *data) {
	for (int cnt = 1 << 15; cnt > 0; cnt --) {
		printf("b");
	}
	return NULL;
}

TEST_CASE("Create two tasks") {
	new_task("", tsk_hnd1, NULL);
	new_task("", tsk_hnd2, NULL);
}
