#ifndef R1_CCF_H
#define R1_CCF_H

#define     __I     volatile const
#define     __O     volatile
#define     __IO    volatile

#ifndef uint32
#define uint32
typedef unsigned int uint32_t;
#endif

//bit macro
#define SET_BIT(ELEMENT, BIT_NUM) ELEMENT |= (1 << BIT_NUM));
#define CLR_BIT(ELEMENT, BIT_NUM) ELEMENT &= (~(1 << BIT_NUM));

#endif
