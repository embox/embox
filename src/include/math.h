/**
 * @file
 * @brief Basic numeric operations
 *
 * @date 04.06.10
 * @author Eldar Abusalimov
 */

#ifndef MATH_H_
#define MATH_H_

__BEGIN_DECLS

/**
 * First 1 in val
 * @param val Value
 * @return Number of first 1 in Value
 */
extern int blog2(int val);

extern double floor(double x);
extern float floorf(float x);
extern long double floorl(long double x);

extern double pow(double x, double y);
extern float powf(float x, float y);
extern long double powl(long double x, long double y);

extern double sqrt(double x);


__END_DECLS

#define CEILING_POS(X) ((X-(int)(X)) > 0 ? (int)(X+1) : (int)(X))
#define CEILING_NEG(X) ((X-(int)(X)) < 0 ? (int)(X-1) : (int)(X))
#define CEILING(X) ( ((X) > 0) ? CEILING_POS(X) : CEILING_NEG(X) )

static inline double ceil(double x) {
	return CEILING(x);
}


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
