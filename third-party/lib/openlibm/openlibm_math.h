/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.07.24
 */

#ifndef THIRD_PARTY_LIB_OPENLIBM_MATH_H_
#define THIRD_PARTY_LIB_OPENLIBM_MATH_H_

#include <float.h>
#include <limits.h>
#include <sys/cdefs.h>

#define FP_ILOGB0        (-INT_MAX)
#define FP_ILOGBNAN      INT_MAX

#define HUGE_VAL         __builtin_huge_val()
#define HUGE_VALF        __builtin_huge_valf()
#define HUGE_VALL        __builtin_huge_vall()
#define INFINITY         __builtin_inff()
#define NAN              __builtin_nanf("")

#define MATH_ERRNO       1
#define MATH_ERREXCEPT   2
#define math_errhandling MATH_ERREXCEPT

#define FP_FAST_FMAF     1
// #define FP_FAST_FMA   1
// #define FP_FAST_FMAL  1

/* Symbolic constants to classify floating point numbers. */
#define FP_INFINITE      0x01
#define FP_NAN           0x02
#define FP_NORMAL        0x04
#define FP_SUBNORMAL     0x08
#define FP_ZERO          0x10

#define fpclassify(x)                                      \
	((sizeof(x) == sizeof(float))       ? __fpclassifyf(x) \
	    : (sizeof(x) == sizeof(double)) ? __fpclassifyd(x) \
	                                    : __fpclassifyl(x))

#define isfinite(x)                                      \
	((sizeof(x) == sizeof(float))       ? __isfinitef(x) \
	    : (sizeof(x) == sizeof(double)) ? __isfinite(x)  \
	                                    : __isfinitel(x))
#define isinf(x)                                      \
	((sizeof(x) == sizeof(float))       ? __isinff(x) \
	    : (sizeof(x) == sizeof(double)) ? isinf(x)    \
	                                    : __isinfl(x))
#define isnan(x)                                      \
	((sizeof(x) == sizeof(float))       ? __isnanf(x) \
	    : (sizeof(x) == sizeof(double)) ? isnan(x)    \
	                                    : __isnanl(x))
#define isnormal(x)                                      \
	((sizeof(x) == sizeof(float))       ? __isnormalf(x) \
	    : (sizeof(x) == sizeof(double)) ? __isnormal(x)  \
	                                    : __isnormall(x))

#define isgreater(x, y)      __builtin_isgreater((x), (y))
#define isgreaterequal(x, y) __builtin_isgreaterequal((x), (y))
#define isless(x, y)         __builtin_isless((x), (y))
#define islessequal(x, y)    __builtin_islessequal((x), (y))
#define islessgreater(x, y)  __builtin_islessgreater((x), (y))
#define isunordered(x, y)    __builtin_isunordered((x), (y))

#define signbit(x)                                      \
	((sizeof(x) == sizeof(float))       ? __signbitf(x) \
	    : (sizeof(x) == sizeof(double)) ? __signbit(x)  \
	                                    : __signbitl(x))

#define M_E        2.7182818284590452354  /* e */
#define M_LOG2E    1.4426950408889634074  /* log 2e */
#define M_LOG10E   0.43429448190325182765 /* log 10e */
#define M_LN2      0.69314718055994530942 /* log e2 */
#define M_LN10     2.30258509299404568402 /* log e10 */
#define M_PI       3.14159265358979323846 /* pi */
#define M_PI_2     1.57079632679489661923 /* pi/2 */
#define M_PI_4     0.78539816339744830962 /* pi/4 */
#define M_1_PI     0.31830988618379067154 /* 1/pi */
#define M_2_PI     0.63661977236758134308 /* 2/pi */
#define M_2_SQRTPI 1.12837916709551257390 /* 2/sqrt(pi) */
#define M_SQRT2    1.41421356237309504880 /* sqrt(2) */
#define M_SQRT1_2  0.70710678118654752440 /* 1/sqrt(2) */

#define MAXFLOAT   ((float)3.40282346638528860e+38)
#define HUGE       MAXFLOAT

#if FLT_EVAL_METHOD == 0
typedef float float_t;
typedef double double_t;
#elif FLT_EVAL_METHOD == 1
typedef double float_t;
typedef double double_t;
#elif FLT_EVAL_METHOD == 2
typedef long double float_t;
typedef long double double_t;
#endif

__BEGIN_DECLS

extern int signgam;

extern int __fpclassifyd(double);
extern int __fpclassifyf(float);
extern int __fpclassifyl(long double);
extern int __isfinitef(float);
extern int __isfinite(double);
extern int __isfinitel(long double);
extern int __isinff(float);
extern int __isinfl(long double);
extern int __isnanf(float);
extern int __isnanl(long double);
extern int __isnormalf(float);
extern int __isnormal(double);
extern int __isnormall(long double);
extern int __signbit(double);
extern int __signbitf(float);
extern int __signbitl(long double);

extern double acos(double);
extern double asin(double);
extern double atan(double);
extern double atan2(double, double);
extern double cos(double);
extern double sin(double);
extern double tan(double);

extern double cosh(double);
extern double sinh(double);
extern double tanh(double);

extern double exp(double);
extern double frexp(double, int *);
extern double ldexp(double, int);
extern double log(double);
extern double log10(double);
extern double modf(double, double *);

extern double pow(double, double);
extern double sqrt(double);

extern double ceil(double);
extern double fabs(double);
extern double floor(double);
extern double fmod(double, double);

