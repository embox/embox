#ifndef _COMMON_H_
#define _COMMON_H_

#include "autoconf.h"

#ifdef _TEST_SYSTEM_
//#include "conio.h"

#define SetBit(rg, bit)   { REG_ORIN(rg, (1UL << (bit))); }
#define ClearBit(rg, bit) { REG_ANDIN(rg, ~(1UL << (bit))); }
//#define GetBit(rg, bit)   ( ((rg) >> (bit)) & 1 )

#define BMASK(nbit)  (0x00000001 << (nbit))
// Stop processor
//TODO this depends on the architecture
#define HALT     { __asm__ ("nop;"); }
#endif //_TEST_SYSTEM_

#define LOGGER()		    printf("%s (%s:%d) ", __FUNCTION__, __FILE__, __LINE__)
#if defined(_ERROR) && !defined(SIMULATION_TRG)
#define LOG_ERROR(...)  do {LOGGER(); printf("ERROR: "__VA_ARGS__);} while(0)
#else
#define LOG_ERROR(...)  do ; while(0)
#endif //_ERROR

#if defined(_WARN) && !defined(SIMULATION_TRG)
#define LOG_WARN(format, args...)   printf("WARN: "format, ##args)
#else
#define LOG_WARN(...)   do ; while(0)
#endif //_WARN

#if defined(_DEBUG) && !defined(SIMULATION_TRG)
#define LOG_DEBUG(...)  do {LOGGER(); printf("DEBUG: "__VA_ARGS__);} while(0)
#else
#define LOG_DEBUG(...)  do ; while(0)
#endif //_DEBUG

#if defined(_TRACE) && !defined(SIMULATION_TRG)
#ifdef _TEST_SYSTEM_
#define TRACE(...)  printf(__VA_ARGS__)
#else
#define TRACE(...)  printk(__VA_ARGS__)
#endif //_TEST_SYSTEM_
#else
#define TRACE(...)  do ; while(0)
#endif //_TRACE && !SIMULATION_TRG
#ifdef SIMULATION_TRG
#define assert(cond)	{}
#else

#include "asm/types.h"

#define PRINTREG32_BIN(reg) {int i=0; for(;i<32;i++) TRACE("%d", (reg>>i)&1); TRACE(" (0x%x)\n", reg);}

//extern unsigned long old_psr;
//extern unsigned long new_psr;
//			printf("\nold_psr_icc: %x\nnew_psr_icc: %x", old_psr & PSR_ICC, new_psr & PSR_ICC);

#define assert(cond)	{\
		if (!(cond)){\
			printf("\nASSERTION FAILED at %s, line %d:" #cond "\n", __FILE__, __LINE__);\
			HALT;\
		}\
}
#endif //SIMULATION_TRG
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
