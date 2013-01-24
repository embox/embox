/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <lib/math/ieee754.h>
#include <stdlib.h>
#include <math.h>

double modf(double x, double *iptr) {
	union ieee754_double val;
	int exponent, bit_mask;

	assert(iptr != NULL);

	val.pure = x;
	exponent = val.ieee.exponent - IEEE754_DOUBLE_BIAS;

	if (exponent < 0) {
		*iptr = copysign(0.0, x);
		return x;
	}

	bit_mask = IEEE754_DOUBLE_MANT_LEN - exponent;
	if (bit_mask > 31) {
		val.ieee.mantissa1 = 0;
		val.ieee.mantissa0 &= ~((1 << (bit_mask - 32)) - 1);
	}
	else if (bit_mask > 0) {
		val.ieee.mantissa1 &= ~((1 << bit_mask) - 1);
	}

	*iptr = val.pure;
	return x - val.pure;
}

float modff(float x, float *iptr) {
	union ieee754_single val;
	int exponent, bit_mask;

	assert(iptr != NULL);

	val.pure = x;
	exponent = val.ieee.exponent - IEEE754_SINGLE_BIAS;

	if (exponent < 0) {
		*iptr = copysignf(0.0F, x);
		return x;
	}

	bit_mask = IEEE754_SINGLE_MANT_LEN - exponent;
	if (bit_mask > 0) {
		val.ieee.mantissa &= ~((1 << bit_mask) - 1);
	}

	*iptr = val.pure;
	return x - val.pure;
}

long double modfl(long double x, long double *iptr) {
	double int_part, frac_part;
	/*assert(sizeof x == sizeof(double));*/
	/*assert(sizeof *iptr == sizeof(double));*/
	frac_part = modf((double)x, &int_part);
	*iptr = (long double)int_part;
	return (long double)frac_part;
}
