/**
 * @file
 *
 * @date May 23, 2018
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LIBC_MATH_STUB_H_
#define SRC_COMPAT_LIBC_MATH_STUB_H_

extern double modf(double x, double *i_ptr);
extern double fmod(double x, double y);
extern int signbit(double x);
extern double fabs(double x);

extern double round(double x);
extern float roundf(float x);
extern long double roundl(long double x);

extern double pow(double x, double y);
extern double log10(double x);
extern double ceil(double x);
extern float ceilf(float x);
extern long double ceill(long double x);

extern double floor(double x);
extern float floorf(float x);
extern long double floorl(long double x);

#define	isnan(x) \
    ((sizeof (x) == sizeof (float)) ? __builtin_isnanf(x) \
    : (sizeof (x) == sizeof (double)) ? __builtin_isnan(x) \
    : __builtin_isnanl(x))

#endif /* SRC_COMPAT_LIBC_MATH_MATH_BUILTINS_H_ */
