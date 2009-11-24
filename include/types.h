#ifndef TYPES_H
#define TYPES_H

#include "asm/types.h"
#include "sys/types.h"

#ifndef __ASSEMBLER__

#ifndef NULL
//TODO this maybe (void *)0x0
#define NULL    0x00000000
#endif

typedef __u8                    UINT8;
typedef __s8                    INT8;
typedef __u16                   UINT16;
typedef __s16                   INT16;
typedef __u32                   UINT32;
typedef __s32                   INT32;
typedef __u64                   UINT64;
typedef __s64                   INT64;

/* BSD */
typedef unsigned char           u_char;
typedef unsigned short          u_short;
typedef unsigned int            u_int;
typedef unsigned long           u_long;

/* SysV */
typedef unsigned char           unchar;
typedef unsigned short          ushort;
typedef unsigned int            uint;
typedef unsigned long           ulong;

typedef __u8                    u_int8_t;
typedef __s8                    int8_t;
typedef __u16                   u_int16_t;
typedef __s16                   int16_t;
typedef __u32                   u_int32_t;
typedef __s32                   int32_t;
typedef __u64                   u_int64_t;
typedef __s64                   int64_t;

typedef __u8                    u8_t;
typedef __u16                   u16_t;
typedef __u32                   u32_t;
typedef __u64                   u64_t;

typedef __u8                    uint8_t;
typedef __u16                   uint16_t;
typedef __u32                   uint32_t;
typedef __u64                   uint64_t;

typedef __u16                   __le16;
typedef __u16                   __be16;
typedef __u32                   __le32;
typedef __u32                   __be32;
typedef __u64                   __le64;
typedef __u64                   __be64;

typedef unsigned long           spinlock_t;
typedef __s32                   socklen_t;
typedef __u32                   gfp_t;

typedef struct {
        volatile int counter;
} atomic_t;



//TODO is this type posix?
#define WORD    unsigned long
#define HWRD    unsigned short int
#define BYTE    unsigned char
#define BOOL    int

#ifndef TRUE
#define TRUE    0x1
#endif

#ifndef FALSE
#define FALSE   0x0
#endif


#endif  // __ASSEMBLER__

#endif /* TYPES_H */
