/**
 * @file
 * @brief
 *
 * @date 21.11.2012
 * @author Anton Bulychev
 */

#include <embox/test.h>

#include <stdio.h>

#include <kernel/task.h>

EMBOX_TEST_SUITE("idx inheritance test");

static void *new_routine(void *file) {
	fclose(file);

	return NULL;
}

TEST_CASE("inheritance test") {
	FILE *file = fopen("hosts", "r"); // some file
	test_assert_not_null(file);

	test_assert_not_zero(new_task(new_routine, file));

	fclose(file);
}
