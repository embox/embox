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

#ifndef __ASSEMBLER__

#include <asm/types.h>

/* Those types are used in network subsystem.
 * Their main purpose is to show that some fields MUST be treated in a different way
 */
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;

/* bsd */
typedef unsigned char     u_char;
typedef unsigned short    u_short;
typedef unsigned int      u_int;
typedef unsigned long     u_long;

/* sysv */
typedef unsigned char     unchar;
typedef unsigned short    ushort;
typedef unsigned int      uint;
typedef unsigned long     ulong;

/* minix */
typedef unsigned char 		u8_t;
typedef unsigned short int 	u16_t;
typedef unsigned long int 	u32_t;

#endif /* __ASSEMBLER__ */

#endif /* COMPAT_LINUX_LINUX_TYPES_H_ */
