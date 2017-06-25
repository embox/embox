//PLL constant file
#ifndef PLL_H
#define PLL_H

#include "R1_ccf.h"

#define PLL_8_100_RCNT 0x4
#define PLL_8_100_NCNT 0x2E
#define PLL_8_100_MXVCO 0xC




int asm_set_PLL(unsigned char RCNT, unsigned char NCNT, unsigned char MXVCO, unsigned char Enable);
#endif
