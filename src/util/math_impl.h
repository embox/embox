/**
 * @file
 * @brief Math utilities macros implementation.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MATH_IMPL_H_
#define UTIL_MATH_IMPL_H_

#define __abs(x) \
	({ \
		typeof(x) __abs_x = (x); \
		__abs_x < 0              \
				? -__abs_x       \
				:  __abs_x;      \
	})

#define __min(a, b) \
	({ \
		typeof(a) __min_a = (a); \
		typeof(b) __min_b = (b); \
		__min_a < __min_b        \
				? __min_a        \
				: __min_b;       \
	})

#define __max(a, b) \
	({ \
		typeof(a) __max_a = (a); \
		typeof(b) __max_b = (b); \
		__max_a > __max_b        \
				? __max_a        \
				: __max_b;       \
	})

#define __clamp(val, lo, hi) \
	({ \
		typeof(val) __clamp_val = (val);        \
		typeof(lo)  __clamp_lo  = (lo);         \
		typeof(hi)  __clamp_hi  = (hi);         \
		__clamp_val < __clamp_lo ? __clamp_lo : \
		__clamp_val > __clamp_hi ? __clamp_hi : \
		__clamp_val; \
	})

static inline int blog2(int val) {
	int j = 0;
	while (!(val & 1)) {
		j++;
		val >>= 1;
	}
	return j;
}

#endif /* UTIL_MATH_IMPL_H_ */
