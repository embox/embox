/**
 * @file
 *
 * @date 25.11.09
 * @author John R. Hauser - initial implementation.
 * @author Nikolay Korotky
 */

#include "softfloat.h"

/**
 * Underflow tininess-detection mode, statically initialized to default value.
 * (The declaration in `softfloat.h' must match the `int8' type here.)
 */
int8 float_detect_tininess = float_tininess_before_rounding;

/**
 * Raises the exceptions specified by `flags'.  Floating-point traps can be
 * defined here if desired.  It is currently not possible for such a trap
 * to substitute a result value.  If traps are not implemented, this routine
 * should be simply `float_exception_flags |= flags;'.
 */
void float_raise(int8 flags) {
	float_exception_flags |= flags;
}

/**
 * Returns 1 if the single-precision floating-point value `a' is a NaN;
 * otherwise returns 0.
 */
flag float32_is_nan(float32 a) {
	return (0xFF000000 < (bits32) (a<<1));
}

/**
 * Returns 1 if the single-precision floating-point value `a' is a signaling
 * NaN; otherwise returns 0.
 */
flag float32_is_signaling_nan(float32 a) {
	return (((a>>22) & 0x1FF) == 0x1FE) && (a & 0x003FFFFF);
}

/**
 * Returns the result of converting the single-precision floating-point NaN
 * `a' to the canonical NaN format.  If `a' is a signaling NaN, the invalid
 * exception is raised.
 */
commonNaNT float32ToCommonNaN(float32 a) {
	commonNaNT z;
	if (float32_is_signaling_nan(a)) float_raise(float_flag_invalid);
	z.sign = a>>31;
	z.low = 0;
	z.high = a<<9;
	return z;
}

/**
 * Returns the result of converting the canonical NaN `a' to the single-
 * precision floating-point format.
 */
float32 commonNaNToFloat32(commonNaNT a) {
	return (((bits32) a.sign)<<31) | 0x7FC00000 | (a.high>>9);
}

/**
 * Takes two single-precision floating-point values `a' and `b', one of which
 * is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
 * signaling NaN, the invalid exception is raised.
 */
float32 propagateFloat32NaN(float32 a, float32 b) {
	flag /*aIsNaN,*/ aIsSignalingNaN, bIsNaN, bIsSignalingNaN;
	/*aIsNaN = float32_is_nan(a);*/
	aIsSignalingNaN = float32_is_signaling_nan(a);
	bIsNaN = float32_is_nan(b);
	bIsSignalingNaN = float32_is_signaling_nan(b);
	a |= 0x00400000;
	b |= 0x00400000;
	if (aIsSignalingNaN | bIsSignalingNaN) float_raise(float_flag_invalid);
	return bIsSignalingNaN ? b : aIsSignalingNaN ? a : bIsNaN ? b : a;
}

/**
 * Returns 1 if the double-precision floating-point value `a' is a NaN;
 * otherwise returns 0.
 */
flag float64_is_nan(float64 a) {
	return (0xFFE00000 <= (bits32) (a.high<<1))
		&& (a.low || (a.high & 0x000FFFFF));
}

/**
 * Returns 1 if the double-precision floating-point value `a' is a signaling
 * NaN; otherwise returns 0.
 */
flag float64_is_signaling_nan(float64 a) {
	return (((a.high>>19) & 0xFFF) == 0xFFE)
		&& (a.low || (a.high & 0x0007FFFF));
}

/**
 * Returns the result of converting the double-precision floating-point NaN
 * `a' to the canonical NaN format.  If `a' is a signaling NaN, the invalid
 * exception is raised.
 */
commonNaNT float64ToCommonNaN(float64 a) {
	commonNaNT z;

	if (float64_is_signaling_nan(a)) float_raise(float_flag_invalid);
	z.sign = a.high>>31;
	shortShift64Left(a.high, a.low, 12, &z.high, &z.low);
	return z;
}

/**
 * Returns the result of converting the canonical NaN `a' to the double-
 * precision floating-point format.
 */
float64 commonNaNToFloat64(commonNaNT a) {
	float64 z;
	shift64Right(a.high, a.low, 12, &z.high, &z.low);
	z.high |= (((bits32) a.sign)<<31) | 0x7FF80000;
	return z;
}

/**
 * Takes two double-precision floating-point values `a' and `b', one of which
 * is a NaN, and returns the appropriate NaN result.  If either `a' or `b' is a
 * signaling NaN, the invalid exception is raised.
 */
float64 propagateFloat64NaN(float64 a, float64 b) {
	flag /*aIsNaN,*/aIsSignalingNaN, bIsNaN, bIsSignalingNaN;
	/*aIsNaN = float64_is_nan(a);*/
	aIsSignalingNaN = float64_is_signaling_nan(a);
	bIsNaN = float64_is_nan(b);
	bIsSignalingNaN = float64_is_signaling_nan(b);
	a.high |= 0x00080000;
	b.high |= 0x00080000;
	if (aIsSignalingNaN | bIsSignalingNaN) float_raise(float_flag_invalid);
	return bIsSignalingNaN ? b : aIsSignalingNaN ? a : bIsNaN ? b : a;
}
