/**
 * @file
 * @brief Necessary arch-dependent typedefs.
 *
 * @date 01.07.10
 * @author Anton Kozlov
 */

#ifndef ARM_TYPES_H_
#define ARM_TYPES_H_

#define __WORDSIZE 16

#ifndef __ASSEMBLER__

typedef signed char    __s8;
typedef unsigned char  __u8;
typedef signed int   __s16;
typedef unsigned int __u16;
typedef signed long     __s32;
typedef unsigned long   __u32;
typedef long long      __s64;
typedef unsigned long long __u64;

#if 0
typedef unsigned int __paddr_t;
typedef unsigned int __psize_t;
typedef unsigned int __vaddr_t;
typedef unsigned int __vsize_t;
#endif

typedef unsigned int  __size_t;
typedef signed int __ptrdiff_t;

typedef unsigned int __uintptr_t;
typedef int __intptr_t;

#endif /* __ASSEMBLER__ */

#endif /* ARM_TYPES_H_ */
