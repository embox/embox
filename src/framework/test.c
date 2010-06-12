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
#include <string.h>

#include <embox/kernel.h>
#include <embox/mod.h>
#include <test/framework.h>

static int test_mod_enable(struct mod *mod);
static int test_mod_invoke(struct mod *mod, void *data);

const struct mod_ops __test_mod_ops = {
	.enable = &test_mod_enable,
	.invoke = &test_mod_invoke,
};

MOD_TAG_DEF(test, "test");

static int test_mod_enable(struct mod *mod) {
	return test_mod_invoke(mod, NULL);
}

static int test_mod_invoke(struct mod *mod, void *data) {
	return test_invoke((struct test *) mod_data(mod));
}

int test_invoke(struct test *test) {
	int result;

	if (NULL == test) {
		return -EINVAL;
	}
	if (NULL == test->run) {
		return -EBADF;
	}

	TRACE("test: running %s: ", test->name);
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

