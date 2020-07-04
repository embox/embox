#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#include <kernel/task.h>

#include <embox/test.h>

EMBOX_TEST_SUITE("atexit() testsuite");

static void bye(void)
{
	test_emit('>');
}

static void *test_atexit_single(void *arg) {
	long a;
	int i;

	a = sysconf(_SC_ATEXIT_MAX);

	for (int c = 0; c < a; c++) {
		i = atexit(bye);
		test_assert_equal(i, 0);
	}

	/* this one should fail */
	i = atexit(bye);
	test_assert_not_equal(i, 0);

	exit(0);
}

TEST_CASE("register ATEXIT_MAX functions with atexit()") {
	int res;

	new_task("", test_atexit_single, NULL);

	wait(&res);

	/* test for atexit_max default value */
	test_assert_emitted(">>>>");
	test_assert_zero(res);
}