extern double acosh(double);
extern double asinh(double);
extern double atanh(double);
extern double cbrt(double);
extern double erf(double);
extern double erfc(double);
extern double exp2(double);
extern double expm1(double);
extern double fma(double, double, double);
extern double hypot(double, double);
extern int ilogb(double);
extern int(isinf)(double);
extern int(isnan)(double);
extern double lgamma(double);
extern long long llrint(double);
extern long long llround(double);
extern double log1p(double);
extern double log2(double);
extern double logb(double);
extern long lrint(double);
extern long lround(double);
extern double nan(const char *);
extern double nextafter(double, double);
extern double remainder(double, double);
extern double remquo(double, double, int *);
extern double rint(double);

extern double j0(double);
extern double j1(double);
extern double jn(int, double);
extern double y0(double);
extern double y1(double);
extern double yn(int, double);

extern double copysign(double, double);
extern double fdim(double, double);
extern double fmax(double, double);
extern double fmin(double, double);
extern double nearbyint(double);
extern double round(double);
extern double scalbln(double, long);
extern double scalbn(double, int);
extern double tgamma(double);
extern double trunc(double);

extern int isinff(float);
extern int isnanf(float);

extern double lgamma_r(double, int *);
extern void sincos(double, double *, double *);

extern float acosf(float);
extern float asinf(float);
extern float atanf(float);
extern float atan2f(float, float);
extern float cosf(float);
extern float sinf(float);
extern float tanf(float);

extern float coshf(float);
extern float sinhf(float);
extern float tanhf(float);

extern float exp2f(float);
extern float expf(float);
extern float expm1f(float);
extern float frexpf(float, int *);
extern int ilogbf(float);
extern float ldexpf(float, int);
extern float log10f(float);
extern float log1pf(float);
extern float log2f(float);
extern float logf(float);
extern float modff(float, float *);

extern float powf(float, float);
extern float sqrtf(float);

extern float ceilf(float);
extern float fabsf(float);
extern float floorf(float);
extern float fmodf(float, float);
extern float roundf(float);

extern float erff(float);
extern float erfcf(float);
extern float hypotf(float, float);
extern float lgammaf(float);
extern float tgammaf(float);

extern float acoshf(float);
extern float asinhf(float);
extern float atanhf(float);
extern float cbrtf(float);
extern float logbf(float);
extern float copysignf(float, float);
extern long long llrintf(float);
extern long long llroundf(float);
extern long lrintf(float);
extern long lroundf(float);
extern float nanf(const char *);
extern float nearbyintf(float);
extern float nextafterf(float, float);
extern float remainderf(float, float);
extern float remquof(float, float, int *);
extern float rintf(float);
extern float scalblnf(float, long);
extern float scalbnf(float, int);
extern float truncf(float);

extern float fdimf(float, float);
extern float fmaf(float, float, float);
extern float fmaxf(float, float);
extern float fminf(float, float);

extern float dremf(float, float);
extern float j0f(float);
extern float j1f(float);
extern float jnf(int, float);
extern float y0f(float);
extern float y1f(float);
extern float ynf(int, float);

extern float lgammaf_r(float, int *);
extern void sincosf(float, float *, float *);

/**
 * Long double versions of math functions
 */
extern long double acoshl(long double);
extern long double acosl(long double);
extern long double asinhl(long double);
extern long double asinl(long double);
extern long double atan2l(long double, long double);
extern long double atanhl(long double);
extern long double atanl(long double);
extern long double cbrtl(long double);
extern long double ceill(long double);
extern long double copysignl(long double, long double);
extern long double coshl(long double);
extern long double cosl(long double);
extern long double erfcl(long double);
extern long double erfl(long double);
extern long double exp2l(long double);
extern long double expl(long double);
extern long double expm1l(long double);
extern long double fabsl(long double);
extern long double fdiml(long double, long double);
extern long double floorl(long double);
extern long double fmal(long double, long double, long double);
extern long double fmaxl(long double, long double);
extern long double fminl(long double, long double);
extern long double fmodl(long double, long double);
extern long double frexpl(long double value, int *);
extern long double hypotl(long double, long double);
extern int ilogbl(long double);
extern long double ldexpl(long double, int);
extern long double lgammal(long double);
extern long long llrintl(long double);
extern long long llroundl(long double);
extern long double log10l(long double);
extern long double log1pl(long double);
extern long double log2l(long double);
extern long double logbl(long double);
extern long double logl(long double);
extern long lrintl(long double);
extern long lroundl(long double);
extern long double modfl(long double, long double *);
extern long double nanl(const char *);
extern long double nearbyintl(long double);
extern long double nextafterl(long double, long double);
extern double nexttoward(double, long double);
extern float nexttowardf(float, long double);
extern long double nexttowardl(long double, long double);
extern long double powl(long double, long double);
extern long double remainderl(long double, long double);
extern long double remquol(long double, long double, int *);
extern long double rintl(long double);
extern long double roundl(long double);
extern long double scalblnl(long double, long);
extern long double scalbnl(long double, int);
extern long double sinhl(long double);
extern long double sinl(long double);
extern long double sqrtl(long double);
extern long double tanhl(long double);
extern long double tanl(long double);
extern long double tgammal(long double);
extern long double truncl(long double);
extern void sincosl(long double, long double *, long double *);
extern long double lgammal_r(long double, int *);

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

__END_DECLS

#endif /* THIRD_PARTY_LIB_OPENLIBM_MATH_H_ */
