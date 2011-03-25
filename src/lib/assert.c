/**
 * @file
 * @brief TODO
 *
 * @date Mar 25, 2011
 * @author Eldar Abusalimov
 */

#include <kernel/panic.h>

#include "assert_impl.h"

void __assertion_handle(int pass, const struct __assertion_point *point) {
	const struct location_func *loc = &point->location;

	if (!pass) {
		panic("\nASSERTION FAILED at %s : %d, in function %s\n"
				"\t%s\n",
				loc->at.file, loc->at.line, loc->func,
				point->expression);
	}

}

