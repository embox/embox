#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "math_private.h"

int __signbit_float(float x) {
	int32_t hx;

	GET_FLOAT_WORD(hx, x);
	return hx & 0x80000000;
}

int __signbit_double(double x) {
	int32_t hx;

	GET_HIGH_WORD(hx, x);
	return hx & 0x80000000;
}

int __signbit_long_double(long double x) {
#if LDBL_MANT_DIG == 113
	int64_t msw;

	GET_LDOUBLE_MSW64 (msw, x);
	return msw < 0;
#elif LDBL_MANT_DIG == 53
	int32_t hx;

	GET_HIGH_WORD(hx, x);
	return hx & 0x80000000;
#else
	int32_t e;

	GET_LDOUBLE_EXP(e, x);
	return e & 0x8000;
#endif
}
