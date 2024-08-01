/**
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 30.07.24
 */

#ifndef COMPAT_LINUX_BITS_TYPES_H_
#define COMPAT_LINUX_BITS_TYPES_H_

#include <stdint.h>

typedef uint8_t __uint8_t;
typedef uint16_t __uint16_t;
typedef uint32_t __uint32_t;
typedef uint64_t __uint64_t;

typedef int8_t __int8_t;
typedef int16_t __int16_t;
typedef int32_t __int32_t;
typedef int64_t __int64_t;

typedef uintmax_t __uintmax_t;
typedef intmax_t __intmax_t;

typedef uintptr_t __uintptr_t;
typedef intptr_t __intptr_t;

#endif /* COMPAT_LINUX_BITS_TYPES_H_ */
