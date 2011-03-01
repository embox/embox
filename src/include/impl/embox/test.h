/**
 * @file
 * @brief Hidden implementations of test registration macros.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef EMBOX_TEST_H_
# error "Do not include this file directly, use <embox/test.h> instead!"
#endif /* EMBOX_TEST_H_ */

#include <util/array.h>
#include <mod/self.h>

#include <impl/test/types.h>

#define __EMBOX_TEST(_run) \
	static int _run(void);                             \
	static struct test_private __test_private;         \
	ARRAY_SPREAD_ADD_NAMED(__test_registry, __test, {  \
			.private = &__test_private,                \
			.run = _run,                               \
			.mod = &mod_self                           \
		});                                            \
	MOD_SELF_BIND(__test, &__test_mod_ops)

extern const struct mod_ops __test_mod_ops;
extern const struct test __test_registry[];
