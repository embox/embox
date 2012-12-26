/**
 * @file
 * @brief defines primitive system data types
 *
 * @date 12.11.09
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#ifndef TYPES_H_
#define TYPES_H_

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

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

#ifndef NULL
#ifndef __cplusplus
#define NULL ((void *)0)
#else /* __cplusplus */
#define NULL 0
#endif /* ! __cplusplus */
#endif

#endif /* __ASSEMBLER__ */

#endif /* TYPES_H_ */
