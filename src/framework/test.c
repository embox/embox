/**
 * @file
 *
 * @brief EMBOX test framework implementation.
 *
 * @date Dec 4, 2008
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Reworking handler search algorithm
 * @author Alexey Fomin
 *         - Adding level implementation code
 */

#include <types.h>
#include <errno.h>
#include <common.h>

#include <embox/test.h>
#include <embox/mod.h>

static int test_mod_invoke(void *data);

struct mod_ops __test_mod_ops = { .enable = &test_mod_invoke,
		.invoke = &test_mod_invoke };

MOD_DEF(__test_tag, generic, "test");

static int test_mod_invoke(void *data) {
	int result;
	struct test *test = (struct test *) data;

	if (NULL == test->run) {
		return -EBADF;
	}

	TRACE("test: running %s: ", test->name);
	if (0 == (result = test->run())) {
		TRACE("passed\n");
	} else {
		TRACE("failed\n");
	}

	return (test->private->result = result);
}

struct test **test_get_all(void){
	return MOD_PTR(__test_tag)->provides;
}
