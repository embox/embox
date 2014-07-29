/**
 * @file
 * @brief Basic numeric operations
 *
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef MATH_H_
#define MATH_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

/* Absolute value of floating-point number */
extern double fabs(double x);
extern float fabsf(float x);
extern long double fabsl(long double x);

/* Largest integral value not greater than argument */
extern double floor(double x);
extern float floorf(float x);
extern long double floorl(long double x);

/* Round to nearest integer, away from zero */
extern double round(double x);
extern float roundf(float x);
extern long double roundl(long double x);

/* Round to nearest integer, away from zero */
extern long int lround(double x);
extern long int lroundf(float x);
extern long int lroundl(long double x);

/* Round to nearest integer, away from zero */
extern long long int llround(double x);
extern long long int llroundf(float x);
extern long long int llroundl(long double x);

/* Ceiling function: smallest integral value not less than argument */
extern double ceil(double x);
extern float ceilf(float x);
extern long double ceill(long double x);

/* Copy sign of a number */
extern double copysign(double x, double y);
extern float copysignf(float x, float y);
extern long double copysignl(long double x, long double y);

/* x is a NaN value */
extern int isnan(double x);
extern int isnanf(float x);
extern int isnanl(long double x);

/* x is a infinity value */
extern int isinf(double x);
extern int isinff(float x);
extern int isinfl(long double x);

/* x is a finite value */
extern int finite(double x);
extern int finitef(float x);
extern int finitel(long double x);
static inline int isfinite(double x) { return finite(x); }
static inline int isfinitef(float x) { return finitel(x); }
static inline int isfinitel(long double x) { return finitef(x); }

/*  Test sign of a real floating-point number */
extern int signbit(double x);
extern int signbitf(float x);
extern int signbitl(long double x);

/* Floating-point remainder functions */
extern double fmod(double x, double y);
extern float fmodf(float x, float y);
extern long double fmodl(long double x, long double y);

/* Extract signed integral and fractional values from floating-point number */
extern double modf(double x, double *ipart);
extern float modff(float x, float *ipart);
extern long double modfl(long double x, long double *ipart);

/* Power functions */
extern double pow(double x, double y);
extern float powf(float x, float y);
extern long double powl(long double x, long double y);

/* Base-10 logarithmic functions */
extern double log10(double x);
extern float log10f(float x);
extern long double log10l(long double x);

/* Natural logarithmic functions */
extern double log(double x);
extern float logf(float x);
extern long double logl(long double x);

/* Square root functions */
extern double sqrt(double x);
extern float sqrtf(float x);
extern long double sqrtl(long double x);

extern double acos(double);
extern double asin(double);
extern double atan(double);
extern double atan2(double, double);
extern double cos(double);
extern double sin(double);
//extern double tan(double);

extern double cosh(double);
extern double sinh(double);
//extern double tanh(double);

extern double exp(double);
extern double frexp(double, int *);
//extern double ldexp(double, int);

extern int isnan(double);
extern int isnanl(long double);
extern int isnanf(float);
extern int finite(double);
extern int finitel(long double);
extern int finitef(float);

extern double hypot(double x, double y);

__END_DECLS

#endif /* MATH_H_ */
