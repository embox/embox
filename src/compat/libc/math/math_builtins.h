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

__END_DECLS

#define FP_NAN         0
#define FP_INFINITE    1
#define FP_NORMAL      2
#define FP_SUBNORMAL   3
#define FP_ZERO        4

#define fpclassify(x) \
	__builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, \
		FP_SUBNORMAL, FP_ZERO, x)

#define isnan(x) \
	((sizeof (x) == sizeof (float)) ? __builtin_isnanf(x) \
			: (sizeof (x) == sizeof (double)) ? __builtin_isnan(x) \
					: __builtin_isnanl(x))

#endif /* SRC_COMPAT_LIBC_MATH_MATH_BUILTINS_H_ */
