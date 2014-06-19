/**
 * @file
 * @brief soft-fp non-libgcc routines
 *
 * @date 27.09.2013
 * @author Alexander Kalmuk
 */

#include <assert.h>
#include <softfloat.h>
#include <lib/soft-fp/conversion_routines.h>

float __sqrtsf2(float a) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_to_float(float32_sqrt(float_to_float32(a)));
}

float __roundsf2(float a) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_to_float(float32_round_to_int(float_to_float32(a)));
}

float __remsf3(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_to_float(float32_rem(float_to_float32(a),
				float_to_float32(b)));
}

double __sqrtdf2(double a) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_to_double(float64_sqrt(double_to_float64(a)));
}

double __rounddf2(double a) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_to_double(float64_round_to_int(double_to_float64(a)));
}

double __remdf3(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_to_double(float64_rem(double_to_float64(a),
				double_to_float64(b)));
}

long double __sqrttf2(long double a) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(float64_sqrt(double_to_float64((double)a)))
			: floatx80_to_ldouble(floatx80_sqrt(ldouble_to_floatx80(a)));
}

long double __roundtf2(long double a) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(float64_round_to_int(double_to_float64((double)a)))
			: floatx80_to_ldouble(floatx80_round_to_int(ldouble_to_floatx80(a)));
}

long double __remtf3(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(float64_rem(
					double_to_float64((double)a), double_to_float64((double)b)))
			: floatx80_to_ldouble(floatx80_rem(ldouble_to_floatx80(a),
					ldouble_to_floatx80(b)));
}
