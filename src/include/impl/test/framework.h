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

#include <util/macro.h>
#include <util/array.h>

#include <impl/test/types.h>

#define __test_foreach(t) MACRO_INVOKE(__test_foreach_iterator, \
		t, MACRO_CONCAT(__test_iterator_, __LINE__))
#define __test_foreach_iterator(t, i) \
		for(struct test_iterator i##_alloc, *i = test_get_all(&i##_alloc); \
			(t = test_iterator_has_next(i) ? test_iterator_next(i) : NULL);)

struct test_iterator {
	const struct test *test;
};

inline static struct test_iterator *test_get_all(struct test_iterator *iterator) {
	extern const struct test __test_registry[];
	if (NULL == iterator) {
		return NULL;
	}
	iterator->test = __test_registry;
	return iterator;
}

inline static bool test_iterator_has_next(struct test_iterator *iterator) {
	extern const struct test __test_registry[];
	return NULL != iterator && iterator->test <
			__test_registry + ARRAY_DIFFUSE_SIZE(__test_registry);
}

inline static struct test *test_iterator_next(struct test_iterator *iterator) {
	if (!test_iterator_has_next(iterator)) {
		return NULL;
	}
	return iterator->test++;
}


// XXX for struct mod. -- Eldar
#include <mod/framework.h>
inline static const char *test_name(struct test *test) {
	return NULL != test ? test->mod->name : NULL;
}
