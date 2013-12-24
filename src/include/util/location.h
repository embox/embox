/**
 * @file
 * @brief A simple utility for tracking an input location (file:line) and
 *        accessing registered locations at run-time.
 *
 * @date 18.03.11
 * @author Eldar Abusalimov
 */

#ifndef UTIL_LOCATION_H_
#define UTIL_LOCATION_H_

#include <stddef.h>

struct location {
	const char *file;
	int line;
};

struct location_func {
	struct location at;
	const char *func;
};

#ifndef __cplusplus
#define LOCATION_INIT { \
		.file = __FILE__, \
		.line = __LINE__, \
	}

#define LOCATION_FUNC_INIT { \
		.at = LOCATION_INIT, \
		.func = __func__,       \
	}
#else
#define LOCATION_INIT { \
		__FILE__,       \
		__LINE__        \
	}

#define LOCATION_FUNC_INIT { \
		LOCATION_INIT,       \
		__func__             \
	}
#endif

#define LOCATION_REF() \
	({ static const struct location __loc = LOCATION_INIT; &__loc; })

#define LOCATION_FUNC_REF() \
	({ static const struct location_func __loc = LOCATION_FUNC_INIT; &__loc; })

#define LOCATION_FMT(prefix, suffix) \
	prefix "at %s:%d" suffix
#define LOCATION_ARGS(loc) \
	(loc) ? (loc)->file : NULL, \
	(loc) ? (loc)->line : 0

#define LOCATION_FUNC_FMT(prefix, suffix) \
	LOCATION_FMT(prefix, suffix) \
	prefix "in function %s" suffix
#define LOCATION_FUNC_ARGS(loc) \
	LOCATION_ARGS((loc) ? &(loc)->at : NULL), \
	(loc) ? (loc)->func : NULL

#endif /* UTIL_LOCATION_H_ */
