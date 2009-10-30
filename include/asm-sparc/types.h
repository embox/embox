#ifndef _TYPES_H_
#define _TYPES_H_

#ifndef NULL
//TODO this maybe (void *)0x0
#define NULL  0x00000000
#endif

#if 0
#ifndef TRUE
#define TRUE    0x1
#endif

#ifndef FALSE
#define FALSE   0x0
#endif
#endif

#ifndef __ASSEMBLER__

typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned char UINT8;
typedef signed int INT32;
typedef signed short INT16;
typedef signed char CHAR;
typedef unsigned char BYTE;
typedef int BOOL;
typedef INT32 socklen_t;
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;

#ifdef _TEST_SYSTEM_
typedef int size_t;

typedef unsigned gfp_t;

typedef struct {
	volatile int counter;
} atomic_t;

#endif

#if 0
typedef void * PVOID;
#endif

// unsigned types
#define WORD    unsigned long
#define HWRD    unsigned short int
#define BYTE    unsigned char



#if 0
// volatile types
#define VWORD   volatile WORD
#define VHWRD   volatile HWRD
#define VBYTE   volatile BYTE
#define VCHAR   volatile char
#define VINT    volatile int

// volatile pointers types
#define PVWORD                VWORD*
#define PVHWRD                VHWRD*
#define PVBYTE                VBYTE*

// access to value by address
#define AWORD(addr)           *((PVWORD)(addr))
#define AHWRD(addr)           *((PVHWRD)(addr))
#define ABYTE(addr)           *((PVBYTE)(addr))
#endif
#endif  // ifndef __ASSEMBLER__

#endif // _TYPES_H_
