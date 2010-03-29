/**
 * @file
 *
 * @date 27.11.09
 * @author Nikolay Korotky
 */
#ifndef BITOPS_H_
#define BITOPS_H_

#include <asm/bitops.h>
#include <hal/reg.h>

#define set_bit(rg, bit)  do { REG_ORIN(rg, (1UL << (bit))); } while(0)
#define clear_bit(rg, bit) do { REG_ANDIN(rg, ~(1UL << (bit))); } while(0)
#define get_bit(rg, bit)   ( (REG_LOAD(rg) >> (bit)) & 1 )

#endif /* BITOPS_H_ */
