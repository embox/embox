
#ifndef STDINT
#define	STDINT

#include "mc-stdio.h"

#define	Multiclet
#define	NULL (void*)0
#define FLT_MAX	3.40282346638528859812e+38F
#define DBL_MAX	1.79769313486231570815e+308L

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
#ifndef uint32
#define uint32
typedef unsigned int uint32_t;
#endif
typedef signed int int32_t;

#endif
