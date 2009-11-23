#ifndef _COMMON_H_
#define _COMMON_H_

#include "autoconf.h"
#include "types.h"
#include "stdio.h"

#ifdef _TEST_SYSTEM_

#define SetBit(rg, bit)  do { REG_ORIN(rg, (1UL << (bit))); } while(0)
#define ClearBit(rg, bit) do { REG_ANDIN(rg, ~(1UL << (bit))); } while(0)
#define GetBit(rg, bit)   ( ((rg) >> (bit)) & 1 )

#define BMASK(nbit)  (0x00000001 << (nbit))
// TODO HALT should gracefully stop the processor. -- Eldar
#define HALT do ; while(1)
#endif /* _TEST_SYSTEM_ */

#define LOGGER()		    printf("%s (%s:%d) ", __FUNCTION__, __FILE__, __LINE__)

#if defined(_ERROR) && !defined(SIMULATION_TRG)
# define LOG_ERROR(...)  do {LOGGER(); printf("ERROR: "__VA_ARGS__);} while(0)
#else
# define LOG_ERROR(...)  do ; while(0)
#endif

#if defined(_WARN) && !defined(SIMULATION_TRG)
# define LOG_WARN(format, args...)   printf("WARN: "format, ##args)
#else
# define LOG_WARN(...)   do ; while(0)
#endif

#if defined(_DEBUG) && !defined(SIMULATION_TRG)
# define LOG_DEBUG(...)  do {LOGGER(); printf("DEBUG: "__VA_ARGS__);} while(0)
#else
# define LOG_DEBUG(...)  do ; while(0)
#endif

#if defined(_TRACE) && !defined(SIMULATION_TRG)
# ifdef _TEST_SYSTEM_
#  define TRACE(...)  printf(__VA_ARGS__)
# else
#  define TRACE(...)  printk(__VA_ARGS__)
# endif
#else
# define TRACE(...)  do ; while(0)
#endif

#ifdef SIMULATION_TRG
# define assert(cond)	do ; while(0)
#else
# define __ASSERT_STRING0(cond, file, line) \
	"\nASSERTION FAILED at " #file " : " #line "\n" \
	"(" cond ") is not TRUE\n"
# define __ASSERT_STRING(cond, file, line) \
	__ASSERT_STRING0(cond, file, line)
# define assert(cond) \
	do if (!(cond)) { \
		puts(__ASSERT_STRING(#cond, __FILE__, __LINE__)); \
		HALT; \
	} while(0)
#endif /* SIMULATION_TRG */

#define assert_null(arg) assert((arg) == NULL)
#define assert_not_null(arg) assert((arg) != NULL)

#define PRINTREG32_BIN(reg) {int i=0; for(;i<32;i++) TRACE("%d", (reg>>i)&1); TRACE(" (0x%x)\n", reg);}
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
