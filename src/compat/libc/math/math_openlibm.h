/**
 * @file
 * @date: Jan 9, 2018
 *
 * @author Anton Bondarev
 */

#ifndef SRC_COMPAT_LIBC_MATH_MATH_OPENLIBM_H_
#define SRC_COMPAT_LIBC_MATH_MATH_OPENLIBM_H_

/* OpenLibm doesn't define float_t and double_t somewhy.
 * The only place I found double_t is here, but it is commented out:
 *
 * openlibm-0.7.0/include/openlibm_math.h:160://typedef	__double_t	double_t;
 *
 * So, declare these types here based on FLT_EVAL_METHOD macro.
 */
#if !defined(FLT_EVAL_METHOD) && defined(__FLT_EVAL_METHOD__)
	#define FLT_EVAL_METHOD __FLT_EVAL_METHOD__
#endif
#if defined FLT_EVAL_METHOD
	#if FLT_EVAL_METHOD == 0
		typedef float  float_t;
		typedef double double_t;
	#elif FLT_EVAL_METHOD == 1
		typedef double float_t;
		typedef double double_t;
	#elif FLT_EVAL_METHOD == 2
		typedef long double float_t;
		typedef long double double_t;
	#else
		#error "Unknown FLT_EVAL_METHOD value"
	#endif
#else
	typedef float  float_t;
	typedef double double_t;
#endif

#include <module/third_party/lib/OpenLibm.h>

#endif /* SRC_COMPAT_LIBC_MATH_MATH_OPENLIBM_H_ */
