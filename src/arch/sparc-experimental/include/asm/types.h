/**
 * @file
 * @brief Necessary arch-dependent typedefs.
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_TYPES_H_
#define SPARC_TYPES_H_

#define __WORDSIZE 32

#ifndef __ASSEMBLER__

typedef signed char    __s8;
typedef unsigned char  __u8;
typedef signed short   __s16;
typedef unsigned short __u16;
typedef signed int     __s32;
typedef unsigned int   __u32;
typedef long long      __s64;
typedef unsigned long long __u64;

typedef unsigned int __paddr_t;
typedef unsigned int __psize_t;
typedef unsigned int __vaddr_t;
typedef unsigned int __vsize_t;

typedef unsigned int  __size_t;
typedef signed int __ptrdiff_t;

#endif /* __ASSEMBLER__ */

#endif /* SPARC_TYPES_H_ */
