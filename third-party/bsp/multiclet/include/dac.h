//DACx constant file
#ifndef DAC_h
#define DAC_h

#include "R1_ccf.h"

typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t DATA;

} DAC_TypeDef;

#define APB1PERIPH_BASE       (0xC0100000)

//GPIOA
#define DAC0_BASE            (APB1PERIPH_BASE + 0x000D1000)
#define DAC0                 ((DAC_TypeDef *) DAC0_BASE)
//GPIOB
#define DAC1_BASE            (APB1PERIPH_BASE + 0x000D1100)
#define DAC1                 ((DAC_TypeDef *) DAC1_BASE)

//adc constant and macro




#endif
