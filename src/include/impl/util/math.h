/**
 * @file
 * @brief Math utilities macros implementation.
 *
 * @date 12.06.2010
 * @author Eldar Abusalimov
 */

#ifndef UTIL_MATH_H_
# error "Do not include this file directly, use <util/math.h> instead!"
#endif /* UTIL_MATH_H_ */

#define __abs(x) __extension__ ({ \
	typeof(x) __abs_x = (x);    \
	__abs_x < 0 ? -__abs_x : __abs_x; \
})

#define __min(a, b) __extension__ ({ \
	typeof(a) __min_a = (a);       \
	typeof(b) __min_b = (b);       \
	__min_a < __min_b ? __min_a : __min_b; \
})

#define __max(a, b) __extension__ ({ \
	typeof(a) __max_a = (a);       \
	typeof(b) __max_b = (b);       \
	__max_a > __max_b ? __max_a : __max_b; \
})

#define __clamp(val, lo, hi) __extension__ ({ \
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
