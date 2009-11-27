/**
 * \file bitops.h
 * \date 27.11.09
 * \author sikmir
 */
#ifndef BITOPS_H_
#define BITOPS_H_

#define SetBit(rg, bit)  do { REG_ORIN(rg, (1UL << (bit))); } while(0)
#define ClearBit(rg, bit) do { REG_ANDIN(rg, ~(1UL << (bit))); } while(0)
#define GetBit(rg, bit)   ( ((rg) >> (bit)) & 1 )
#define BMASK(nbit)  (0x00000001 << (nbit))

#endif /* BITOPS_H_ */
