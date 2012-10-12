

#include <stdio.h>
#include <embox/test.h>
#include <kernel/task.h>

EMBOX_TEST_SUITE("tasks");

void *tsk_hnd1(void *data) {
	for (int cnt = 1 << 15; cnt > 0; cnt --) {
		printf("a");
	}
	return NULL;
}

void *tsk_hnd2(void *data) {
	for (int cnt = 1 << 15; cnt > 0; cnt --) {
		printf("b");
	}
	return NULL;
}


TEST_CASE("Create two tasks") {
	new_task(tsk_hnd1, NULL, 0);
	new_task(tsk_hnd2, NULL, 0);
}

