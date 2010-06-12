/**
 * @file
 * @brief Implementation of test iterator operations (macros and inline
 *        methods).
 *
 * @date 11.06.2010
 * @author Eldar Abusalimov
 */

#ifndef TEST_FRAMEWORK_H_
# error "Do not include this file directly, use <test/framework.h> instead!"
#endif /* TEST_FRAMEWORK_H_ */

#include <embox/mod.h>
#include <util/macro.h>

#include <impl/test/types.h>

#define __test_foreach(t) MACRO_INVOKE(__test_foreach_iterator, \
		t, MACRO_CONCAT(__test_iterator_, __LINE__))
#define __test_foreach_iterator(t, i) \
		for(struct test_iterator i##_alloc, *i = test_get_all(&i##_alloc); \
			(t = test_iterator_has_next(i) ? test_iterator_next(i) : NULL);)

struct test_iterator {
	struct mod_iterator mod_iterator;
};

inline static struct test_iterator *test_get_all(struct test_iterator *iterator) {
	__MOD_TAG_DECL(test);
	if (NULL == iterator) {
		return NULL;
	}
	mod_tagged(MOD_TAG_PTR(test), &iterator->mod_iterator);
	return iterator;
}

inline static bool test_iterator_has_next(struct test_iterator *iterator) {
	return NULL != iterator && mod_iterator_has_next(&iterator->mod_iterator);
}

inline static struct test *test_iterator_next(struct test_iterator *iterator) {
	if (!test_iterator_has_next(iterator)) {
		return NULL;
	}
	return (struct test *) mod_data(mod_iterator_next(&iterator->mod_iterator));
}

