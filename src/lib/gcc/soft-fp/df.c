/**
 * @file
 * @brief Double precision soft-fp
 *
 * @date 25.11.09
 * @author Nikolay Korotky
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <softfloat.h>
#include <lib/soft-fp/conversion_routines.h>

double __adddf3(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_to_double(float64_add(double_to_float64(a),
				double_to_float64(b)));
}

double __subdf3(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_to_double(float64_sub(double_to_float64(a),
				double_to_float64(b)));
}

double __muldf3(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_to_double(float64_mul(double_to_float64(a),
				double_to_float64(b)));
}

double __divdf3(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_to_double(float64_div(double_to_float64(a),
				double_to_float64(b)));
}

double __negdf2(double a) {
	return __subdf3(0.0, a);
}

long double __extenddftf2(double a) {
	static_assert(sizeof(float64) == sizeof(double));
	static_assert((sizeof(long double) != sizeof(float64))
			|| (sizeof(float64) == sizeof(double)));
	return sizeof(long double) == sizeof(float64) ? (long double)a
			: floatx80_to_ldouble(float64_to_floatx80(double_to_float64(a)));
}

float __truncdfsf2(double a) {
	static_assert(sizeof(float64) == sizeof(double));
	static_assert(sizeof(float) == sizeof(float32));
	return (float)float32_to_float(float64_to_float32(double_to_float64(a)));
}

int __fixdfsi(double a) {
	static_assert(sizeof(float64) == sizeof(double));
	static_assert(sizeof(int) == sizeof(int32));
	return (int)float64_to_int32(double_to_float64(a));
}

long int __fixdfdi(double a) {
	static_assert(sizeof(float64) == sizeof(double));
	static_assert(sizeof(long int) == sizeof(int32));
	return (long int)float64_to_int32(double_to_float64(a));
}

long long int __fixdfti(double a) {
	static_assert(sizeof(float64) == sizeof(double));
	static_assert(sizeof(long long int) == sizeof(int64));
	return (long long int)float64_to_int64(double_to_float64(a));
}

extern unsigned int __fixunsdfsi(double a);
extern unsigned long __fixunsdfdi(double a);
extern unsigned long long __fixunsdfti(double a);

double __floatsidf(int i) {
	static_assert(sizeof(int32) == sizeof(int));
	static_assert(sizeof(double) == sizeof(float64));
	return float64_to_double(int32_to_float64((int32)i));
}

double __floatdidf(long int i) {
	static_assert(sizeof(int32) == sizeof(long int));
	static_assert(sizeof(double) == sizeof(float64));
	return float64_to_double(int32_to_float64((int32)i));
}

double __floattidf(long long int i) {
	static_assert(sizeof(int64) == sizeof(long long int));
	static_assert(sizeof(double) == sizeof(float64));
	return float64_to_double(int64_to_float64((int64)i));
}

extern double __floatunsidf(unsigned int i);
extern double __floatundidf(unsigned long i);
extern double __floatuntidf(unsigned long long i);

int __cmpdf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return float64_lt(double_to_float64(a), double_to_float64(b)) ? -1
			: !float64_le(double_to_float64(a), double_to_float64(b));
}

int __unorddf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return !float64_eq(double_to_float64(a), double_to_float64(a))
			|| !float64_eq(double_to_float64(b), double_to_float64(b));
}

int __eqdf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return !float64_eq(double_to_float64(a), double_to_float64(b));
}

int __nedf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return !float64_eq(double_to_float64(a), double_to_float64(b));
}

int __gedf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return -float64_lt(double_to_float64(a), double_to_float64(b));
}

int __ltdf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return -float64_lt(double_to_float64(a), double_to_float64(b));
}

int __ledf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return !float64_le(double_to_float64(a), double_to_float64(b));
}

int __gtdf2(double a, double b) {
	static_assert(sizeof(float64) == sizeof(double));
	return !float64_le(double_to_float64(a), double_to_float64(b));
}
