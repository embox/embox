/**
 * @file
 * @brief Single precision soft-fp
 *
 * @date 26.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <softfloat.h>
#include <lib/soft-fp/conversion_routines.h>

float __addsf3(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_to_float(float32_add(float_to_float32(a),
				float_to_float32(b)));
}

float __subsf3(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_to_float(float32_sub(float_to_float32(a),
				float_to_float32(b)));
}

float __mulsf3(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_to_float(float32_mul(float_to_float32(a),
				float_to_float32(b)));
}

float __divsf3(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_to_float(float32_div(float_to_float32(a),
				float_to_float32(b)));
}

float __negsf2(float a) {
	return __subsf3(0.0F, a);
}

double __extendsfdf2(float a) {
	static_assert(sizeof(float32) == sizeof(float));
	static_assert(sizeof(double) == sizeof(float64));
	return float64_to_double(float32_to_float64(float_to_float32(a)));
}

long double __extendsftf2(float a) {
	static_assert(sizeof(float32) == sizeof(float));
	static_assert((sizeof(long double) != sizeof(float64))
			|| (sizeof(float64) == sizeof(double)));
	return sizeof(long double) == sizeof(float64)
			? (long double)float64_to_double(float32_to_float64(float_to_float32(a)))
			: floatx80_to_ldouble(float32_to_floatx80(float_to_float32(a)));
}

int __fixsfsi(float a) {
	static_assert(sizeof(float32) == sizeof(float));
	static_assert(sizeof(int) == sizeof(int32));
	return (int)float32_to_int32(float_to_float32(a));
}

long int __fixsfdi(float a) {
	static_assert(sizeof(float32) == sizeof(float));
	static_assert(sizeof(long int) == sizeof(int32));
	return (long int)float32_to_int32(float_to_float32(a));
}

long long int __fixsfti(float a) {
	static_assert(sizeof(float32) == sizeof(float));
	static_assert(sizeof(long long int) == sizeof(int64));
	return (long long int)float32_to_int64(float_to_float32(a));
}

extern unsigned int __fixunssfsi(float a);
extern unsigned long int __fixunssfdi(float a);
extern unsigned long long int __fixunssfti(float a);

float __floatsisf(int i) {
	static_assert(sizeof(int32) == sizeof(int));
	static_assert(sizeof(float) == sizeof(float32));
	return float32_to_float(int32_to_float32((int32)i));
}

float __floatdisf(long int i) {
	static_assert(sizeof(int32) == sizeof(long int));
	static_assert(sizeof(float) == sizeof(float32));
	return float32_to_float(int32_to_float32((int32)i));
}

float __floattisf(long long int i) {
	static_assert(sizeof(int64) == sizeof(long long int));
	static_assert(sizeof(float) == sizeof(float32));
	return float32_to_float(int64_to_float32((int64)i));
}

extern float __floatunsisf(unsigned int i);
extern float __floatundisf(unsigned long int i);
extern float __floatuntisf(unsigned long long int i);

int __cmpsf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return float32_lt(float_to_float32(a), float_to_float32(b)) ? -1
			: !float32_le(float_to_float32(a), float_to_float32(b));
}

int __unordsf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return !float32_eq(float_to_float32(a), float_to_float32(a))
			|| !float32_eq(float_to_float32(b), float_to_float32(b));
}

int __eqsf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return !float32_eq(float_to_float32(a), float_to_float32(b));
}

int __nesf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return !float32_eq(float_to_float32(a), float_to_float32(b));
}

int __gesf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return -float32_lt(float_to_float32(a), float_to_float32(b));
}

int __ltsf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return -float32_lt(float_to_float32(a), float_to_float32(b));
}

int __lesf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return !float32_le(float_to_float32(a), float_to_float32(b));
}

int __gtsf2(float a, float b) {
	static_assert(sizeof(float32) == sizeof(float));
	return !float32_le(float_to_float32(a), float_to_float32(b));
}
