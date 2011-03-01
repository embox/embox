/**
 * @file
 *
 * @brief Embox test framework implementation.
 *
 * @date Dec 4, 2008
 * @author Anton Bondarev
 *         - Initial implementation
 * @author Eldar Abusalimov
 *         - Reworking handler search algorithm
 * @author Alexey Fomin
 *         - Adding level implementation code
 */

#include <test/framework.h>

#include <stddef.h>
#include <errno.h>
#include <stdio.h>

#include <util/array.h>
#include <mod/core.h>

static int test_mod_enable(struct mod *mod);

const struct mod_ops __test_mod_ops = {
	.enable = &test_mod_enable,
};

ARRAY_SPREAD_DEF(const struct test, __test_registry);

static int test_mod_enable(struct mod *mod) {
	return test_invoke((struct test *) mod_data(mod));
}

int test_invoke(const struct test *test) {
	int result;

	if (NULL == test) {
		return -EINVAL;
	}
	if (NULL == test->run) {
		return -EBADF;
	}

	TRACE("test: running %s: ", test->mod->name);
	if (0 == (result = test->run())) {
		TRACE("passed\n");
	} else {
#if 0
		struct test_failure *failure = (struct test_failure *) result;
		TRACE("failed: %s (0x%08x), at %s : %d, in function %s\n",
				failure->info->reason, (unsigned int) failure->info->data,
				failure->file, failure->line, failure->func);
#else
		TRACE("failed\n");
#endif
	}

	return (test->private->result = result);
}

