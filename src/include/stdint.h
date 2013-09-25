/**
 * @file
 * @brief ISO C99: 7.18 Integer types.
 *
 * @date 09.03.10
 * @author Eldar Abusalimov
 */

#ifndef STDINT_H_
#define STDINT_H_

#include <asm/types.h>

#ifndef __WORDSIZE
# error "You must define __WORDSIZE in asm/types.h"
#endif

#ifndef __ASSEMBLER__

typedef __s8  int8_t;
typedef __s16 int16_t;
typedef __s32 int32_t;
typedef __s64 int64_t;

typedef __u8  uint8_t;
typedef __u16 uint16_t;
typedef __u32 uint32_t;
typedef __u64 uint64_t;

typedef __s8  int_least8_t;
typedef __s16 int_least16_t;
typedef __s32 int_least32_t;
typedef __s64 int_least64_t;

typedef __u8  uint_least8_t;
typedef __u16 uint_least16_t;
typedef __u32 uint_least32_t;
typedef __u64 uint_least64_t;


typedef __s_fast int_fast8_t;
typedef __s_fast int_fast16_t;
typedef __s_fast int_fast32_t;
typedef __s_fast int_fast64_t;

typedef __u_fast uint_fast8_t;
typedef __u_fast uint_fast16_t;
typedef __u_fast uint_fast32_t;
typedef __u_fast uint_fast64_t;

typedef int64_t intmax_t;
typedef uint64_t uintmax_t;


typedef __intptr_t intptr_t;

typedef __uintptr_t uintptr_t;


/* XXX these macros should be defined as INT64_MAX/MIN and UINT64_MAX/MIN --Alexander */
#define INTMAX_MIN  (-INTMAX_MAX - 1)
#define INTMAX_MAX  9223372036854775807LL

#define UINTMAX_MAX 18446744073709551615ULL

#define INT16_MAX 32767

#include <stdint-gcc.h>

#endif /* __ASSEMBLER__ */

#endif /* STDINT_H_ */
