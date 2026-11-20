/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 17.11.26
 */

#undef __CORRECT_ISO_CPP_MATH_H_PROTO
#define __CORRECT_ISO_CPP_MATH_H_PROTO

#undef __CORRECT_ISO_CPP11_MATH_H_PROTO_FP
#define __CORRECT_ISO_CPP11_MATH_H_PROTO_FP

#undef __CORRECT_ISO_CPP11_MATH_H_PROTO_INT

#undef _GLIBCXX_HAVE_ACOSF
#define _GLIBCXX_HAVE_ACOSF

#undef _GLIBCXX_HAVE_ACOSL
#define _GLIBCXX_HAVE_ACOSL

#undef _GLIBCXX_HAVE_ASINF
#define _GLIBCXX_HAVE_ASINF

#undef _GLIBCXX_HAVE_ASINL
#define _GLIBCXX_HAVE_ASINL

#undef _GLIBCXX_HAVE_ATANF
#define _GLIBCXX_HAVE_ATANF

#undef _GLIBCXX_HAVE_ATANL
#define _GLIBCXX_HAVE_ATANL

#undef _GLIBCXX_HAVE_ATAN2F
#define _GLIBCXX_HAVE_ATAN2F

#undef _GLIBCXX_HAVE_ATAN2L
#define _GLIBCXX_HAVE_ATAN2L

#undef _GLIBCXX_HAVE_CEILF
#define _GLIBCXX_HAVE_CEILF

#undef _GLIBCXX_HAVE_CEILL
#define _GLIBCXX_HAVE_CEILL

#undef _GLIBCXX_HAVE_COSF
#define _GLIBCXX_HAVE_COSF

#undef _GLIBCXX_HAVE_COSL
#define _GLIBCXX_HAVE_COSL

#undef _GLIBCXX_HAVE_COSHF
#define _GLIBCXX_HAVE_COSHF

#undef _GLIBCXX_HAVE_COSHL
#define _GLIBCXX_HAVE_COSHL

#undef _GLIBCXX_HAVE_EXPF
#define _GLIBCXX_HAVE_EXPF

#undef _GLIBCXX_HAVE_EXPL
#define _GLIBCXX_HAVE_EXPL

#undef _GLIBCXX_HAVE_FABSF
#define _GLIBCXX_HAVE_FABSF

#undef _GLIBCXX_HAVE_FABSL
#define _GLIBCXX_HAVE_FABSL

#undef _GLIBCXX_HAVE_FLOORF
#define _GLIBCXX_HAVE_FLOORF

#undef _GLIBCXX_HAVE_FLOORL
#define _GLIBCXX_HAVE_FLOORL

#undef _GLIBCXX_HAVE_FMODF
#define _GLIBCXX_HAVE_FMODF

#undef _GLIBCXX_HAVE_FMODL
#define _GLIBCXX_HAVE_FMODL

#undef _GLIBCXX_HAVE_FREXPF
#define _GLIBCXX_HAVE_FREXPF

#undef _GLIBCXX_HAVE_FREXPL
#define _GLIBCXX_HAVE_FREXPL

#undef _GLIBCXX_HAVE_LDEXPF
#define _GLIBCXX_HAVE_LDEXPF

#undef _GLIBCXX_HAVE_LDEXPL
#define _GLIBCXX_HAVE_LDEXPL

#undef _GLIBCXX_HAVE_LOGF
#define _GLIBCXX_HAVE_LOGF

#undef _GLIBCXX_HAVE_LOGL
#define _GLIBCXX_HAVE_LOGL

#undef _GLIBCXX_HAVE_LOG10F
#define _GLIBCXX_HAVE_LOG10F

#undef _GLIBCXX_HAVE_LOG10L
#define _GLIBCXX_HAVE_LOG10L

#undef _GLIBCXX_HAVE_MODFF
#define _GLIBCXX_HAVE_MODFF

#undef _GLIBCXX_HAVE_MODFL
#define _GLIBCXX_HAVE_MODFL

#undef _GLIBCXX_HAVE_POWF
#define _GLIBCXX_HAVE_POWF

#undef _GLIBCXX_HAVE_POWL
#define _GLIBCXX_HAVE_POWL

#undef _GLIBCXX_HAVE_SINF
#define _GLIBCXX_HAVE_SINF

#undef _GLIBCXX_HAVE_SINL
#define _GLIBCXX_HAVE_SINL

#undef _GLIBCXX_HAVE_SINHF
#define _GLIBCXX_HAVE_SINHF

#undef _GLIBCXX_HAVE_SINHL
#define _GLIBCXX_HAVE_SINHL

