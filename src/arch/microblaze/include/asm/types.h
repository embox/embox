/**
 * @file
 * @brief Types for microblaze architecture
 *
 * @date 11.11.2009
 * @author Anton Bondarev
 */

#ifndef MICROBLAZE_TYPES_H_
#define MICROBLAZE_TYPES_H_

#define __WORDSIZE 32

#ifndef __ASSEMBLER__

typedef signed char        __s8;
typedef unsigned char      __u8;
typedef signed short       __s16;
typedef unsigned short     __u16;
typedef signed int         __s32;
typedef unsigned int       __u32;
typedef long long          __s64;
typedef unsigned long long __u64;

typedef unsigned int  __size_t;
typedef signed int __ptrdiff_t;

#endif  /* __ASSEMBLER__*/

#endif /* MICROBLAZE_TYPES_H_ */
