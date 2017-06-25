/**
 * @file
 *
 * @date Aug 28, 2013
 * @author: Anton Bondarev
 */

#ifndef ASM_GENERIC_TYPES16_H_
#define ASM_GENERIC_TYPES16_H_

#define __WORDSIZE         16

#ifndef __ASSEMBLER__

typedef signed char        __s8;
typedef unsigned char      __u8;
typedef signed int         __s16;
typedef unsigned int       __u16;
typedef signed long        __s32;
typedef unsigned long      __u32;
typedef long long          __s64;
typedef unsigned long long __u64;

typedef unsigned int      __size_t;
typedef signed int        __ptrdiff_t;

typedef unsigned int      __uintptr_t;
typedef int               __intptr_t;

typedef __s16             __s_fast;
typedef __u16             __u_fast;

#endif /* __ASSEMBLER__ */

#endif /* ASM_GENERIC_TYPES16_H_ */
