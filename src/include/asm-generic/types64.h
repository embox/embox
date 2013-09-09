/**
 * @file
 *
 * @date Aug 28, 2013
 * @author: Anton Bondarev
 */

#ifndef ASM_GENERIC_TYPES64_H_
#define ASM_GENERIC_TYPES64_H_

#define __WORDSIZE         64

#ifndef __ASSEMBLER__

typedef signed char        __s8;
typedef unsigned char      __u8;
typedef signed short       __s16;
typedef unsigned short     __u16;
typedef signed int         __s32;
typedef unsigned int       __u32;
typedef long long          __s64;
typedef unsigned long long __u64;

typedef unsigned int      __size_t;
typedef signed int        __ptrdiff_t;

typedef unsigned long int __uintptr_t;
typedef long int          __intptr_t;

typedef __s64             __s_fast;
typedef __u64             __u_fast;

#endif /* __ASSEMBLER__ */

#endif /* ASM_GENERIC_TYPES64_H_ */
