/**
 * @file
 * @brief
 *
 * @date 18.12.12
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <lib/math/ieee754.h>
#include <math.h>

int signbit(double x) {
	return ((union ieee754_double *)&x)->ieee.negative;
}

int signbitf(float x) {
	return ((union ieee754_single *)&x)->ieee.negative;
}

int signbitl(long double x) {
	/* FIXME it's work only if size of double and long double are the same */
	/*assert(sizeof x == sizeof(double));*/
	return signbit((double)x);
}
