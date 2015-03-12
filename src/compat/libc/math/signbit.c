/**
 * @file
 * @brief
 *
 * @date 18.12.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <lib/math/ieee.h>
#include <math.h>

int signbit(double x) {
	static_assert(sizeof x == sizeof(struct ieee_binary64));
	return ((struct ieee_binary64 *)&x)->negative;
}

int signbitf(float x) {
	static_assert(sizeof x == sizeof(struct ieee_binary32));
	return ((struct ieee_binary32 *)&x)->negative;
}

int signbitl(long double x) {
	static_assert((sizeof x == sizeof(struct ieee_binary64))
			|| (sizeof x == sizeof(struct ieee_binary80))
			|| (sizeof x == sizeof(struct ieee_binary96)));
	return sizeof x == sizeof(struct ieee_binary64)
			? ((struct ieee_binary64 *)&x)->negative
			: sizeof x == sizeof(struct ieee_binary80)
				? ((struct ieee_binary80 *)&x)->negative
				: ((struct ieee_binary96 *)&x)->negative;
}
