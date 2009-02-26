#ifndef _COMMON_H_
#define _COMMON_H_

#define SetBit(rg, bit)   { (rg) |=  (1UL << (bit)); }
#define ClearBit(rg, bit) { (rg) &= ~(1UL << (bit)); }
#define GetBit(rg, bit)   ( ((rg) >> (bit)) & 1 )

#define BMASK(nbit)  (0x00000001 << (nbit))

// Stop processor
#define HALT     { asm ("ta 0; nop;"); }

inline static int dummy() {
	return 0;
}

#ifdef SIMULATE
#define TRACE(format, args...)		dummy()
#define assert(cond)
#else
#define TRACE(format, args...)   printf(format, ##args)
#define assert(cond)	{\
		if (!(cond)){\
			printf("\nASSERTION FAILED at %s, line %d:" #cond "\n", __FILE__, __LINE__);\
			HALT;\
		}\
}
#endif  // ifdef SIMULATE
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

void *memcpy(void *OUT, const void* IN, unsigned int size);

#endif //_COMMON_H_
