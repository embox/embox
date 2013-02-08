/**
 * @file
 * @brief
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <lib/math/ieee.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

/**
 * TODO
 * SoftFloat library support the operation of taking
 * the remainder but it doesn't work on microblaze :(
 * use #if 1 for test this. also set #if 0 on fmod.c
 */

double modf(double x, double *iptr) {
#if 0
	double fract_part;

	fract_part = fmod(x, 1.0);

	assert(iptr != NULL);
	*iptr = copysign(x - fract_part, x);

	return fract_part;
#else
	struct ieee_binary64 ieee64_x;
	int exponent, bit_mask;

	static_assert(sizeof x == sizeof ieee64_x);

	assert(iptr != NULL);

	memcpy(&ieee64_x, &x, sizeof ieee64_x);
	exponent = ieee64_x.exponent - IEEE_DOUBLE_BIAS;

	if (exponent < 0) {
		*iptr = copysign(0.0, x);
		return x;
	}

	bit_mask = IEEE_DOUBLE_MANT_LEN - exponent;
	if (bit_mask > 31) {
		ieee64_x.mantissa1 = 0;
		ieee64_x.mantissa0 &= ~((1 << (bit_mask - 32)) - 1);
	}
	else if (bit_mask > 0) {
		ieee64_x.mantissa1 &= ~((1 << bit_mask) - 1);
	}

	memcpy(iptr, &ieee64_x, sizeof *iptr);
	return x - *iptr;
#endif
}

float modff(float x, float *iptr) {
#if 0
	float fract_part;

	fract_part = fmodf(x, 1.0F);

	assert(iptr != NULL);
	*iptr = copysignf(x - fract_part, x);

	return fract_part;
#else
	struct ieee_binary32 ieee32_x;
	int exponent, bit_mask;

	static_assert(sizeof x == sizeof ieee32_x);

	assert(iptr != NULL);

	memcpy(&ieee32_x, &x, sizeof ieee32_x);
	exponent = ieee32_x.exponent - IEEE_SINGLE_BIAS;

	if (exponent < 0) {
		*iptr = copysignf(0.0F, x);
		return x;
	}

	bit_mask = IEEE_SINGLE_MANT_LEN - exponent;
	if (bit_mask > 0) {
		ieee32_x.mantissa &= ~((1 << bit_mask) - 1);
	}

	memcpy(iptr, &ieee32_x, sizeof *iptr);
	return x - *iptr;
#endif
}

long double modfl(long double x, long double *iptr) {
#if 0
	long double fract_part;

	fract_part = fmodl(x, 1.0L);

	assert(iptr != NULL);
	*iptr = copysignl(x - fract_part, x);

	return fract_part;
#else
	struct ieee_binary64 ieee64_x;
	struct ieee_binary80 ieee80_x;
	struct ieee_binary96 ieee96_x;
	int exponent, bit_mask;

	static_assert((sizeof x == sizeof ieee64_x)
			|| (sizeof x == sizeof ieee80_x)
			|| (sizeof x == sizeof ieee96_x));

	assert(iptr != NULL);

	memcpy(sizeof x == sizeof ieee64_x ? (void *)&ieee64_x
			: sizeof x == sizeof ieee80_x ? (void *)&ieee80_x
			: (void *)&ieee96_x, &x, sizeof x);
	exponent = sizeof x == sizeof ieee64_x
			? ieee64_x.exponent - IEEE_DOUBLE_BIAS
			: (sizeof x == sizeof ieee80_x ? ieee80_x.exponent
				: ieee96_x.exponent) - IEEE_EXTENDED_BIAS;

	if (exponent < 0) {
		*iptr = copysignl(0.0L, x);
		return x;
	}

	bit_mask = (sizeof x == sizeof ieee64_x ? IEEE_DOUBLE_MANT_LEN
			: IEEE_EXTENDED_MANT_LEN) - exponent;
	if (bit_mask > 31) {
		if (sizeof x == sizeof ieee64_x) {
			ieee64_x.mantissa1 = 0;
			ieee64_x.mantissa0 &= ~((1 << (bit_mask - 32)) - 1);
		}
		else if (sizeof x == sizeof ieee80_x) {
			ieee80_x.mantissa1 = 0;
			ieee80_x.mantissa0 &= ~((1 << (bit_mask - 32)) - 1);
		}
		else {
			ieee96_x.mantissa1 = 0;
			ieee96_x.mantissa0 &= ~((1 << (bit_mask - 32)) - 1);
		}
	}
	else if (bit_mask > 0) {
		if (sizeof x == sizeof ieee64_x) {
			ieee64_x.mantissa1 &= ~((1 << bit_mask) - 1);
		}
		else if (sizeof x == sizeof ieee80_x) {
			ieee80_x.mantissa1 &= ~((1 << bit_mask) - 1);
		}
		else {
			ieee96_x.mantissa1 &= ~((1 << bit_mask) - 1);
		}
	}

	memcpy(iptr, sizeof x == sizeof ieee64_x ? (const void *)&ieee64_x
			: sizeof x == sizeof ieee80_x ? (const void *)&ieee80_x
			: (const void *)&ieee96_x, sizeof *iptr);
	return x - *iptr;
#endif
}
