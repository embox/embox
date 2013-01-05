/**
 * @file
 * @brief IEEE Standard 754 Floating Point Numbers
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#ifndef LIB_MATH_IEEE754_H_
#define LIB_MATH_IEEE754_H_

#include <hal/arch.h>

/**
 * IEEE BIAS for single/double-precision formats
 */
#define IEEE754_SINGLE_BIAS 0x7f
#define IEEE754_DOUBLE_BIAS 0x3ff

#define IEEE754_SINGLE_MANT_LEN 23
#define IEEE754_DOUBLE_MANT_LEN 52

/**
 * IEEE single-precision format
 */
union ieee754_single {
	float pure;
	struct {
#if defined(__BIG_ENDIAN)
	unsigned int negative:1,
			exponent:8,
			mantissa:23;
#elif defined(__LITTLE_ENDIAN)
	unsigned int mantissa:23,
			exponent:8,
			negative:1;
#endif
	} ieee;
};

/**
 * IEEE double-precision format
 */
union ieee754_double {
	double pure;
	struct {
#if defined(__BIG_ENDIAN)
	unsigned int negative:1,
			exponent:11,
			mantissa0:20,
			mantissa1:32;
#elif defined(__LITTLE_ENDIAN)
	unsigned int mantissa1:32,
			mantissa0:20,
			exponent:11,
			negative:1;
#endif
	} ieee;
};

#endif /* LIB_MATH_IEEE754_H_ */
