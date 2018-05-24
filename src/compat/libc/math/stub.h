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
extern double roundl(double x);
extern double pow(double x, double y);
extern double log10(double x);
extern double ceill(double x);

extern double floor(double x);
extern float floorf(float x);
extern long double floorl(long double x);

#endif /* SRC_COMPAT_LIBC_MATH_STUB_H_ */
