/**
 * @file
 * @brief Math utilities macros implementation.
 *
 * @date 12.06.10
 * @author Eldar Abusalimov
 * @author Anton Kozlov
 * @author Malkovsky Nikolay
 */

#ifndef UTIL_MATH_IMPL_H_
#define UTIL_MATH_IMPL_H_

#define __abs(x) \
	({                                        \
		typeof(x) __abs_x = (x);          \
		__abs_x < 0 ? -__abs_x : __abs_x; \
	})

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

static inline int blog2(int val) {
	static unsigned long __log2_magic = 73743071;

	static unsigned char __log2_magic_offsets[] = {
	         0,  1,  2,  6,  3,
	        11,  7, 16,  4, 14,
	        12, 21,  8, 23, 17,
	        26, 31,  5, 10, 15,
	        13, 20, 22, 25, 30,
	         9, 19, 24, 29, 18,
	        28, 27
	};

	return __log2_magic_offsets[(__log2_magic * (val & (~val + 1))) >> 27];
}

#endif /* UTIL_MATH_IMPL_H_ */
