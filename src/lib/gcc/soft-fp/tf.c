/**
 * @file
 * @brief Extended precision soft-fp
 *
 * @date 31.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <softfloat.h>
#include <lib/soft-fp/conversion_routines.h>

long double __addtf3(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(float64_add(
					double_to_float64((double)a), double_to_float64((double)b)))
			: floatx80_to_ldouble(floatx80_add(ldouble_to_floatx80(a),
					ldouble_to_floatx80(b)));
}

long double __subtf3(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(float64_sub(
					double_to_float64((double)a), double_to_float64((double)b)))
			: floatx80_to_ldouble(floatx80_sub(ldouble_to_floatx80(a),
					ldouble_to_floatx80(b)));
}

long double __multf3(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(float64_mul(
					double_to_float64((double)a), double_to_float64((double)b)))
			: floatx80_to_ldouble(floatx80_mul(ldouble_to_floatx80(a),
					ldouble_to_floatx80(b)));
}

long double __divtf3(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(float64_div(
					double_to_float64((double)a), double_to_float64((double)b)))
			: floatx80_to_ldouble(floatx80_div(ldouble_to_floatx80(a),
					ldouble_to_floatx80(b)));
}

long double __negtf2(long double a) {
	return __subtf3(0.0L, a);
}

float __trunctfsf2(long double a) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? float32_to_float(float64_to_float32(double_to_float64((double)a)))
			: float32_to_float(floatx80_to_float32(ldouble_to_floatx80(a)));
}

double __trunctfdf2(long double a) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double) ? (double)a
			: float64_to_double(floatx80_to_float64(ldouble_to_floatx80(a)));
}

int __fixtfsi(long double a) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	static_assert(sizeof(int) == sizeof(int32));
	return sizeof(float64) == sizeof(long double)
			? (int)float64_to_int32(double_to_float64((double)a))
			: (int)floatx80_to_int32(ldouble_to_floatx80(a));
}

long int __fixtfdi(long double a) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	static_assert(sizeof(long int) == sizeof(int32));
	return sizeof(float64) == sizeof(long double)
			? (long int)float64_to_int32(double_to_float64((double)a))
			: (long int)floatx80_to_int32(ldouble_to_floatx80(a));
}

long long int __fixtfti(long double a) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	static_assert(sizeof(long long int) == sizeof(int64));
	return sizeof(float64) == sizeof(long double)
			? (long long int)float64_to_int64(double_to_float64((double)a))
			: (long long int)floatx80_to_int64(ldouble_to_floatx80(a));
}

extern unsigned int __fixunstfsi(long double a);
extern unsigned long __fixunstfdi(long double a);
extern unsigned long long __fixunstfti(long double a);

long double __floatsitf(int i) {
	static_assert(sizeof(int32) == sizeof(int));
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(int32_to_float64((int32)i))
			: floatx80_to_ldouble(int32_to_floatx80((int32)i));
}

long double __floatditf(long int i) {
	static_assert(sizeof(int32) == sizeof(long int));
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(int32_to_float64((int32)i))
			: floatx80_to_ldouble(int32_to_floatx80((int32)i));
}

long double __floattitf(long long int i) {
	static_assert(sizeof(int64) == sizeof(long long int));
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? (long double)float64_to_double(int64_to_float64((int64)i))
			: floatx80_to_ldouble(int64_to_floatx80((int64)i));
}

extern long double __floatunsitf(unsigned int i);
extern long double __floatunditf(unsigned long i);
extern long double __floatuntitf(unsigned long long i);

/////////FROM HERE!
int __cmptf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? float64_lt(double_to_float64((double)a), double_to_float64((double)b)) ? -1
				: !float64_le(double_to_float64((double)a), double_to_float64((double)b))
			: floatx80_lt(ldouble_to_floatx80(a), ldouble_to_floatx80(b)) ? -1
				: !floatx80_le(ldouble_to_floatx80(a), ldouble_to_floatx80(b));
}

int __unordtf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? !float64_eq(double_to_float64((double)a), double_to_float64((double)a))
				|| !float64_eq(double_to_float64((double)b), double_to_float64((double)b))
			: !floatx80_eq(ldouble_to_floatx80(a), ldouble_to_floatx80(a))
				|| !floatx80_eq(ldouble_to_floatx80(b), ldouble_to_floatx80(b));
}

int __eqtf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? !float64_eq(double_to_float64((double)a), double_to_float64((double)b))
			: !floatx80_eq(ldouble_to_floatx80(a), ldouble_to_floatx80(b));
}

int __netf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? !float64_eq(double_to_float64((double)a), double_to_float64((double)b))
			: !floatx80_eq(ldouble_to_floatx80(a), ldouble_to_floatx80(b));
}

int __getf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? -float64_lt(double_to_float64((double)a), double_to_float64((double)b))
			: -floatx80_lt(ldouble_to_floatx80(a), ldouble_to_floatx80(b));
}

int __lttf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? -float64_lt(double_to_float64((double)a), double_to_float64((double)b))
			: -floatx80_lt(ldouble_to_floatx80(a), ldouble_to_floatx80(b));
}

int __letf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? !float64_le(double_to_float64((double)a), double_to_float64((double)b))
			: !floatx80_le(ldouble_to_floatx80(a), ldouble_to_floatx80(b));
}

int __gttf2(long double a, long double b) {
	static_assert((sizeof(float64) != sizeof(long double))
			|| (sizeof(long double) == sizeof(double)));
	return sizeof(float64) == sizeof(long double)
			? !float64_le(double_to_float64((double)a), double_to_float64((double)b))
			: !floatx80_le(ldouble_to_floatx80(a), ldouble_to_floatx80(b));
}
