/**
 * @file
 *
 * @date 16 jule 2015
 * @author: Anton Bondarev
 */

#ifndef SRC_ARCH_MULTICLET_INCLUDE_ASM_LIMITS_H_
#define SRC_ARCH_MULTICLET_INCLUDE_ASM_LIMITS_H_

#include <asm-generic/limits32.h>

#undef LLONG_MAX
#define LLONG_MAX LONG_MAX

#undef LLONG_MIN
#define LLONG_MIN LONG_MIN

#undef ULLONG_MAX
#define ULLONG_MAX ULONG_MAX

#endif /* SRC_ARCH_MULTICLET_INCLUDE_ASM_LIMITS_H_ */