#undef _GLIBCXX_HAVE_SQRTF
#define _GLIBCXX_HAVE_SQRTF

#undef _GLIBCXX_HAVE_SQRTL
#define _GLIBCXX_HAVE_SQRTL

#undef _GLIBCXX_HAVE_TANF
#define _GLIBCXX_HAVE_TANF

#undef _GLIBCXX_HAVE_TANL
#define _GLIBCXX_HAVE_TANL

#undef _GLIBCXX_HAVE_TANHF
#define _GLIBCXX_HAVE_TANHF

#undef _GLIBCXX_HAVE_TANHL
#define _GLIBCXX_HAVE_TANHL

#undef _GLIBCXX_HAVE_C99_FLT_EVAL_TYPES
#define _GLIBCXX_HAVE_C99_FLT_EVAL_TYPES

#undef _GLIBCXX_USE_C99_MATH_FUNCS
#define _GLIBCXX_USE_C99_MATH_FUNCS 1

#undef fpclassify
#undef isfinite
#undef isinf
#undef isnan
#undef isnormal
#undef isgreater
#undef isgreaterequal
#undef isless
#undef islessequal
#undef islessgreater
#undef isunordered
#undef signbit

inline float acos(float __x) {
	return ::acosf(__x);
}

inline long double acos(long double __x) {
	return ::acosl(__x);
}

inline float asin(float __x) {
	return ::asinf(__x);
}

inline long double asin(long double __x) {
	return ::asinl(__x);
}

inline float atan(float __x) {
	return ::atanf(__x);
}

inline long double atan(long double __x) {
	return ::atanl(__x);
}

inline float atan2(float __y, float __x) {
	return ::atan2f(__y, __x);
}

inline long double atan2(long double __y, long double __x) {
	return ::atan2l(__y, __x);
}

inline float ceil(float __x) {
	return ::ceilf(__x);
}

inline long double ceil(long double __x) {
	return ::ceill(__x);
}

inline float cos(float __x) {
	return ::cosf(__x);
}

inline long double cos(long double __x) {
	return ::cosl(__x);
}

inline float cosh(float __x) {
	return ::coshf(__x);
}

inline long double cosh(long double __x) {
	return ::coshl(__x);
}

inline float exp(float __x) {
	return ::expf(__x);
}

inline long double exp(long double __x) {
	return ::expl(__x);
}

inline float fabs(float __x) {
	return ::fabsf(__x);
}

inline long double fabs(long double __x) {
	return ::fabsl(__x);
}

inline float floor(float __x) {
	return ::floorf(__x);
}

inline long double floor(long double __x) {
	return ::floorl(__x);
}

inline float fmod(float __x, float __y) {
	return ::fmodf(__x, __y);
}

inline long double fmod(long double __x, long double __y) {
	return ::fmodl(__x, __y);
}

inline float frexp(float __x, int *__exp) {
	return ::frexpf(__x, __exp);
}

inline long double frexp(long double __x, int *__exp) {
	return ::frexpl(__x, __exp);
}

inline float ldexp(float __x, int __exp) {
	return ::ldexpf(__x, __exp);
}

inline long double ldexp(long double __x, int __exp) {
	return ::ldexpl(__x, __exp);
}

inline float log(float __x) {
	return ::logf(__x);
}

inline long double log(long double __x) {
	return ::logl(__x);
}

inline float log10(float __x) {
	return ::log10f(__x);
}

inline long double log10(long double __x) {
	return ::log10l(__x);
}

inline float modf(float __x, float *__iptr) {
	return ::modff(__x, __iptr);
}

inline long double modf(long double __x, long double *__iptr) {
	return ::modfl(__x, __iptr);
}

inline float pow(float __x, float __y) {
	return ::powf(__x, __y);
}

inline long double pow(long double __x, long double __y) {
	return ::powl(__x, __y);
}

#if __cplusplus < 201103L

inline double pow(double __x, int __i) {
	return ::pow(__x, (double)__i);
}

inline float pow(float __x, int __n) {
	return ::powf(__x, (float)__n);
}

inline long double pow(long double __x, int __n) {
	return ::powl(__x, (long double)__n);
}

#endif /* __cplusplus */

inline float sin(float __x) {
	return ::sinf(__x);
}

inline long double sin(long double __x) {
	return ::sinl(__x);
}

inline float sinh(float __x) {
	return ::sinhf(__x);
}

inline long double sinh(long double __x) {
	return ::sinhl(__x);
}

inline float sqrt(float __x) {
	return ::sqrtf(__x);
}

inline long double sqrt(long double __x) {
	return ::sqrtl(__x);
}

