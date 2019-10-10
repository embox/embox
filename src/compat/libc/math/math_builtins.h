/**
 * @file
 *
 * @date May 23, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LIBC_MATH_STUB_H_
#define SRC_COMPAT_LIBC_MATH_STUB_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

extern double modf(double x, double *i_ptr);
extern double fmod(double x, double y);
extern int signbit(double x);
extern double fabs(double x);
extern float fabsf(float x);
extern long double fabsl(long double x);

extern double round(double x);
extern float roundf(float x);
extern long double roundl(long double x);

extern double pow(double x, double y);
extern float powf(float x, float y);
extern long double powl(long double x, long double y);

extern double log10(double x);

extern double ceil(double x);
extern float ceilf(float x);
extern long double ceill(long double x);

extern double floor(double x);
extern float floorf(float x);
extern long double floorl(long double x);

extern double acos(double x);
extern float acosf(float x);
extern long double acosl(long double x);

extern double asin(double x);
extern float asinf(float x);
extern long double asinl(long double x);

extern double atan(double x);
extern float atanf(float x);
extern long double atanl(long double x);

extern double atan2(double y, double x);
extern float atan2f(float y, float x);
extern long double atan2l(long double y, long double x);

extern double cos(double x);
extern float cosf(float x);
extern long double cosl(long double x);

extern double cosh(double x);
extern float coshf(float x);
extern long double coshl(long double x);

extern double exp(double x);
extern float expf(float x);
extern long double expl(long double x);

extern double frexp(double num, int *exp);
extern float frexpf(float num, int *exp);
extern long double frexpl(long double num, int *exp);

extern double ldexp(double x, int exp);
extern float ldexpf(float x, int exp);
extern long double ldexpl(long double x, int exp);

extern double log(double x);
extern float logf(float x);
extern long double logl(long double x);

extern double sin(double x);
extern float sinf(float x);
extern long double sinl(long double x);

extern double sinh(double x);
extern float sinhf(float x);
extern long double sinhl(long double x);

extern double sqrt(double x);
extern float sqrtf(float x);
extern long double sqrtl(long double x);

extern double tan(double x);
extern float tanf(float x);
extern long double tanl(long double x);

extern double tanh(double x);
extern float tanhf(float x);
extern long double tanhl(long double x);

extern long lrintf(float x);
extern long lrint(double x);
extern long lrintl(long double x);
extern long long llrintf(float x);
extern long long llrint(double x);
extern long long llrintl(long double x);
extern float rintf(float x);
extern double rint(double x);
extern long double rintl(long double x);

extern float log2f(float x);
extern double log2(double x);
extern long double log2l(long double x);

extern float exp2f(float x);
extern double exp2(double x);
extern long double exp2l(long double x);

extern float truncf(float x);
extern double trunc(double x);
extern long double truncl(long double x);

extern float fminf(float x, float y);
extern double fmin(double x, double y);
extern long double fminl(long double x, long double y);

extern float fmaxf(float x, float y);
extern double fmax(double x, double y);
extern long double fmaxl(long double x, long double y);

extern float copysignf(float x, float y);
extern double copysign(double x, double y);
extern long double copysignl(long double x, long double y);

extern long lroundf(float x);
extern long lround(double x);
extern long lroundl(long double x);
extern long long llroundf(float x);
extern long long llround(double x);
extern long long llroundl(long double x);

extern int __isnormal(double d);
extern int __isnormalf(float f);
extern int __isnormall(long double f);
extern int __isfinite(double d);
extern int __isfinitef(float f);
extern int __isfinitel(long double e);

extern double hypot(double x, double y);
extern float hypotf(float x, float y);
extern long double hypotl(long double x, long double y);

__END_DECLS

#define FP_NAN 0
#define FP_INFINITE 1
#define FP_NORMAL 2
#define FP_SUBNORMAL 3
#define FP_ZERO 4

#define fpclassify(x) \
	__builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, \
		FP_SUBNORMAL, FP_ZERO, x)


#define	isfinite(x)					\
    ((sizeof (x) == sizeof (float)) ? __isfinitef(x)	\
    : (sizeof (x) == sizeof (double)) ? __isfinite(x)	\
    : __isfinitel(x))
#define	isinf(x)					\
    ((sizeof (x) == sizeof (float)) ? __builtin_isinff(x)	\
    : (sizeof (x) == sizeof (double)) ? __builtin_isinf(x)	\
    : __builtin_isinfl(x))
#define	isnan(x)					\
    ((sizeof (x) == sizeof (float)) ? __builtin_isnanf(x)	\
    : (sizeof (x) == sizeof (double)) ? __builtin_isnan(x)	\
    : __builtin_isnanl(x))

#define	isnormal(x)					\
    ((sizeof (x) == sizeof (float)) ? __isnormalf(x)	\
    : (sizeof (x) == sizeof (double)) ? __isnormal(x)	\
    : __isnormall(x))

#define	HUGE_VALF	__builtin_huge_valf()
#define	HUGE_VALL	__builtin_huge_vall()
#define	INFINITY	__builtin_inff()
#define	NAN		__builtin_nanf("")

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
