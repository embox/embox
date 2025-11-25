/**
 * @file
 *
 * @date May 23, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LIBC_MATH_STUB_H_
#define SRC_COMPAT_LIBC_MATH_STUB_H_

#include <sys/cdefs.h>

#if !defined(FLT_EVAL_METHOD) && defined(__FLT_EVAL_METHOD__)
	#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
#endif
#if defined FLT_EVAL_METHOD
	#if FLT_EVAL_METHOD == 0
		typedef float  float_t;
		typedef double double_t;
	#elif FLT_EVAL_METHOD == 1
		typedef double float_t;
		typedef double double_t;
	#elif FLT_EVAL_METHOD == 2
		typedef long double float_t;
		typedef long double double_t;
	#else
		#error "Unknown FLT_EVAL_METHOD value"
	#endif
#else
	typedef float  float_t;
	typedef double double_t;
#endif

__BEGIN_DECLS

extern double modf(double x, double *iptr);
extern double fmod( double x, double y );

extern double log(double x);
extern float logf(float x);
extern long double logl(long double x);

extern double log10(double x);
extern double pow(double x, double y);
extern float powf(float x, float y);
extern long double powl(long double x, long double y);

extern double exp(double x);
extern float expf(float x);
extern long double expl(long double x);

extern double sqrt(double x);
extern float sqrtf(float x);
extern long double sqrtl(long double x);

extern long double floorl(long double x);
extern double floor(double x);
extern float floorf(float x);

extern long double ceill(long double x);
extern double ceil(double x);
extern float ceilf(float x);

extern float roundf(float x);
extern double round(double x);
extern long double roundl(long double x);

extern int __signbit_float(float x);
extern int __signbit_double(double x);
extern int __signbit_long_double(long double x);
#define signbit(x) (sizeof(x) == sizeof(float) ? __signbit_float(x) : \
		(sizeof(x) == sizeof(double) ? __signbit_double(x) : __signbit_long_double(x)))

extern double fabs(double x);
extern float fabsf(float x);
extern long double fabsl(long double x);

#define acos(x) __builtin_acos(x)
#define acosf(x) __builtin_acosf(x)
#define acosl(x) __builtin_acosl(x)

#define asin(x) __builtin_asin(x)
#define asinf(x) __builtin_asinf(x)
#define asinl(x) __builtin_asinl(x)

#define atan(x) __builtin_atan(x)
#define atanf(x) __builtin_atanf(x)
#define atanl(x) __builtin_atanl(x)

#define atan2(...) __builtin_atan2(__VA_ARGS__)
#define atan2f(...) __builtin_atan2f(__VA_ARGS__)
#define atan2l(...) __builtin_atan2l(__VA_ARGS__)

#define cos(x) __builtin_cos(x)
#define cosf(x) __builtin_cosf(x)
#define cosl(x) __builtin_cosl(x)

#define cosh(x) __builtin_cosh(x)
#define coshf(x) __builtin_coshf(x)
#define coshl(x) __builtin_coshl(x)

#define atanh(x) __builtin_atanh(x)
#define atanhf(x) __builtin_atanhf(x)
#define atanhl(x) __builtin_atanhl(x)

#define frexp(x, exp) __builtin_frexp(x, exp)
#define frexpf(x, exp) __builtin_frexpf(x, exp)
#define frexpl(x, exp) __builtin_frexpl(x, exp)

#define ldexp(x, exp) __builtin_ldexp(x, exp)
#define ldexpf(x, exp) __builtin_ldexpf(x, exp)
#define ldexpl(x, exp) __builtin_ldexpl(x, exp)

#if 0
#define log(x) __builtin_log(x)
#endif

#define sin(x) __builtin_sin(x)
#define sinf(x) __builtin_sinf(x)
#define sinl(x) __builtin_sinl(x)

#define sinh(x) __builtin_sinh(x)
#define sinhf(x) __builtin_sinhf(x)
#define sinhl(x) __builtin_sinhl(x)

#if 0
#define sqrt(x) __builtin_sqrt(x)
#endif

#define tan(x) __builtin_tan(x)
#define tanf(x) __builtin_tanf(x)
#define tanl(x) __builtin_tanl(x)

#define tanh(x) __builtin_tanh(x)
#define tanhf(x) __builtin_tanhf(x)
#define tanhl(x) __builtin_tanhl(x)

#define lrintf(x) __builtin_lrintf(x)
#define lrint(x) __builtin_lrint(x)
#define lrintl(x) __builtin_lrintl(x)
#define llrintf(x) __builtin_llrintf(x)
#define llrint(x) __builtin_llrint(x)
#define llrintl(x) __builtin_llrintl(x)
#define rintf(x) __builtin_rintf(x)
#define rint(x) __builtin_rint(x)
#define rintl(x) __builtin_rintl(x)

#define log2f(x) __builtin_log2f(x)
#define log2(x) __builtin_log2(x)
#define log2l(x) __builtin_log2l(x)

#define exp2f(x) __builtin_exp2f(x)
#define exp2(x) __builtin_exp2(x)
#define exp2l(x) __builtin_exp2l(x)

#define truncf(x) __builtin_truncf(x)
#define trunc(x) __builtin_trunc(x)
#define truncl(x) __builtin_truncl(x)

#define fminf(x, y) __builtin_fminf(x, y)
#define fmin(x, y) __builtin_fmin(x, y)
#define fminl(x, y) __builtin_fminl(x, y)

#define fmaxf(x, y) __builtin_fmaxf(x, y)
#define fmax(x, y) __builtin_fmax(x, y)
#define fmaxl(x, y) __builtin_fmaxl(x, y)

#define copysignf(x, y) __builtin_copysignf(x, y)
#define copysign(x, y) __builtin_copysign(x, y)
#define copysignl(x, y) __builtin_copysignl(x, y)

#define lroundf(x) __builtin_lroundf(x)
#define lround(x) __builtin_lround(x)
#define lroundl(x) __builtin_lroundl(x)
#define llroundf(x) __builtin_llroundf(x)
#define llround(x) __builtin_llround(x)
#define llroundl(x) __builtin_llroundl(x)

#define __isnormal(x) __builtin___isnormal(x)
#define __isnormalf(x) __builtin___isnormalf(x)
#define __isnormall(x) __builtin___isnormall(x)
#define __isfinite(x) __builtin___isfinite(x)
#define __isfinitef(x) __builtin___isfinitef(x)
#define __isfinitel(x) __builtin___isfinitel(x)

#define hypot(x, y) __builtin_hypot(x, y)
#define hypotf(x, y) __builtin_hypotf(x, y)
#define hypotl(x, y) __builtin_hypotl(x, y)

#define cbrt(x) __builtin_cbrt(x)
#define cbrtf(x) __builtin_cbrtf(x)
#define cbrtl(x) __builtin_cbrtl(x)

__END_DECLS

#define FP_NAN 0
#define FP_INFINITE 1
#define FP_NORMAL 2
#define FP_SUBNORMAL 3
#define FP_ZERO 4

#define fpclassify(x) \
	__builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, \
		FP_SUBNORMAL, FP_ZERO, x)

#ifdef __clang__
#define isfinite(x) __isfinite(x)
#define isnormal(x) __isnormal(x)
#define isinf(x) __builtin_isinf(x)
#define isnan(x) __builtin_isnan(x)
#else
#define isfinite(x)                                      \
	((sizeof(x) == sizeof(float))       ? __isfinitef(x) \
	    : (sizeof(x) == sizeof(double)) ? __isfinite(x)  \
	                                    : __isfinitel(x))

#define isnormal(x)                                      \
	((sizeof(x) == sizeof(float))       ? __isnormalf(x) \
	    : (sizeof(x) == sizeof(double)) ? __isnormal(x)  \
	                                    : __isnormall(x))

#define isinf(x)                                              \
	((sizeof(x) == sizeof(float))       ? __builtin_isinff(x) \
	    : (sizeof(x) == sizeof(double)) ? __builtin_isinf(x)  \
	                                    : __builtin_isinfl(x))

#define isnan(x)                                              \
	((sizeof(x) == sizeof(float))       ? __builtin_isnanf(x) \
	    : (sizeof(x) == sizeof(double)) ? __builtin_isnan(x)  \
	                                    : __builtin_isnanl(x))
#endif

#define HUGE_VAL   (__builtin_huge_val())
#define HUGE_VALF  __builtin_huge_valf()
#define HUGE_VALL  __builtin_huge_vall()
#define INFINITY   __builtin_inff()
#define NAN        __builtin_nanf("")

#define	M_E		2.7182818284590452354	/* e */
#define	M_LOG2E		1.4426950408889634074	/* log 2e */
#define	M_LOG10E	0.43429448190325182765	/* log 10e */
#define	M_LN2		0.69314718055994530942	/* log e2 */
#define	M_LN10		2.30258509299404568402	/* log e10 */
#define	M_PI		3.14159265358979323846	/* pi */
#define	M_PI_2		1.57079632679489661923	/* pi/2 */
#define	M_PI_4		0.78539816339744830962	/* pi/4 */
#define	M_1_PI		0.31830988618379067154	/* 1/pi */
#define	M_2_PI		0.63661977236758134308	/* 2/pi */
#define	M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#define	M_SQRT2		1.41421356237309504880	/* sqrt(2) */
#define	M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

#define	MAXFLOAT	((float)3.40282346638528860e+38)

#endif /* SRC_COMPAT_LIBC_MATH_MATH_BUILTINS_H_ */
