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

extern const struct test __test_registry[];
#define __test_foreach(test_ptr) \
	array_diffuse_foreach_ptr(test_ptr, __test_registry)

// XXX for struct mod. -- Eldar
#include <mod/core.h>
inline static const char *test_name(const struct test *test) {
	return NULL != test ? test->mod->name : NULL;
}
