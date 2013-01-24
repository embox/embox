/*
 * @file
 * @brief max/min mocros definitions
 *
 * @details this is not standard math.h file. This is <util/math.h> for macros
 * max and min
 *
 * @date Jan 18, 2013
 * @author: Anton Bondarev
 */

#ifndef UTIL_MATH_H_
#define UTIL_MATH_H_


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
	({                                              \
		typeof(val) __clamp_val = (val);        \
		typeof(lo)  __clamp_lo  = (lo);         \
		typeof(hi)  __clamp_hi  = (hi);         \
		__clamp_val < __clamp_lo ? __clamp_lo : \
		__clamp_val > __clamp_hi ? __clamp_hi : \
		__clamp_val;                            \
	})


#endif /* UTIL_MATH_H_ */
