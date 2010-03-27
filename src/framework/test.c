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
#include <embox/kernel.h>

#include <embox/test.h>
#include <embox/mod.h>

static int test_mod_enable(struct mod *mod);
static int test_mod_invoke(struct mod *mod, void *data);

struct mod_ops __test_mod_ops = { .enable = &test_mod_enable,
		.invoke = &test_mod_invoke };

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
		TRACE("failed\n");
	}

	return (test->private->result = result);
}

struct test_iterator *test_get_all(struct test_iterator *iterator) {
	if (NULL == iterator) {
		return NULL;
	}
	mod_tagged(MOD_TAG_PTR(test), &iterator->mod_iterator);
	return iterator;
}

inline bool test_iterator_has_next(struct test_iterator *iterator) {
	return NULL != iterator && mod_iterator_has_next(&iterator->mod_iterator);
}

struct test *test_iterator_next(struct test_iterator *iterator) {
	if (!test_iterator_has_next(iterator)) {
		return NULL;
	}
	return (struct test *) mod_data(mod_iterator_next(&iterator->mod_iterator));
}

