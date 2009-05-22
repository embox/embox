#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef _TEST_SYSTEM_
    #include "conio.h"

    #define SetBit(rg, bit)   { (rg) |=  (1UL << (bit)); }
    #define ClearBit(rg, bit) { (rg) &= ~(1UL << (bit)); }
    #define GetBit(rg, bit)   ( ((rg) >> (bit)) & 1 )

    #define BMASK(nbit)  (0x00000001 << (nbit))
    // Stop processor
    #define HALT     { asm ("ta 0; nop;"); }
#endif //_TEST_SYSTEM_

inline static int dummy() {
	return 0;
}

#ifdef _ERROR
    #define LOG_ERROR(format, args...)  printf("ERROR: "format, ##args)
#else
    #define LOG_ERROR(format, args...)  dummy()
#endif //_ERROR

#ifdef _WARN
    #define LOG_WARN(format, args...)   printf("WARN: "format, ##args)
#else
    #define LOG_WARN(format, args...)   dummy()
#endif //_WARN

#ifdef _DEBUG
    #define LOGGER()		    printf("%s (%s:%d)\n", __FUNCTION__, __FILE__, __LINE__)
    #define LOG_DEBUG(format, args...)  LOGGER(); printf("DEBUG: "format, ##args)
#else
    #define LOGGER()                dummy()
    #define LOG_DEBUG(format, args...)  dummy()
#endif //_DEBUG

#if defined(_TRACE) && !defined(SIMULATE)
    #ifdef _TEST_SYSTEM_
	#define TRACE(format, args...)  printf(format, ##args)
    #else
	#define TRACE(format, args...)  printk(format, ##args)
    #endif //_TEST_SYSTEM_
#else
    #define TRACE(format, args...)  dummy()
#endif //_TRACE && !SIMULATE

#ifdef SIMULATE
    #define assert(cond)	{}
#else
    #define assert(cond)	{\
		if (!(cond)){\
			printf("\nASSERTION FAILED at %s, line %d:" #cond "\n", __FILE__, __LINE__);\
			HALT;\
		}\
}
#endif //SIMULATE

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
    #define REG_LOAD(a)	a
    #define REG_STORE(a,v) a=v
    #define REG_ORIN(a,v) (REG_STORE(a,(REG_LOAD(a) | (v))))
    #define REG_ANDIN(a,v) (REG_STORE(a,(REG_LOAD(a) & (v))))
#else
    #define REG_LOAD(a)	(LEON3_BYPASS_LOAD_PA(&(a)))
    #define REG_STORE(a,v) (LEON3_BYPASS_STORE_PA(&(a),v))
    #define REG_ORIN(a,v) (REG_STORE(a,(REG_LOAD(a) | (v))))
    #define REG_ANDIN(a,v) (REG_STORE(a,(REG_LOAD(a) & (v))))
#endif //_TEST_SYSTEM_

#endif //_COMMON_H_
