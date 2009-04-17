#ifndef _COMMON_H_
#define _COMMON_H_

#include "conio.h"

#define SetBit(rg, bit)   { (rg) |=  (1UL << (bit)); }
#define ClearBit(rg, bit) { (rg) &= ~(1UL << (bit)); }
#define GetBit(rg, bit)   ( ((rg) >> (bit)) & 1 )

#define BMASK(nbit)  (0x00000001 << (nbit))

// Stop processor
#define HALT     { asm ("ta 0; nop;"); }

inline static int dummy() {
	return 0;
}


#define CHECK3(X)	if(X!=1 && X!=2 && X!=3) return -1;

#ifdef SIMULATE
#define TRACE(format, args...)		dummy()
#define LOGGER()     dummy()
#define assert(cond)	{}
#else
#define TRACE(format, args...)   printf(format, ##args)

#ifdef _LOGGER_SYSTEM
#define LOGGER()     TRACE("%s (%s:%d)\n", __FUNCTION__, __FILE__, __LINE__)
#else
#define LOGGER()     dummy()
#endif // ifdef _LOGGER_SYSTEM

#define assert(cond)	{\
		if (!(cond)){\
			printf("\nASSERTION FAILED at %s, line %d:" #cond "\n", __FILE__, __LINE__);\
			HALT;\
		}\
}
#endif  // ifdef SIMULATE
#define array_len(array)		(sizeof(array) / sizeof(array[0]))

// mathematics
#define max(i, j)   (((i) > (j)) ? (i) : (j))
#define min(i, j)   (((i) < (j)) ? (i) : (j))

#define abs(i)   max((i), -(i))

// check if 'num' is between 'min' and 'max'
#define between(num, min, max) \
    (((num) >= (min)) && ((num) <= (max)))
// check if ch is HEX digit
#define hex_digit(ch)           \
   (   between((ch),'0','9')    \
    || between((ch),'a','f')    \
    || between((ch),'A','F') )


#ifdef _TEST_SYSTEM_
#define OETH_REGLOAD(a)	a
#define OETH_REGSAVE(a,v) a=v
#define OETH_REGORIN(a,v) (OETH_REGSAVE(a,(OETH_REGLOAD(a) | (v))))
#define OETH_REGANDIN(a,v) (OETH_REGSAVE(a,(OETH_REGLOAD(a) & (v))))
#else
#define TRACE(format, args...)   printk(format, ##args)
#define OETH_REGLOAD(a)	(LEON3_BYPASS_LOAD_PA(&(a)))
#define OETH_REGSAVE(a,v) (LEON3_BYPASS_STORE_PA(&(a),v))
#define OETH_REGORIN(a,v) (OETH_REGSAVE(a,(OETH_REGLOAD(a) | (v))))
#define OETH_REGANDIN(a,v) (OETH_REGSAVE(a,(OETH_REGLOAD(a) & (v))))
#endif //_TEST_SYSTEM_


#endif //_COMMON_H_
