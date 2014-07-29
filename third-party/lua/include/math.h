#ifndef LUA_MATH_H_
#define LUA_MATH_H_

#include <sys/cdefs.h>

#define HUGE_VAL (__builtin_huge_val())

__BEGIN_DECLS

static inline double tan(double x) {
	return 0.0;
}

static inline double tanh(double x) {
	return 0.0;
}

static inline double ldexp(double x, int exp) {
	return 0.0;
}

__END_DECLS

#include_next <math.h>

#endif /* LUA_MATH_H_ */
