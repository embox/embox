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

#include <mod/bind.h>

#include <impl/test/types.h>

#define __EMBOX_TEST(_run) \
	static const struct test __test__; \
	extern const struct mod_ops __test_mod_ops; \
	MOD_SELF_INFO_TAGGED_DEF(&__test__, &__test_mod_ops, test); \
	extern const char MOD_SELF_NAME[]; \
	static int _run(void); \
	static struct test_private __test_private__; \
	static const struct test __test__ = { \
			.private = &__test_private__, \
			.run = _run, \
			.name = MOD_SELF_NAME \
		}