inline float tan(float __x) {
	return ::tanf(__x);
}

inline long double tan(long double __x) {
	return ::tanl(__x);
}

inline float tanh(float __x) {
	return ::tanhf(__x);
}

inline long double tanh(long double __x) {
	return ::tanhl(__x);
}

#if __cplusplus >= 201103L

inline int fpclassify(float __x) {
	return ::__fpclassifyf(__x);
}

inline int fpclassify(double __x) {
	return ::__fpclassifyd(__x);
}

inline int fpclassify(long double __x) {
	return ::__fpclassifyl(__x);
}

inline bool isfinite(float __x) {
	return ::__isfinitef(__x);
}

inline bool isfinite(double __x) {
	return ::__isfinite(__x);
}

inline bool isfinite(long double __x) {
	return ::__isfinitel(__x);
}

inline bool isinf(float __x) {
	return ::__isinff(__x);
}

inline bool isinf(double __x) {
	return ::__isinf(__x);
}

inline bool isinf(long double __x) {
	return ::__isinfl(__x);
}

inline bool isnan(float __x) {
	return ::__isnanf(__x);
}

inline bool isnan(double __x) {
	return ::__isnan(__x);
}

inline bool isnan(long double __x) {
	return ::__isnanl(__x);
}

inline bool isnormal(float __x) {
	return ::__isnormalf(__x);
}

inline bool isnormal(double __x) {
	return ::__isnormal(__x);
}

inline bool isnormal(long double __x) {
	return ::__isnormall(__x);
}

inline bool signbit(float __x) {
	return ::__signbitf(__x);
}

inline bool signbit(double __x) {
	return ::__signbit(__x);
}

inline bool signbit(long double __x) {
	return ::__signbitl(__x);
}

constexpr bool isgreater(float __x, float __y) {
	return __builtin_isgreater(__x, __y);
}

constexpr bool isgreater(double __x, double __y) {
	return __builtin_isgreater(__x, __y);
}

constexpr bool isgreater(long double __x, long double __y) {
	return __builtin_isgreater(__x, __y);
}

constexpr bool isgreaterequal(float __x, float __y) {
	return __builtin_isgreaterequal(__x, __y);
}

constexpr bool isgreaterequal(double __x, double __y) {
	return __builtin_isgreaterequal(__x, __y);
}

constexpr bool isgreaterequal(long double __x, long double __y) {
	return __builtin_isgreaterequal(__x, __y);
}

constexpr bool isless(float __x, float __y) {
	return __builtin_isless(__x, __y);
}

constexpr bool isless(double __x, double __y) {
	return __builtin_isless(__x, __y);
}

constexpr bool isless(long double __x, long double __y) {
	return __builtin_isless(__x, __y);
}

constexpr bool islessequal(float __x, float __y) {
	return __builtin_islessequal(__x, __y);
}

constexpr bool islessequal(double __x, double __y) {
	return __builtin_islessequal(__x, __y);
}

constexpr bool islessequal(long double __x, long double __y) {
	return __builtin_islessequal(__x, __y);
}

constexpr bool islessgreater(float __x, float __y) {
	return __builtin_islessgreater(__x, __y);
}

constexpr bool islessgreater(double __x, double __y) {
	return __builtin_islessgreater(__x, __y);
}

constexpr bool islessgreater(long double __x, long double __y) {
	return __builtin_islessgreater(__x, __y);
}

constexpr bool isunordered(float __x, float __y) {
	return __builtin_isunordered(__x, __y);
}

constexpr bool isunordered(double __x, double __y) {
	return __builtin_isunordered(__x, __y);
}

constexpr bool isunordered(long double __x, long double __y) {
	return __builtin_isunordered(__x, __y);
}

inline float acosh(float __x) {
	return ::acoshf(__x);
}

inline long double acosh(long double __x) {
	return ::acoshl(__x);
}

inline float asinh(float __x) {
	return ::asinhf(__x);
}

inline long double asinh(long double __x) {
	return ::asinhl(__x);
}

inline float atanh(float __x) {
	return ::atanhf(__x);
}

inline long double atanh(long double __x) {
	return ::atanhl(__x);
}

inline float cbrt(float __x) {
	return ::cbrtf(__x);
}

inline long double cbrt(long double __x) {
	return ::cbrtl(__x);
}

inline float copysign(float __x, float __y) {
	return ::copysignf(__x, __y);
}

inline long double copysign(long double __x, long double __y) {
	return ::copysignl(__x, __y);
}

inline float erf(float __x) {
	return ::erff(__x);
}

inline long double erf(long double __x) {
	return ::erfl(__x);
}

inline float erfc(float __x) {
	return ::erfcf(__x);
}

