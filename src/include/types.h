/**
 * @file types.h
 *
 * @date 12.11.09
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */
#ifndef TYPES_H
#define TYPES_H

#include <asm/types.h>
#include <sys/types.h>

#ifndef __ASSEMBLER__

typedef __s8  int8_t;
typedef __s16 int16_t;
typedef __s32 int32_t;
typedef __s64 int64_t;

typedef __u8  uint8_t;
typedef __u16 uint16_t;
typedef __u32 uint32_t;
typedef __u64 uint64_t;

// FIXME try to avoid using these types -- Eldar
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;

typedef _Bool bool;

#define true 1
#define false 0

#define NULL ((void *)0x0)

#define offsetof(type, member) ((size_t) &((type *)0)->member)

#if 0
typedef __paddr_t paddr_t;
typedef __psize_t psize_t;
typedef __vaddr_t vaddr_t;
typedef __vsize_t vsize_t;
#endif

#endif /* __ASSEMBLER__ */

#endif /* TYPES_H */
