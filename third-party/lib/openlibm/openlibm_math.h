
#ifndef THIRD_PARTY_LIB_OPENLIBM_MATH_H_
#define THIRD_PARTY_LIB_OPENLIBM_MATH_H_

#define __BSD_VISIBLE 1
#include <../../build/extbld/third_party/lib/OpenLibm/install/openlibm_math.h>

/**
 * Stubs for long double functions (long double == double)
 */
#if (__LDBL_MANT_DIG__ == __DBL_MANT_DIG__)
#define acoshl      acosh
#define acosl       acos
#define asinhl      asinh
#define asinl       asin
#define atan2l      atan2
#define atanhl      atanh
#define atanl       atan
#define cbrtl       cbrt
#define ceill       ceil
#define copysignl   copysign
#define coshl       cosh
#define cosl        cos
#define erfcl       erfc
#define erfl        erf
#define exp2l       exp2
#define expl        exp
#define expm1l      expm1
#define fabsl       fabs
#define fdiml       fdim
#define floorl      floor
#define fmal        fma
#define fmaxl       fmax
#define fminl       fmin
#define fmodl       fmod
#define frexpl      frexp
#define hypotl      hypot
#define ilogbl      ilogb
#define ldexpl      ldexp
#define lgammal     lgamma
#define llrintl     llrint
#define llroundl    llround
#define log10l      log10
#define log1pl      log1p
#define log2l       log2
#define logbl       logb
#define logl        log
#define lrintl      lrint
#define lroundl     lround
#define modfl       modf
#define nanl        nan
#define nearbyintl  nearbyint
#define nextafterl  nextafter
#define nexttowardl nexttoward
#define powl        pow
#define remainderl  remainder
#define remquol     remquo
#define rintl       rint
#define roundl      round
#define scalblnl    scalbln
#define scalbnl     scalbn
#define sinhl       sinh
#define sinl        sin
#define sqrtl       sqrt
#define tanhl       tanh
#define tanl        tan
#define tgammal     tgamma
#define truncl      trunc
#endif /* (__LDBL_MANT_DIG__ == __DBL_MANT_DIG__) */

/**
 * For compatibility with Linux
 */
static inline int finite(double x) {
	return isfinite(x);
}

static inline int finitef(float x) {
	return isfinite(x);
}

static inline int finitel(long double x) {
	return isfinite(x);
}

#endif /* THIRD_PARTY_LIB_OPENLIBM_MATH_H_ */
