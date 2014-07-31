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
#include <softfloat.h>

static inline float32 float_to_float32(float x) {
	static_assert(sizeof x == sizeof(struct ieee_binary32));
	static_assert(sizeof(struct ieee_binary32) == sizeof(float32));
	return *(float32 *)(struct ieee_binary32 *)&x;
}

static inline float float32_to_float(float32 x) {
	static_assert(sizeof x == sizeof(struct ieee_binary32));
	static_assert(sizeof(struct ieee_binary32) == sizeof(float));
	return *(float *)(struct ieee_binary32 *)&x;
}

static inline float64 double_to_float64(double x) {
	static_assert(sizeof x == sizeof(struct ieee_binary64));
	static_assert(sizeof(struct ieee_binary64) == sizeof(float64));
	return *(float64 *)(struct ieee_binary64 *)&x;
}

static inline double float64_to_double(float64 x) {
	static_assert(sizeof x == sizeof(struct ieee_binary64));
	static_assert(sizeof(struct ieee_binary64) == sizeof(double));
	return *(double *)(struct ieee_binary64 *)&x;
}

static inline floatx80 ldouble_to_floatx80(long double x) {
	struct ieee_binary80 ieee80_x;
	struct ieee_binary96 *ieee96_px;
	static_assert((sizeof x == sizeof(struct ieee_binary64))
			|| (sizeof x == sizeof(struct ieee_binary80))
			|| (sizeof x == sizeof(struct ieee_binary96)));
	if (sizeof x == sizeof(struct ieee_binary64)) {
		static_assert((sizeof x != sizeof(struct ieee_binary64))
					|| (sizeof(double) == sizeof(long double)));
		return float64_to_floatx80(double_to_float64((double)x));
	}
	else if (sizeof x == sizeof(struct ieee_binary80)) {
		static_assert((sizeof x != sizeof(struct ieee_binary80))
			 	|| (sizeof(struct ieee_binary80) == sizeof(floatx80)));
		return *(floatx80 *)(struct ieee_binary80 *)&x;
	}
	else {
		static_assert((sizeof x != sizeof(struct ieee_binary96))
				|| (sizeof ieee80_x == sizeof(floatx80)));
		ieee96_px = (struct ieee_binary96 *)&x;
		ieee80_x.negative = ieee96_px->negative;
		ieee80_x.exponent = ieee96_px->exponent;
		ieee80_x.mantissa0 = ieee96_px->mantissa0;
		ieee80_x.mantissa1 = ieee96_px->mantissa1;
		return *(floatx80 *)&ieee80_x;
	}
}

static inline long double floatx80_to_ldouble(floatx80 x) {
	struct ieee_binary80 *ieee80_px;
	struct ieee_binary96 ieee96_x;
	static_assert((sizeof(struct ieee_binary64) == sizeof(long double))
			|| (sizeof(struct ieee_binary80) == sizeof(long double))
			|| (sizeof(struct ieee_binary96) == sizeof(long double)));
	if (sizeof(struct ieee_binary64) == sizeof(long double)) {
		static_assert((sizeof(struct ieee_binary64) != sizeof(long double))
				|| (sizeof(long double) == sizeof(double)));
		return (long double)float64_to_double(floatx80_to_float64(x));
	}
	else if (sizeof(struct ieee_binary80) == sizeof(long double)) {
		static_assert((sizeof(struct ieee_binary80) != sizeof(long double))
				|| (sizeof x == sizeof(struct ieee_binary80)));
		return *(long double *)(struct ieee_binary80 *)&x;
	}
	else {
		static_assert((sizeof(struct ieee_binary96) != sizeof(long double))
				|| (sizeof x == sizeof(struct ieee_binary80)));
		ieee80_px = (struct ieee_binary80 *)&x;
		ieee96_x.negative = ieee80_px->negative;
		ieee96_x.exponent = ieee80_px->exponent;
		ieee96_x.unused = 0;
		ieee96_x.mantissa0 = ieee80_px->mantissa0;
		ieee96_x.mantissa1 = ieee80_px->mantissa1;
		return *(long double *)&ieee96_x;
	}
}

#endif /* CONVERSION_ROUTINES_H_ */
