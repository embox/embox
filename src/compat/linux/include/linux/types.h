/**
 * @file
 * @brief defines primitive system data types
 *
 * @date 12.11.09
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#ifndef COMPAT_LINUX_LINUX_TYPES_H_
#define COMPAT_LINUX_LINUX_TYPES_H_

#include <stdint.h>

#ifndef __ASSEMBLER__

#include <asm/types.h>

/**
 * Linux-specific macros
 */
#undef __bitwise
#undef __bitwise

#ifdef __CHECKER__
#define __bitwise __attribute__((bitwise))
#define __force   __attribute__((force))
#else
#define __bitwise
#define __force
#endif

/**
 * Those types are used in network subsystem.
 * Their main purpose is to show that some fields MUST be treated in a different way
 */
typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;

typedef uint32_t u32;

/* bsd */
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

/* sysv */
typedef unsigned char unchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

/* minix */
typedef unsigned char u8_t;
typedef unsigned short int u16_t;
typedef unsigned long int u32_t;

typedef uint32_t dma_addr_t;
typedef uint32_t phys_addr_t;

#endif /* __ASSEMBLER__ */

#endif /* COMPAT_LINUX_LINUX_TYPES_H_ */
