//ADCx constant file
#ifndef ADC_h
#define ADC_h

#include "R1_ccf.h"

typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t CH0;
  __IO uint32_t CH1;
  __IO uint32_t CH2;
  __IO uint32_t CH3;

} ADC_TypeDef;

#define APB1PERIPH_BASE       (0xC0100000)

//GPIOA
#define ADC0_BASE            (APB1PERIPH_BASE + 0x000D0000)
#define ADC0                 ((ADC_TypeDef *) ADC0_BASE)
//GPIOB
#define ADC1_BASE            (APB1PERIPH_BASE + 0x000D0100)
#define ADC1                 ((ADC_TypeDef *) ADC1_BASE)

//adc constant and macro




#endif
