/**
 * @file
 * @brief
 *
 * @date 30.01.13
 * @author Ilia Vaprol
 */

#ifndef CONVERSION_ROUTINES_H_
#define CONVERSION_ROUTINES_H_

#include <assert.h>
#include <lib/math/ieee.h>
#include <lib/softfloat.h>

static inline float32 float_to_float32(float x) {
	static_assert(sizeof x == sizeof(float32));
	return *(float32 *)&x;
}

static inline float float32_to_float(float32 x) {
	static_assert(sizeof x == sizeof(float));
	return *(float *)&x;
}

static inline float64 double_to_float64(double x) {
	static_assert(sizeof x == sizeof(float64));
	return *(float64 *)&x;
}

static inline double float64_to_double(float64 x) {
	static_assert(sizeof x == sizeof(double));
	return *(double *)&x;
}

static inline floatx80 ldouble_to_floatx80(long double x) {
	return *(floatx80 *)&x;
}

static inline long double floatx80_to_ldouble(floatx80 x) {
	return *(long double *)&x;
}

#endif /* CONVERSION_ROUTINES_H_ */
