/**
 * @file
 * @brief
 *
 * @date 01.03.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <lib/math/ieee.h>
#include <math.h>

int isnan(double x) {
	struct ieee_binary64 *ieee64_px;

	static_assert(sizeof x == sizeof *ieee64_px);

	ieee64_px = (struct ieee_binary64 *)&x;
	return (ieee64_px->exponent == IEEE_DOUBLE_EXP_MAX)
		&& ((ieee64_px->mantissa0 != 0) || (ieee64_px->mantissa1 != 0));
}

int isnanf(float x) {
	struct ieee_binary32 *ieee32_px;

	static_assert(sizeof x == sizeof *ieee32_px);

	ieee32_px = (struct ieee_binary32 *)&x;
	return (ieee32_px->exponent == IEEE_SINGLE_EXP_MAX)
		&& (ieee32_px->mantissa != 0);
}

int isnanl(long double x) {
	struct ieee_binary64 *ieee64_px;
	struct ieee_binary80 *ieee80_px;
	struct ieee_binary96 *ieee96_px;

	static_assert((sizeof x == sizeof *ieee64_px)
		|| (sizeof x == sizeof *ieee80_px)
		|| (sizeof x == sizeof *ieee96_px));

	return sizeof x == sizeof *ieee64_px
		? (ieee64_px = (struct ieee_binary64 *)&x,
			(ieee64_px->exponent == IEEE_DOUBLE_EXP_MAX)
				&& ((ieee64_px->mantissa0 != 0) || (ieee64_px->mantissa1 != 0)))
		: sizeof x == sizeof *ieee80_px
			? (ieee80_px = (struct ieee_binary80 *)&x,
				(ieee80_px->exponent == IEEE_EXTENDED_EXP_MAX)
					&& ((ieee80_px->mantissa0 != 0) || (ieee80_px->mantissa1 != 0)))
			: (ieee96_px = (struct ieee_binary96 *)&x,
				(ieee96_px->exponent == IEEE_EXTENDED_EXP_MAX)
					&& ((ieee96_px->mantissa0 != 0) || (ieee96_px->mantissa1 != 0)));
}
