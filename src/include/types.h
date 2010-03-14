/**
 * @file
 *
 * @date 12.11.09
 * @author Nikolay Korotky
 * @author Eldar Abusalimov
 */

#ifndef TYPES_H
#define TYPES_H

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stdbool.h>

#include <sys/types.h>

/* FIXME try to avoid using these types -- Eldar*/
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
typedef __u64 __le64;
typedef __u64 __be64;

#ifndef NULL
#define NULL ((void *) 0x0)
#endif

#if 0
typedef __paddr_t paddr_t;
typedef __psize_t psize_t;
typedef __vaddr_t vaddr_t;
typedef __vsize_t vsize_t;
#endif

#endif /* __ASSEMBLER__ */

#endif /* TYPES_H */
