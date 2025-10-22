/**
 * *****************************************************************************
 *  @file       basis_compiler.h
 *  @author     Baikal electronics SDK team
 *  @brief      BASIS third-part compiler macros
 *  @version    1.3
 *  @date       2025-06-30
 * *****************************************************************************
 *  @copyright Copyright (c) 2025 Baikal Electronics
 *
 *  This software is licensed under terms that can be found in the LICENSE file
 *  in the root directory of this software component.
 *  If no LICENSE file comes with this software, it is provided AS-IS.
 * *****************************************************************************
 */

#ifndef BASIS_COMPILER_H
#define BASIS_COMPILER_H

#include <stdint.h>

/* GNU Compiler */
#if defined (__GNUC__)

/* BASIS compiler specific defines */
#ifndef __ASM
#define __ASM                       __asm
#endif
#ifndef __INLINE
#define __INLINE                    inline
#endif
#ifndef __STATIC_INLINE
#define __STATIC_INLINE             static inline
#endif
#ifndef __STATIC_FORCEINLINE                 
#define __STATIC_FORCEINLINE        __attribute__((always_inline)) static inline
#endif                                           
#ifndef __NO_RETURN
#define __NO_RETURN                 __attribute__((__noreturn__))
#endif
#ifndef __USED
#define __USED                      __attribute__((used))
#endif
#ifndef __WEAK
#define __WEAK                      __attribute__((weak))
#endif
#ifndef __PACKED
#define __PACKED                    __attribute__((packed, aligned(4)))
#endif
#ifndef __PACKED_STRUCT
#define __PACKED_STRUCT             struct __attribute__((packed, aligned(4)))
#endif
#ifndef __PACKED_UNION
#define __PACKED_UNION              union __attribute__((packed, aligned(4)))
#endif
#ifndef __ALIGNED
#define __ALIGNED(x)                __attribute__((aligned(x)))
#endif
#ifndef __RESTRICT
#define __RESTRICT                  __restrict
#endif

/**
 * @brief Counts the number of leading zeros of a data value.
 * @param val The value to count leading zeros.
 * @return The number of leading zeros in the value.
 */
#define __CLZ(val)                  ((uint8_t)__builtin_clz(val))

/**
 * @brief Counts the number of trailing zeros of a data value.
 * @param val The value to count trailing zeros.
 * @return The number of trailing zeros in the value.
 */
#define __CTZ(val)                  ((uint8_t)__builtin_ctz(val))

/**
 * @brief A software memory barrier.
 */
#define barrier()                   __ASM volatile("":::"memory")

#else
#error Unknown compiler.
#endif

#endif /* BASIS_COMPILER_H */
