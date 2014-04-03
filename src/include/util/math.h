/*
 * @file
 * @brief Basic math which is not included in <math.h>.
 *
 * @date 04.06.10
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MATH_H_
#define UTIL_MATH_H_

/** @return the larger of @a a and @a b */
#define max(a, b) \
	({                                             \
		typeof(a) __max_a = (a);               \
		typeof(b) __max_b = (b);               \
		__max_a > __max_b ? __max_a : __max_b; \
	})

/** @return the smaller of @a a and @a b */
#define min(a, b) \
	({                                         \
		typeof(a) __min_a = (a);               \
		typeof(b) __min_b = (b);               \
		__min_a < __min_b ? __min_a : __min_b; \
	})

/** @return A @a val limited to a boundary specified by @a lo and @a hi. */
#define clamp(val, lo, hi) \
	({                                          \
		typeof(val) __clamp_val = (val);        \
		typeof(lo)  __clamp_lo  = (lo);         \
		typeof(hi)  __clamp_hi  = (hi);         \
		__clamp_val < __clamp_lo ? __clamp_lo : \
		__clamp_val > __clamp_hi ? __clamp_hi : \
		__clamp_val;                            \
	})

/** Suitable for checking an array index: [0, len). @return (lo <= val < hi) */
#define check_range(val, lo, hi) \
	({                                          \
		typeof(val) __check_val = (val);        \
		typeof(lo)  __check_lo  = (lo);         \
		typeof(hi)  __check_hi  = (hi);         \
		__check_val >= __check_lo &&            \
		__check_val < __check_hi;               \
	})

/** All inclusive version of #check_range(). @return (lo <= val <= hi) */
#define check_range_incl(val, lo, hi) \
	({                                          \
		typeof(val) __check_val = (val);        \
		typeof(lo)  __check_lo  = (lo);         \
		typeof(hi)  __check_hi  = (hi);         \
		__check_val >= __check_lo &&            \
		__check_val <= __check_hi;              \
	})

/** All exclusive version of #check_range(). @return (lo < val < hi) */
#define check_range_excl(val, lo, hi) \
	({                                          \
		typeof(val) __check_val = (val);        \
		typeof(lo)  __check_lo  = (lo);         \
		typeof(hi)  __check_hi  = (hi);         \
		__check_val > __check_lo &&             \
		__check_val < __check_hi;               \
	})

#endif /* UTIL_MATH_H_ */
