/**
 * @file
 * @brief Basic numeric operations
 *
 * @date 04.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MATH_H_
#define UTIL_MATH_H_

/**
 * Returns the absolute value of an argument. If the argument is not negative,
 * the argument is returned. If the argument is negative, the negation of the
 * argument is returned.
 *
 * @param x the argument whose absolute value is to be determined
 *
 * @return the absolute value of the argument
 */
#define abs(x) __extension__ ({ \
	typeof(x) __abs_x = (x);    \
	__abs_x < 0 ? -__abs_x : __abs_x; \
})

/**
 * Returns the greater of two values.
 *
 * @param a an argument
 * @param b another argument
 *
 * @return the larger of @c a and @c b
 */
#define min(a, b) __extension__ ({ \
	typeof(a) __min_a = (a);       \
	typeof(b) __min_b = (b);       \
	__min_a < __min_b ? __min_a : __min_b; \
})

/**
 * Returns the smaller of two values.
 *
 * @param a an argument
 * @param b another argument
 *
 * @return the smaller of @c a and @c b
 */
#define max(a, b) __extension__ ({ \
	typeof(a) __max_a = (a);       \
	typeof(b) __max_b = (b);       \
	__max_a > __max_b ? __max_a : __max_b; \
})

/**
 * Limits value to the specified boundary.
 *
 * @param val value to be clamped
 * @param lo lower bound
 * @param hi upper bound
 *
 * @return clamped value
 * @retval @c lo if @c val < @c lo
 * @retval @c hi if @c val > @c hi
 * @retval @c val otherwise
 */
#define clamp(val, lo, hi) __extension__ ({ \
	typeof(val) __clamp_val = (val);        \
	typeof(lo)  __clamp_lo  = (lo);         \
	typeof(hi)  __clamp_hi  = (hi);         \
	__clamp_val < __clamp_lo ? __clamp_lo : \
	__clamp_val > __clamp_hi ? __clamp_hi : \
	__clamp_val; \
})

#endif /* UTIL_MATH_H_ */
