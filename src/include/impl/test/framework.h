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

#include <stddef.h>

#include <util/macro.h>
#include <util/array.h>

#include <impl/test/types.h>

#define __test_foreach(test_ptr) \
	array_foreach_ptr(test_ptr, __extension__ ({       \
				extern struct test __test_registry[];  \
				__test_registry;                       \
			}),  __extension__ ({                      \
				extern struct test __test_registry[];  \
				ARRAY_DIFFUSE_SIZE(__test_registry);   \
			}))

// XXX for struct mod. -- Eldar
#include <mod/core.h>
inline static const char *test_name(struct test *test) {
	return NULL != test ? test->mod->name : NULL;
}
