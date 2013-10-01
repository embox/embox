/**
 * @file
 * @brief IEEE Standard Floating Point Numbers
 *
 * @date 05.01.13
 * @author Ilia Vaprol
 */

#ifndef LIB_MATH_IEEE_H_
#define LIB_MATH_IEEE_H_

#include <stdint.h>
#include <hal/arch.h>
#include <endian.h>

/**
 * IEEE BIAS for single, double and extended precision formats
 */
#define IEEE_SINGLE_BIAS   0x7f
#define IEEE_DOUBLE_BIAS   0x3ff
#define IEEE_EXTENDED_BIAS 0x3fff

/**
 * IEEE maximum exponent value for single, double and extended precision formats
 */
#define IEEE_SINGLE_EXP_MAX   0xff
#define IEEE_DOUBLE_EXP_MAX   0x7ff
#define IEEE_EXTENDED_EXP_MAX 0x7fff

/**
 * IEEE mantissa length for single, double and extended precision formats
 */
#define IEEE_SINGLE_MANT_LEN   23
#define IEEE_DOUBLE_MANT_LEN   52
#define IEEE_EXTENDED_MANT_LEN 64

/**
 * IEEE single-precision format
 */
struct ieee_binary32 {
#if __BYTE_ORDER == __BIG_ENDIAN
	uint32_t negative:1,
			exponent:8,
			mantissa:23;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint32_t mantissa:23,
			exponent:8,
			negative:1;
#endif
};

/**
 * IEEE double-precision format
 */
struct ieee_binary64 {
#if __BYTE_ORDER == __BIG_ENDIAN
	uint32_t negative:1,
			exponent:11,
			mantissa0:20;
	uint32_t mantissa1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint32_t mantissa1;
	uint32_t mantissa0:20,
			exponent:11,
			negative:1;
#endif
};

/**
 * IEEE extended-precision format
 */
struct ieee_binary80 {
#if __BYTE_ORDER == __BIG_ENDIAN
	uint16_t negative:1,
			exponent:15;
	uint32_t mantissa0;
	uint32_t mantissa1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	uint32_t mantissa1;
	uint32_t mantissa0;
	uint16_t exponent:15,
			negative:1;
#endif
};

/**
 * IEEE extended-precision format with padding
 */
struct ieee_binary96 {
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned int negative:1,
			exponent:15,
			unused:16;
	unsigned int mantissa0;
	unsigned int mantissa1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int mantissa1;
	unsigned int mantissa0;
	unsigned int unused:16,
			exponent:15,
			negative:1;
#endif
};

#endif /* LIB_MATH_IEEE_H_ */