inline long double erfc(long double __x) {
	return ::erfcl(__x);
}

inline float exp2(float __x) {
	return ::exp2f(__x);
}

inline long double exp2(long double __x) {
	return ::exp2l(__x);
}

inline float expm1(float __x) {
	return ::expm1f(__x);
}

inline long double expm1(long double __x) {
	return ::expm1l(__x);
}

inline float fdim(float __x, float __y) {
	return ::fdimf(__x, __y);
}

inline long double fdim(long double __x, long double __y) {
	return ::fdiml(__x, __y);
}

inline float fma(float __x, float __y, float __z) {
	return ::fmaf(__x, __y, __z);
}

inline long double fma(long double __x, long double __y, long double __z) {
	return ::fmal(__x, __y, __z);
}

inline float fmax(float __x, float __y) {
	return ::fmaxf(__x, __y);
}

inline long double fmax(long double __x, long double __y) {
	return ::fmaxl(__x, __y);
}

inline float fmin(float __x, float __y) {
	return ::fminf(__x, __y);
}

inline long double fmin(long double __x, long double __y) {
	return ::fminl(__x, __y);
}

inline float hypot(float __x, float __y) {
	return ::hypotf(__x, __y);
}

inline long double hypot(long double __x, long double __y) {
	return ::hypotl(__x, __y);
}

inline int ilogb(float __x) {
	return ::ilogbf(__x);
}

inline int ilogb(long double __x) {
	return ::ilogbl(__x);
}

inline float lgamma(float __x) {
	return ::lgammaf(__x);
}

inline long double lgamma(long double __x) {
	return ::lgammal(__x);
}

inline long long llrint(float __x) {
	return ::llrintf(__x);
}

inline long long llrint(long double __x) {
	return ::llrintl(__x);
}

inline long long llround(float __x) {
	return ::llroundf(__x);
}

inline long long llround(long double __x) {
	return ::llroundl(__x);
}

inline float log1p(float __x) {
	return ::log1pf(__x);
}

inline long double log1p(long double __x) {
	return ::log1pl(__x);
}

inline float log2(float __x) {
	return ::log2f(__x);
}

inline long double log2(long double __x) {
	return ::log2l(__x);
}

inline float logb(float __x) {
	return ::logbf(__x);
}

inline long double logb(long double __x) {
	return ::logbl(__x);
}

inline long lrint(float __x) {
	return ::lrintf(__x);
}

inline long lrint(long double __x) {
	return ::lrintl(__x);
}

inline long lround(float __x) {
	return ::lroundf(__x);
}

inline long lround(long double __x) {
	return ::lroundl(__x);
}

inline float nearbyint(float __x) {
	return ::nearbyintf(__x);
}

inline long double nearbyint(long double __x) {
	return ::nearbyintl(__x);
}

inline float nextafter(float __x, float __y) {
	return ::nextafterf(__x, __y);
}

inline long double nextafter(long double __x, long double __y) {
	return ::nextafterl(__x, __y);
}

inline float nexttoward(float __x, long double __y) {
	return ::nexttowardf(__x, __y);
}

inline long double nexttoward(long double __x, long double __y) {
	return ::nexttowardl(__x, __y);
}

inline float remainder(float __x, float __y) {
	return ::remainderf(__x, __y);
}

inline long double remainder(long double __x, long double __y) {
	return ::remainderl(__x, __y);
}

inline float remquo(float __x, float __y, int *__pquo) {
	return ::remquof(__x, __y, __pquo);
}

inline long double remquo(long double __x, long double __y, int *__pquo) {
	return ::remquol(__x, __y, __pquo);
}

inline float rint(float __x) {
	return ::rintf(__x);
}

inline long double rint(long double __x) {
	return ::rintl(__x);
}

inline float round(float __x) {
	return ::roundf(__x);
}

inline long double round(long double __x) {
	return ::roundl(__x);
}

inline float scalbln(float __x, long __ex) {
	return ::scalblnf(__x, __ex);
}

inline long double scalbln(long double __x, long __ex) {
	return ::scalblnl(__x, __ex);
}

inline float scalbn(float __x, int __ex) {
	return ::scalbnf(__x, __ex);
}

inline long double scalbn(long double __x, int __ex) {
	return ::scalbnl(__x, __ex);
}

inline float tgamma(float __x) {
	return ::tgammaf(__x);
}

inline long double tgamma(long double __x) {
	return ::tgammal(__x);
}

inline float trunc(float __x) {
	return ::truncf(__x);
}

inline long double trunc(long double __x) {
	return ::truncl(__x);
}

#endif /* __cplusplus */
