/**
 * @file
 * @brief Necessary arch-dependent typedefs.
 *
 * @date 01.07.2010
 * @author Anton Kozlov
 */

#ifndef ARM_TYPES_H_
#define ARM_TYPES_H_

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

typedef volatile unsigned int AT91_REG;// Hardware register definition

#endif /* __ASSEMBLER__ */

#endif /* ARM_TYPES_H_ */
