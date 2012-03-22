/**
 * @file
 * @details compiler-specific definitions
 *
 * @date 21.03.2012
 * @author Vladimir Sokolov
 */

#ifndef _COMPILER_H__
#define _COMPILER_H__

/* Helps compiler to generate more optimal code.
 * It shows which branch has higher probability.
 * See gcc documentation for __builtin_expect()
 */
#define likely(x)	(__builtin_constant_p(x) ? !!(x) :  __builtin_expect(!!(x), 1))
#define unlikely(x)	(__builtin_constant_p(x) ? !!(x) :  __builtin_expect(!!(x), 0))

#endif /* _COMPILER_H__ */
