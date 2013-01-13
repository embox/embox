/**
 * @file
 * @brief Basic numeric operations
 *
 * @date 04.06.10
 * @author Eldar Abusalimov
 */

#ifndef MATH_H_
#define MATH_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

#include <../lib/fdlibm/fdlibm.h>
/**
 * First 1 in val
 * @param val Value
 * @return Number of first 1 in Value
 */
extern int blog2(int val);

extern double fabs(double x);
extern float fabsf(float x);
extern long double fabsl(long double x);

extern double floor(double x);
extern float floorf(float x);
extern long double floorl(long double x);

extern double round(double x);
extern float roundf(float x);
extern long double roundl(long double x);

extern double ceil(double x);
extern float ceilf(float x);
extern long double ceill(long double x);

extern double copysign(double x, double y);
extern float copysignf(float x, float y);
extern long double copysignl(long double x, long double y);

extern int signbit(double x);
extern int signbitf(float x);
extern int signbitl(long double x);

extern double fmod(double x, double y);
extern float fmodf(float x, float y);
extern long double fmodl(long double x, long double y);

extern double modf(double x, double *ipart);
extern float modff(float x, float *ipart);
extern long double modfl(long double x, long double *ipart);

extern double pow(double x, double y);
extern float powf(float x, float y);
extern long double powl(long double x, long double y);

extern double log10(double x);
extern float log10f(float x);
extern long double log10l(long double x);

extern double sqrt(double x);
extern float sqrtf(float x);


__END_DECLS

/* FIXME max, min and clamp not a part of C Standard Library */
/**
 * Returns the greater of two values.
 *
 * @param a an argument
 * @param b another argument
 *
 * @return the larger of @c a and @c b
 */
#define max(a, b) __max(a, b)

/**
 * Returns the smaller of two values.
 *
 * @param a an argument
 * @param b another argument
 *
 * @return the smaller of @c a and @c b
 */
#define min(a, b) __min(a, b)

/**
 * Limits value to the specified boundary.
 *
 * @param val value to be clamped
 * @param lo lower bound
 * @param hi upper bound
 *
 * @return clamped value
 * @retval @c lo if @c val is less than @c lo
 * @retval @c hi if @c val is greater than @c hi
 * @retval @c val otherwise
 */
#define clamp(val, lo, hi) __clamp(val, lo, hi)

#define __min(a, b) \
	({                                             \
		typeof(a) __min_a = (a);               \
		typeof(b) __min_b = (b);               \
		__min_a < __min_b ? __min_a : __min_b; \
	})

#define __max(a, b) \
	({                                             \
		typeof(a) __max_a = (a);               \
		typeof(b) __max_b = (b);               \
		__max_a > __max_b ? __max_a : __max_b; \
	})

#define __clamp(val, lo, hi) \
	({ \
		typeof(val) __clamp_val = (val);        \
		typeof(lo)  __clamp_lo  = (lo);         \
		typeof(hi)  __clamp_hi  = (hi);         \
		__clamp_val < __clamp_lo ? __clamp_lo : \
		__clamp_val > __clamp_hi ? __clamp_hi : \
		__clamp_val;                            \
	})

#endif /* MATH_H_ */
