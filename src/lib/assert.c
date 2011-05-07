/**
 * @file
 * @brief TODO
 *
 * @date 25.03.11
 * @author Eldar Abusalimov
 */

#include <kernel/panic.h>

#define __ASSERT_HANDLE_NO_EXTERN_INLINE
#include "assert_impl.h"

void __assertion_handle_failure(const struct __assertion_point *point) {
	const struct location_func *loc = &point->location;

	panic("\nASSERTION FAILED at %s : %d, in function %s\n"
			"\t%s\n",
			loc->at.file, loc->at.line, loc->func,
			point->expression);
}
